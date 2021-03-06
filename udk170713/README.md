various
--------------------

more questions and ideas that have come up.

fader shader
--

here is a useful shader to have installed on the main camera. it can smooth your side borders as well as mask your screen to look like a circle in the middle. all edges have size settings so the mask can be either sharp or smooth.

* add the following javascript to your Main Camera 

```javascript
#pragma strict

public var mat: Material;
public var fade: float = 1.0;
public var posx: float = 0.05;
public var posy: float = 0.05;
public var sizex: float = 0.1;
public var sizey: float = 0.1;
public var rad: float = 0.707;
public var radsize: float = 0.1;

function OnRenderImage(src: RenderTexture, dest: RenderTexture) {
    mat.SetFloat("_Fade", fade);
    mat.SetFloat("_PosX", posx);
    mat.SetFloat("_PosY", posy);
    mat.SetFloat("_SizeX", Mathf.Max(sizex, 0.0));
    mat.SetFloat("_SizeY", Mathf.Max(sizey, 0.0));
    mat.SetFloat("_Rad", rad);
    mat.SetFloat("_RadSize", radsize);
    Graphics.Blit(src, dest, mat);
}
```

* select Assets / Create / Shader / Image Effect Shader
* optionally give it a name (here 'fadeshader')
* doubleclick the shader icon to open it in MonoDevelop
* replace what is there with the code below

```
Shader "MyShaders/fadeshader" {
    Properties {
        _MainTex ("Texture", 2D) = "white" {}
        _Fade ("Fade", float) = 1.0
        _PosX ("PosX", float) = 0.05
        _PosY ("PosY", float) = 0.05
        _SizeX ("SizeX", float) = 0.1
        _SizeY ("SizeY", float) = 0.1
        _Rad ("Rad", float) = 0.1
        _RadSize ("RadSize", float) = 0.1
    }
    SubShader {
        Cull Off ZWrite Off ZTest Always
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
            v2f vert (appdata v) {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }
            sampler2D _MainTex;
            float _Fade, _PosX, _PosY, _SizeX, _SizeY, _Rad, _RadSize;
            fixed4 frag (v2f i) : SV_Target {
                fixed4 col = tex2D(_MainTex, i.uv);
                float fadex1= smoothstep(_PosX*0.5, _PosX*0.5+_SizeX, i.uv.x);
                float fadex2= smoothstep(_PosX*0.5, _PosX*0.5+_SizeX, 1-i.uv.x);
                float fadey1= smoothstep(_PosY*0.5, _PosY*0.5+_SizeY, i.uv.y);
                float fadey2= smoothstep(_PosY*0.5, _PosY*0.5+_SizeY, 1-i.uv.y);
                float faderad= 1-smoothstep(_Rad, _Rad+_RadSize, distance(float2(0.5, 0.5), i.uv));
                return col*_Fade*fadex1*fadex2*fadey1*fadey2*faderad;
            }
            ENDCG
        }
    }
}
```

* select Assets / Create / Material
* optionally give it a name (here 'fadematerial')
* make sure the Shader in inspector is set to 'MyShaders/fadeshader'
* select the Main Camera and in its inspector drag&drop the fadematerial icon onto the 'Mat' slot
* run and you should see a rectangular mask

now in the Main Camera inspector you should see some parameters to control. play around with them. 'Fade' works like a main fader. 0.0 is all black and 1.0 is normal. you can also go beyond 1.0 to brighten up the graphics. 'Posx' and 'Posy' are 0.0 to 1.0 positions of where the edges start, 'Sizex' and 'Sizey' are how wide or long the edge section should be. 'Rad' determines the radius of the circle (set this to >= 0.707 if not used) and 'Radsize' is again how much of a smooth edge there should be.

![mask](00mask.png?raw=true "mask")

