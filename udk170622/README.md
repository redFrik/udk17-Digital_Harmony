samples
--------------------

again we will use textures in unity. this time experimenting with writing audio sample data directly into one dimensional textures.

audio samples
--

* start unity and create a new 3D project
* select GameObject / 3D Objects / Plane (or some other object - not terrain)
* set rotation x to -45 in inspector
* click 'Add Component' in the inspector and then 'Audio / Audio Source'
* click 'Add Component' in the inspector and then 'New Script'
* give the script a name (here 'audio') and make sure language is JavaScript
* doubleclick the script symbol and paste in the code here below

```javascript
#pragma strict

private var tex : Texture2D;
private var samples : float[];
private var snd : AudioSource;
public var delaySamples = 0;  //max 44100-256= 43844

function Start() {
    snd= GetComponent.<AudioSource>();
    snd.clip= Microphone.Start(null, true, 1.0, 44100);  //1.0 is buffersize in seconds, 44100 samplerate
    snd.loop= true;
    while(!(Microphone.GetPosition(null)>0)) {}
    //snd.Play();	//monitor
    samples= new float[snd.clip.samples*snd.clip.channels];
    tex= new Texture2D(256, snd.clip.channels);
    GetComponent.<Renderer>().material.mainTexture= tex;
}
function Update() {
    var pos= Microphone.GetPosition(null);
    snd.clip.GetData(samples, 0);
    for(var y= 0; y<tex.height; y++) {
        for(var x= 0; x<tex.width; x++) {
            var sampleIndex= pos-x-delaySamples;
            if(sampleIndex<0) {
                sampleIndex= sampleIndex+samples.Length;
            }
            var sample= samples[sampleIndex];
            tex.SetPixel(x, y, Color(
                sample,	//red
                sample,	//green
                sample,	//blue
                1.0		//alpha
            ));
        }
    }
    tex.Apply();
}
```

click play in unity and tap the microphone. you should see something like this...

![mic](00mic.png?raw=true "mic")

if not check that you have 'built-in microphone' selected as sound input source in your system preferences.

try playing with the delay time in the inspector (usable range of values: 0-43844), try to change the colours and texture resolution.

now let us add a shader.

* select Assets / Create / Shader / Unlit Shader
* give it a name (here 'myshader')
* doubleclick the shader and paste in the following code...

```
Shader "Unlit/myshader" {
    Properties {
        _MainTex ("Texture", 2D) = "white" {}
    }
    SubShader {
        Pass {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #include "UnityCG.cginc"
            
            struct appdata {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };
            
            struct v2f {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };
            
            sampler2D _MainTex;
            
            v2f vert(appdata v) {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv= v.uv;
                return o;
            }

            fixed4 frag(v2f i) : SV_Target {
                fixed4 col = tex2D(_MainTex, i.uv+sin(i.uv.y*5+_Time[2]));
                return col;
            }
            ENDCG
        }
    }
}
```

and then back in the javascript code we add this line somewhere inside the Start function.

```javascript
GetComponent.<Renderer>().material.shader= Shader.Find("Unlit/myshader");
```

make sure that the shader name (here 'Unlit/myshader') matches the name at the top of the shader code above.

when you press play you should see the audio sampledata texture as wavy curves. try scaling and rotating the plane (`x= -90`) so that it fills the screen.

spectrum
--

next we will use the built in `GetSpectrumData` to access the spectrum of the audio.

```javascript
#pragma strict

private var tex : Texture2D;
private var spectrum : float[];
private var snd : AudioSource;
public var scale= 10.0;
private var bufferSize= 512;

function Start() {
    snd= GetComponent.<AudioSource>();
    snd.clip= Microphone.Start(null, true, 1.0, 44100);  //1.0 is buffersize in seconds, 44100 samplerate
    snd.loop= true;
    while(!(Microphone.GetPosition(null)>0)) {}
    snd.Play();	//monitor - must be playing for GetSpectrumData to work
    spectrum= new float[bufferSize];
    tex= new Texture2D(bufferSize, snd.clip.channels);
    GetComponent.<Renderer>().material.mainTexture= tex;
    //GetComponent.<Renderer>().material.shader= Shader.Find("Unlit/myshader");
}
function Update() {
    var pos= Microphone.GetPosition(null);
    snd.GetSpectrumData(spectrum, 0, FFTWindow.Hanning);
    for(var y= 0; y<tex.height; y++) {
        for(var x= 0; x<tex.width; x++) {
            var sample= spectrum[x]*scale;
            tex.SetPixel(x, y, Color(
                sample,	//red
                sample,	//green
                sample,	//blue
                1.0		//alpha
            ));
        }
    }
    tex.Apply();
}
```

try with and without the shader (uncomment the `GetComponent` line in the Start function).

the above will show the lower frequencies on the right hand side. to make it symmetric and mirror the lower frequencies in the middle, change the fragment shader to look like this...

```
fixed4 frag(v2f i) : SV_Target {
    float halfway= step(0.5, i.uv.x);
    fixed4 col = tex2D(_MainTex, float2((i.uv.x+0.5)*halfway, i.uv.y)+float2((1.5-i.uv.x)*(1-halfway), i.uv.y));
    return col;
}
```

try editing the shader.

here's one suggestion...
```
fixed4 col = tex2D(_MainTex, i.uv+sin(i.uv.y*5+(_Time[2]*100)))*40;
```

sonogram
--

replace the javascript with the following...

```javascript
#pragma strict

private var tex : Texture2D;
private var spectrum : float[];
private var snd : AudioSource;
public var scale= 10.0;
private var bufferSize= 512;

function Start() {
    snd= GetComponent.<AudioSource>();
    snd.clip= Microphone.Start(null, true, 1.0, 44100);  //1.0 is buffersize in seconds, 44100 samplerate
    snd.loop= true;
    while(!(Microphone.GetPosition(null)>0)) {}
    snd.Play();	//monitor - must be playing for GetSpectrumData to work
    spectrum= new float[bufferSize];
    tex= new Texture2D(bufferSize, bufferSize);  //now we use a 512x512 rectangular texture instead of 512x1
    GetComponent.<Renderer>().material.mainTexture= tex;
    //GetComponent.<Renderer>().material.shader= Shader.Find("Unlit/myshader");
}
function Update() {
    var pos= Microphone.GetPosition(null);
    snd.GetSpectrumData(spectrum, 0, FFTWindow.Hanning);
    var y= Time.frameCount%bufferSize;
    for(var x= 0; x<tex.width; x++) {
        var sample= spectrum[x]*scale;
        tex.SetPixel(x, y, Color(
            sample,	//red
            sample,	//green
            sample,	//blue
            1.0		//alpha
        ));
    }
    tex.Apply();
}
```

note how this time we set up a two dimensional texture (`Texture2D(bufferSize, bufferSize)`)

again try with and without your shader from above. try to change it to draw from left to right instead (hint: swap x and y in one place). try making it draw faster/slower (hint: scale the frameCount), change the colours (hint: `sample*0.5+0.5` and `(1-sample)*0.75` etc)

soundfiles
--

instead of the microphone you can use sound files.

just drag&drop a soundfile into the assets window and then drag&drop that onto the Audio Source's AudioClip.

it should look like this... notice the blue outlined box.

![soundfile](01soundfile.png?raw=true "soundfile")

then we change the javascript code to look like this...

```javascript
#pragma strict

private var tex : Texture2D;
private var samples : float[];
private var snd : AudioSource;

function Start() {
    snd= GetComponent.<AudioSource>();
    snd.loop= true;
    samples= new float[snd.clip.samples*snd.clip.channels];
    tex= new Texture2D(256, snd.clip.channels);
    GetComponent.<Renderer>().material.mainTexture= tex;
}
function Update() {
    snd.clip.GetData(samples, snd.timeSamples);
    for(var y= 0; y<tex.height; y++) {
        for(var x= 0; x<tex.width; x++) {
            var sample= samples[x];
            tex.SetPixel(x, y, Color(
                sample,	//red
                sample,	//green
                sample,	//blue
                1.0		//alpha
            ));
        }
    }
    tex.Apply();
}
```