this script can also be used as a main fader for your live visuals. by setting posx, posy, sizex and sizey to 0.0 and then only controlling the fade parameter, you can bring in and out your graphics to/from black in a nice way. perhaps set up midi or osc receiver code to control this fade parameter from a midi controller or from your supercollider sound patch (by adapting the code [here](https://github.com/redFrik/udk16-Immersive_Technologies/tree/master/udk170112#midi-input) or [here](https://github.com/redFrik/udk17-Digital_Harmony/tree/master/udk170511#sc-to-unity))

fullscreen
--

connect a video projector, turn off screen mirroring and set its resolution to 1920x1080 (= 1080p). on mac osx you do all this in System Preferences / Displays. if projector or monitor does not have 1920x1080, try to figure out its native resolution by reading the manual and then adapt the numbers in the script below.

* use an existing project or create a new unity project (add at least a cube to see something)
* select the Main Camera, 'Add Component' and then 'New Script'
* give the script a name (here 'display2') and make sure language is JavaScript
* doubleclick the script symbol and paste in the code here below

```javascript
#pragma strict

function Start() {
    var beamerwidth= 1920;  //edit to match video projector width
    var beamerheight= 1080;	//edit to match video projector height
    Debug.Log(Screen.currentResolution);
    var laptopwidth= Screen.currentResolution.width;
    Screen.SetResolution(laptopwidth+beamerwidth, beamerheight, false);
    if(Display.displays.Length>1) {
        Display.displays[1].Activate();
        Display.displays[1].SetParams(beamerwidth, beamerheight, laptopwidth, 0);
    }
}
```

* save and close
* back in unity select File / Build Settings...
* click 'Player Settings...'
* set 'Display Resolution Dialog' to 'Disabled'
* deselect 'Default Is Full Screen'
* enter 1920 and 1080 for default screen width and height
* click 'Build And Run', give your application a name and save

![playersettings](01playersettings.png?raw=true "playersettings")

NOTE: at least on mac osx this will now only work every second time you run the app (weird bug). to get around that open Terminal and type `/Users/asdf/unity/2nd_screen/2nd.app/Contents/MacOS/2nd -screen-height 900 -screen-width 1440` but adapt the path and app name to match your system (i.e. where you saved your app when you built it). quit with ctrl+c

NOTE: all the above is kind of a hack and temporary solution in current unity (5.6). hopefully in the future unity will have better multi-display support. see discussion [here](https://stackoverflow.com/questions/43066541/unity-multiple-displays-not-working)

syphon multi display
--

NOTE: this section is for ***macOs*** only. for Windows there is a similar texture sending application here... <https://spout.zeal.co> that should be able to do something similar.

because unity have big problems with multi-display setups, one can use syphon and these simple max patches to deal with the window management. in this directory you will find four max patches that help run your unity scenes with 2-3 projectors. if you don't have max they should run in the demo version of [MaxMSPJitter](https://cycling74.com/downloads). you will need to install the Syphon package in max's package manager.

**syphonmulti2.maxpat** and **syphonmulti3.maxpat** takes two or three syphon inputs and display them on as many projectors/displays. so with this you can for example have three cameras looking in your scene, project them next to each other and get kind of three separate views of the same object. to try it out install the Funnel package (see [here](https://github.com/redFrik/udk17-Digital_Harmony/tree/master/udk170706#syphon)) and then add the Funnel C# script to all three cameras. set the resolution to 1920x1080, and importantly tick run in background. run and then open the max patch (syphonmulti2 for two cameras + two projectors, syphonmulti3 for three). click start and select a different syphone server (unity camera) for each syphon client. last click set up windows.

**syphonwide2.maxpat** and **syphonwide3.maxpat** takes a single syphon input and spreads it over two or three projectors. so with this you can for example have a unity camera/scene that fills a whole room, projecting on three walls. to try it out install the Funnel package (see [here](https://github.com/redFrik/udk17-Digital_Harmony/tree/master/udk170706#syphon)) and then add the Funnel C# script to all three cameras. for two projectors set the funnel resolution to 3840x1080 and for three projectors use 5760x1080 (so we will be sending very wide textures from unity). next tick run in background, open the max patch (syphonwide2 for two projectors, syphonwide3 for three). click start and select a different syphone server (unity camera) for each syphon client. last click set up windows.

NOTE: the above might be running slow on your computer (specially the multi camera patches). watch the framerate and set the funnel script to 'Send Only'. it's not an optimal solution but might work in some cases (until unity is updated and the multi display problems are solved).

NOTE: instead of max this could be done using mapmapper, resolume, processing, openframeworks or some other syphon enabled software.

supercollider
--

more on patterns (advanced)

read sc helpfiles for `Pseq`, `Prand`, `Pwrand`, `Pseg`, `Pstutter` etc.

```supercollider
s.boot;

(
SynthDef(\pong, {|out= 0, freq= 400, pan= 0, amp= 0.5, mod= 0.1, atk= 0.005, rel= 0.1, cur= -4, gate= 1|
    var env= EnvGen.ar(Env.asr(atk, amp, rel, cur), gate, doneAction:2);
    var snd= SinOscFB.ar(freq-env, mod, 2**mod).tanh;
    Out.ar(out, Pan2.ar(snd, pan, env));
}).add;
)

//run these one at a time - look at the code what is added each time
Pdef(\pong, Pbind(\instrument, \pong, \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;
Pdef(\pong, Pbind(\instrument, \pong, \dur, 0.25,   \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;
Pdef(\pong, Pbind(\instrument, \pong, \dur, Pseq([0.25, 0.125, 0.25], inf),   \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;
Pdef(\pong, Pbind(\instrument, \pong, \degree, Pseq([0, 1, 3, 4], inf), \pan, Pwhite(-0.6, 0.6, inf),   \dur, Pseq([0.25, 0.125, 0.25], inf), \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;
Pdef(\pong, Pbind(\instrument, \pong, \degree, Pseq([0, 1, 3, 4, 5], inf),   \pan, Pwhite(-0.6, 0.6, inf), \dur, Pseq([0.25, 0.125, 0.25], inf), \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;
Pdef(\pong, Pbind(\instrument, \pong, \octave, Pseq([5, 5, 5, 6], inf),   \degree, Pseq([0, 1, 3, 4, 5], inf), \pan, Pwhite(-0.6, 0.6, inf), \dur, Pseq([0.25, 0.125, 0.25], inf), \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;
Pdef(\pong, Pbind(\instrument, \pong, \mod, Pseg(Pseq([0, 1, 0], inf), Pseq([6, 6, 6], inf)),   \octave, Pseq([5, 5, 5, 6], inf), \degree, Pseq([0, 1, 3, 4, 5], inf), \pan, Pwhite(-0.6, 0.6, inf), \dur, Pseq([0.25, 0.125, 0.25], inf), \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;
Pdef(\pong, Pbind(\instrument, \pong, \cur, Pseq([-5, -5, 5], inf),   \mod, Pseg(Pseq([0, 1, 0], inf), Pseq([5, 5, 5], inf)), \octave, Pseq([5, 5, 5, 6], inf), \degree, Pseq([0, 1, 3, 4, 5], inf), \pan, Pwhite(-0.6, 0.6, inf), \dur, Pseq([0.25, 0.125, 0.25], inf), \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;
Pdef(\pong, Pbind(\instrument, \pong, \scale, Scale.lydian(Tuning.just),   \cur, Pseq([-5, -5, 5], inf), \mod, Pseg(Pseq([0, 1, 0], inf), Pseq([5, 5, 5], inf)), \octave, Pseq([5, 5, 5, 6], inf), \degree, Pseq([0, 1, 3, 4, 5], inf), \pan, Pwhite(-0.5, 0.5, inf), \dur, Pseq([0.25, 0.125, 0.25], inf), \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;
Pdef(\pong, Pbind(\instrument, \pong, \mtranspose, Pstutter(32, Pseq([0, 1], inf)),   \scale, Scale.lydian(Tuning.just), \cur, Pseq([-5, -5, 5], inf), \mod, Pseg(Pseq([0, 1, 0], inf), Pseq([5, 5, 5], inf)), \octave, Pseq([5, 5, 5, 6], inf), \degree, Pseq([0, 1, 3, 4, 5], inf), \pan, Pwhite(-0.5, 0.5, inf), \dur, Pseq([0.25, 0.125, 0.25], inf), \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;
(
SynthDef(\dist, {|in= 50, out= 0, freq= 500, amount= 25, detune= 1, mix= 1|  //mix is -1 (dry) to 1 (wet)
    var snd= In.ar(in, 2);
    var efx= (snd*amount).tanh*SinOsc.ar(freq+[0, detune], 1, 2/amount);
    Out.ar(out, XFade2.ar(snd, efx, mix));
}).add;
)
e.free; e= Synth(\dist);

Pdef(\pong, Pbind(\instrument, \pong, \out, Pwrand([0, 50], [0.9, 0.1], inf),   \mtranspose, Pstutter(32, Pseq([0, 1], inf)), \scale, Scale.lydian(Tuning.just), \mod, Pseg(Pseq([0, 1, 0], inf), Pseq([5, 5, 5], inf)), \octave, Pseq([5, 5, 5, 6], inf), \degree, Pseq([0, 1, 3, 4, 5], inf), \pan, Pwhite(-0.5, 0.5, inf), \dur, Pseq([0.25, 0.125, 0.25], inf), \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;

(
SynthDef(\verb, {|in= 52, out= 0, room= 0.75, mix= 0.75|  //mix is -1 (dry) to 1 (wet)
    var snd= In.ar(in, 2);
    var efx= FreeVerb2.ar(snd[0], snd[1], mix+1*0.5, room);
    Out.ar(out, efx);
}).add;
)
f.free; f= Synth(\verb);

Pdef(\pong, Pbind(\instrument, \pong, \out, Pseq([0!8, 50, 0!3, 51].flat, inf),   \mtranspose, Pstutter(32, Pseq([0, 1], inf)), \scale, Scale.lydian(Tuning.just), \cur, Pseq([-5, -5, 5], inf), \mod, Pseg(Pseq([0, 1, 0], inf), Pseq([5, 5, 5], inf)), \octave, Pseq([5, 5, 5, 6], inf), \degree, Pseq([0, 1, 3, 4, 5], inf), \pan, Pwhite(-0.5, 0.5, inf), \dur, Pseq([0.25, 0.125, 0.25], inf), \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;

Pdef(\pong, Pbind(\instrument, \pong, \amp, Pseq([0, Pwhite(0, 0.05, 1)], inf), \rel, Pseq([2.5, 1.5], inf),   \out, Pseq([0!8, 50, 0!3, 51].flat, inf), \mtranspose, Pstutter(32, Pseq([0, 1], inf)), \scale, Scale.lydian(Tuning.just), \cur, Pseq([-5, -5, 5], inf), \mod, Pseg(Pseq([0, 1, 0], inf), Pseq([5, 5, 5], inf)), \octave, Pseq([5, 5, 5, 6], inf), \degree, Pseq([0, 1, 3, 4, 5], inf), \pan, Pwhite(-0.5, 0.5, inf), \dur, Pseq([0.25, 0.125, 0.25], inf), \legato, Pseq([0.1, 0.2, 0.4, 0.8], inf))).play;

Pdef(\pong).stop;
```

patterns can be nested. compare...

```supercollider
//compare...
Pdef(\myseq, Pbind(\instrument, \pong, \dur, Pseq([0.5, 0.25, 0.25, 0.25, 0.125], inf), \legato, 0.3, \degree, Pseq([0, 0, 7, 6, 5, 4, 3, 2, 1], inf))).play;
Pdef(\myseq, Pbind(\instrument, \pong, \dur, Pseq([0.5, Pseq([0.25], 3), 0.125], inf), \legato, 0.3, \degree, Pseq([0, 0, 7, 6, 5, 4, 3, 2, 1], inf))).play;
//they should be the same but the second line uses a nested pattern that repeat the value 0.25 three times 

//but now you can do...
Pdef(\myseq, Pbind(\instrument, \pong, \dur, Pseq([0.5, Prand([0.125, 0.0625], 5), 0.125], inf), \legato, 0.3, \degree, Pseq([0, 0, 7, 6, 5, 4, 3, 2, 1], inf))).play;
//or...
Pdef(\myseq, Pbind(\instrument, \pong, \dur, Pseq([0.5, Prand([Pseq([0.125], 3), 0.0625], 5), 0.125], inf), \legato, 0.3, \degree, Pseq([0, 0, 7, 6, 5, 4, 3, 2, 1], inf))).play;
//for three levels deep nesting
```

also math operations are possible directly on patterns. you can add or multiply with a single float or even with other patterns.

```supercollider
Pdef(\myseq, Pbind(\instrument, \pong, \amp, Pseq([0, 0.5, 0.25, 0.25, 0.25, 0.25, 0.25], inf)*0.5, \dur, Pseq([0.5, 0.25, 0.25, 0.25, 0.125], inf), \legato, 0.3, \degree, Pseq([0, 0, 7, 6, 5, 4, 3, 2, 1], inf)+5)).play;

Pdef(\myseq, Pbind(\instrument, \pong, \amp, Pseq([0, 0.5, 0.25, 0.25, 0.25, 0.25, 0.25], inf)*0.5, \dur, Pseq([0.5, 0.25, 0.25, 0.25, 0.125], inf), \legato, 0.3, \degree, Pseq([0, 0, 7, 6, 5, 4, 3, 2, 1], inf)+Pseq([-7, 0], inf))).play;
```

we do not need to use the simple pong sound, but can also load a soundfile and play that as our instrument. note that there's no freq argument any more so degree, scale, octave will not work (unless you use freq for filter settings or connect freq to rate (playbackrate) somehow). but most parameters will work like dur, legato, amp, mod, pan, atk, rel etc. 

```supercollider
b= Buffer.readChannel(s, "/Users/asdf/Desktop/ND_BreakC125-05.wav", channels:[0]);
(
SynthDef(\sampler, {|out= 0, pan= 0, buf, mod= 0, amp= 0.5, rate= 1, atk= 0.005, rel= 0.1, cur= -4, gate= 1|
    var env= EnvGen.ar(Env.asr(atk, amp, rel, cur), gate, doneAction:2);
    var snd= PlayBuf.ar(1, buf, rate, Impulse.ar(mod*50));  //Impulse could be Dust
    Out.ar(out, Pan2.ar(snd, pan, env));
}).add;
)

Pdef(\myseq, Pbind(\instrument, \sampler, \buf, b, \dur, 0.5, \amp, 0.2)).play;
Pdef(\myseq, Pbind(\instrument, \sampler, \buf, b, \dur, Pseq([Pseq([0.1], 10), Pseries(0.1, -0.002, 40)], inf))).play;
Pdef(\myseq, Pbind(\instrument, \sampler, \buf, b, \mod, Pseq([0, 0, 0.1, 1], inf), \dur, Pseq([Pseq([0.1], 10), Pseries(0.1, -0.002, 40)], inf))).play;
Pdef(\myseq, Pbind(\instrument, \sampler, \buf, b, \dur, 0.1, \rate, Pwrand([0, 5, 4, 3, 2, 1], [0.05, 0.15, 0.1, 0.5, 0.1, 0.1], inf)+1)).play;
Pdef(\myseq, Pbind(\instrument, \sampler, \buf, b, \dur, 0.1, \legato, 1.5, \rel, 0.5, \pan, Pwhite(-1.0, 1.0), \rate, Pwrand([0, 5, 4, 3, 2, 1], [0.05, 0.15, 0.1, 0.5, 0.1, 0.1], inf)+1)).play;
```

question: how to change individual patterns while it is running and how to add a gui slider?

```supercollider
Pdef(\myseq, Pbind(\instrument, \pong, \dur, Pdefn(\mydur, 0.1), \degree, Pseq([0, 5, 4, 3, 2, 1], inf))).play;
Pdefn(\mydur, 0.05);
Pdefn(\mydur, Pseq([0.01, 0.1], inf));
Pdefn(\mydur, Pseq([0.01, 0.1, 0.05, 0.2], inf));
(
var w= Window().front;
Slider(w).action_({|v| Pdefn(\mydur, v.value.linlin(0, 1, 0.01, 0.2))});
CmdPeriod.doOnce({w.close});
)
```

question: how to write your own patterns functions similar to ugens? one way is to use `Pfunc` like this...

```supercollider
Pdef(\myseq, Pbind(\instrument, \pong, \cnt, Pseries(0, 1), \dur, Pfunc({|ev| sin(ev.cnt*0.15)*0.1+0.125}), \degree, Pseq([0, 5, 4, 3, 2, 1], inf))).play;
```

or one can install additional patterns that are similar to the built in ugens...

```supercollider
Quarks.install("UGenPatterns");  //after installing you will need to recompile and start again
Pdef(\myseq, Pbind(\instrument, \pong, \dur, PSinOsc(40)*0.1+0.125, \degree, Pseq([0, 5, 4, 3, 2, 1], inf))).play;
Pdef(\myseq, Pbind(\instrument, \pong, \dur, PSaw(40)*0.1+0.125, \degree, Pseq([0, 5, 4, 3, 2, 1], inf))).play;
Pdef(\myseq, Pbind(\instrument, \pong, \dur, PPulse(10, 0.25)*0.1+0.125, \degree, Pseq([0, 5, 4, 3, 2, 1], inf))).play;
```

projectors
--

on recent mac laptops one can easily connect and output graphics to three projectors at the same time. i use one hdmi output and two thunderbolt-to-hdmi adapters for my mid-2015 machine. 

when you connect all three you should see this in your System Preferences / Displays  

![systempref](02systempref.png?raw=true "systempref")

things to check with projectors...

* never turn off the power to the projector without letting it cool off (wait for the fan to stop spinning)
* lamp setting (sometimes you want eco mode to save the lamp - sometimes not)
* keystone
* colour profile
* turn off screensavers, nightshift, f.lux, notifications etc in your system and other things that can pop up during performance 
* bring small wooden sticks to put under the feet to adjust the projector

extra
--

unity tutorials by Board To Bits Games <https://www.youtube.com/channel/UCifiUB82IZ6kCkjNXN8dwsQ/videos>

unity tips and tutorials by SpeedTutor <https://www.youtube.com/user/SpeedTutor/videos> 

more on shaders <https://thebookofshaders.com>

Eli Fieldsteel's supercollider tutorials <https://www.youtube.com/playlist?list=PLPYzvS8A_rTaNDweXe6PX4CXSGq4iEWYC>

supercollider code sharing site <https://sccode.org>

supercollider wiki <https://supercollider.github.io/pages>

supercollider mailing list <https://www.birmingham.ac.uk/facilities/ea-studios/research/supercollider/mailinglist.aspx>

pattern guide <https://doc.sccode.org/Tutorials/A-Practical-Guide/PG_01_Introduction.html>