and with a soundfile the spectrum example above would look like this...

```javascript
#pragma strict

private var tex : Texture2D;
private var spectrum : float[];
private var snd : AudioSource;
public var scale= 10.0;
private var bufferSize= 512;

function Start() {
    snd= GetComponent.<AudioSource>();
    snd.loop= true;
    spectrum= new float[bufferSize];
    tex= new Texture2D(bufferSize, snd.clip.channels);
    GetComponent.<Renderer>().material.mainTexture= tex;
    //GetComponent.<Renderer>().material.shader= Shader.Find("Unlit/myshader");
}
function Update() {
    snd.GetSpectrumData(spectrum, 0, FFTWindow.Hanning);
    for(var y= 0; y<tex.height; y++) {
        for(var x= 0; x<tex.width; x++) {
            var sample= spectrum[x]*scale;
            tex.SetPixel(x, y, Color(
                sample,	//red
                sample,	//green
                sample,	//blue
                1.0		//alpha
            ));
        }
    }
    tex.Apply();
}
```

and the sonogram example from above would look like this using a soundfile instead of microphone...

```javascript
#pragma strict

private var tex : Texture2D;
private var spectrum : float[];
private var snd : AudioSource;
public var scale= 10.0;
private var bufferSize= 512;

function Start() {
    snd= GetComponent.<AudioSource>();
    snd.loop= true;
    spectrum= new float[bufferSize];
    tex= new Texture2D(bufferSize, bufferSize);  //now we use a 512x512 rectangular texture instead of 512x1
    GetComponent.<Renderer>().material.mainTexture= tex;
    //GetComponent.<Renderer>().material.shader= Shader.Find("Unlit/myshader");
}
function Update() {
    snd.GetSpectrumData(spectrum, 0, FFTWindow.Hanning);
    var y= Time.frameCount%bufferSize;
    for(var x= 0; x<tex.width; x++) {
        var sample= spectrum[x]*scale;
        tex.SetPixel(x, y, Color(
            sample,	//red
            sample,	//green
            sample,	//blue
            1.0		//alpha
        ));
    }
    tex.Apply();
}
```

audacity
--

download and install Audacity from <http://www.audacityteam.org>

* select File / Import / Raw Data...
* pick a .pdf file or similar (pdf files ususally sound good, pictures, small applications etc also possible)
* experiment with the settings in the dialog that pops up but make sure to set it to use 2channels (just because the supercollider code below assumes stereo soundfiles)

you should see something like this...

![audacity](02audacity.png?raw=true "audacity")

it's usually a good idea to remove dc offset after importing raw files.

* select Effect / Normalize...
* make sure the 'Remove DC offset' box is ticked

this will make your speakers happier and last longer.

![audacity2](03audacity2.png?raw=true "audacity2")

now export this file as `.aiff`. make a few different ones

supercollider
--

```supercollider
s.boot;

//should all be stereo soundfiles
a= Buffer.read(s, "/Users/asdf/Desktop/7OUKXX.aiff");
b= Buffer.read(s, "/Users/asdf/Desktop/chrome.aiff");
c= Buffer.read(s, "/Users/asdf/Desktop/VF486M.aiff");
f= Buffer.read(s, "/Users/asdf/Desktop/green_cut1.aiff");

(
SynthDef(\player, {|buf, amp= 0.1, rate= 1, offset= 0, atk= 0.005, rel= 0.01, gate= 1|
    var env= EnvGen.kr(Env.asr(atk, amp, rel), gate, doneAction:2);
    var src= PlayBuf.ar(2, buf, rate*BufRateScale.ir(buf), 1, offset*BufFrames.ir(buf), 1);
    Out.ar(0, src*env);
}).add;
)


Pdef(\player1, Pbind(\instrument, \player, \buf, a, \dur, 0.5)).play;

Pdef(\player1, Pbind(\instrument, \player, \buf, a, \dur, 0.5, \offset, Pseq([0, 0, 0.5], inf))).play;
Pdef(\player1, Pbind(\instrument, \player, \buf, a, \dur, Pseq([0.5, 0.25, 0.25, 0.25], inf), \offset, Pseq([0, 0, 0.5], inf))).play;
Pdef(\player1, Pbind(\instrument, \player, \buf, a, \dur, Pseq([0.5, 0.25, 0.25, 0.25], inf), \offset, Pseq([0, 0, 0.5], inf), \rate, 0.5)).play;
Pdef(\player1, Pbind(\instrument, \player, \buf, a, \dur, Pseq([0.5, 0.25, 0.25, 0.25], inf), \offset, Pseq([0, 0, 0.5], inf), \rate, Pseq([0.5, -0.8], inf))).play;
Pdef(\player1, Pbind(\instrument, \player, \buf, a, \dur, Pseq([0.5, 0.25, 0.25, 0.25], inf), \offset, Pseq([0, 0, 0.5], inf), \rate, Pseq([0.5, -0.8], inf), \legato, 1)).play;
Pdef(\player1, Pbind(\instrument, \player, \buf, a, \dur, Pseq([0.5, 0.25, 0.25, 0.25], inf), \offset, Pseq([0, 0, 0.5], inf), \rate, Pseq([0.5, -0.8], inf), \legato, 1.5)).play;
Pdef(\player1, Pbind(\instrument, \player, \buf, a, \dur, Pseq([0.5, 0.25, 0.25, 0.25], inf), \offset, Pseq([0, 0, 0.5], inf), \rate, Pseq([0.5, -0.8], inf), \legato, 3)).play;


Pdef(\player2, Pbind(\instrument, \player, \buf, b, \dur, 1, \legato, 0.25, \rel, 0.5)).play;

//etc

Pdef(\player3, Pbind(\instrument, \player, \buf, c, \dur, 0.75, \offset, 0.7, \amp, 0.05)).play;

//etc

Pdef(\player4, Pbind(\instrument, \player, \buf, f, \dur, 2, \atk, 0.5, \rel, 0.5)).play;
```

one can also create raw sample data arrays in supercollider, load them into buffers and play them - a bit like the import raw in audacity. this could be sensor data, strings, functions etc etc.

```supercollider
//create a buffers from functions or other data and play it using the pdef from above
e= {|i| sin(i*0.02)}.dup(1000);  //a sine function
//e= {|i| 1.0.rand2}.dup(1000);  //white noise (rand2 gives values from -1.0 to 1.0)
//t= "some random text - could be REALLY long"; e= {|i| t.wrapAt(i).ascii}.dup(1000).normalize(-1, 1);
a.free; a= Buffer.loadCollection(s, e, 2);
Pdef(\player1, Pbind(\instrument, \player, \buf, a, \dur, 0.5)).play;
```

or we could create similar buffers but play them as controllers instead (LFOs)

```supercollider
//here a new synthdef that uses the sample data as frequencies for a sinosc - note mono buffer this time
(
SynthDef(\playerCtrl, {|buf, amp= 0.1, rate= 1, gate= 1|
    var env= EnvGen.kr(Env.asr(0, amp, 0), gate, doneAction:2);
    var ctrl= PlayBuf.kr(1, buf, rate*BufRateScale.ir(buf), 1, 0, 1);
    var src= SinOsc.ar(ctrl.linexp(-1, 1, 200, 2000));
    Out.ar(0, Pan2.ar(src, 0, env));
}).add;
)
//e= {|i| sin(i*0.02)}.dup(1000);  //a sine function
//e= {|i| 1.0.rand2}.dup(1000);  //white noise (rand2 gives values from -1.0 to 1.0)
t= "some random text - could be REALLY long"; e= {|i| t.wrapAt(i).ascii}.dup(1000).normalize(-1, 1);
a.free; a= Buffer.loadCollection(s, e, 1);
Pdef(\player2, Pbind(\instrument, \playerCtrl, \buf, a, \dur, 4)).play;
```

