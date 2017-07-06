various
--------------------

questions and ideas that have come up.

mesh distortion
--

an example of distorting or displacing mesh vertices.

* start unity and create a new 3D project
* select GameObject / 3D Objects / Sphere
* scale it up to x:5, y:5, z:5 in the inspector
* click 'Add Component' in the inspector and then 'New Script'
* give the script a name (here 'deform') and make sure language is JavaScript
* doubleclick the script symbol and paste in the code here below


```javascript
#pragma strict

public var scalex = 0.7;
public var scaley = 0.7;
public var scalez = 0.7;
public var distx = 2.0;
public var disty = 2.0;
public var distz = 2.0;
public var speedx = 1.1;
public var speedy = 1.2;
public var speedz = 1.3;
private var mesh : Mesh;
private var baseVertices : Vector3[];

function Start() {
    mesh = GetComponent(MeshFilter).mesh;
    baseVertices = mesh.vertices;
}
function Update() {
    var vertices = new Vector3[baseVertices.Length];
    for (var i= 0; i<vertices.Length; i++) {
        var vertex = baseVertices[i];
        vertex.x += Mathf.Sin((Time.time*speedx)+(vertex.x*distx))*scalex;
        vertex.y += Mathf.Sin((Time.time*speedy)+(vertex.y*disty))*scaley;
        vertex.z += Mathf.Sin((Time.time*speedz)+(vertex.z*distz))*scalez;
        vertices[i] = vertex;
    }
    mesh.vertices= vertices;
    mesh.RecalculateNormals();  //optional - can comment out if you don't care about light
    mesh.RecalculateBounds();  //also optional - comment out to save cpu
}
```

run and play around with the variables in the inspector. study the code and improve it. try attach this script to other models and prefabs.

![deform](00deform.png?raw=true "deform")

now let us add a texture coming from the built-in webcamera. 

* select the sphere (or whatever objects your deform script above is attached to)
* click 'Add Component' in the inspector and then 'New Script'
* give the script a name (here 'webcam') and make sure language is JavaScript
* doubleclick the script symbol and paste in the code here below

```javascript
#pragma strict

function Start() {
    var webcamTexture: WebCamTexture = new WebCamTexture();
    var renderer: Renderer = GetComponent.<Renderer>();
    renderer.material.mainTexture = webcamTexture;
    webcamTexture.Play();
    //GetComponent.<Renderer>().material.shader= Shader.Find("Mobile/Bumped Diffuse");
    //GetComponent.<Renderer>().material.shader= Shader.Find("Unlit/Transparent");  //this one is good if you also turn off the light
} 
```

when you click run your webcamera should start and you should see an image mapped onto the object moving around.

again play around with the variables, try the suggested shaders (uncomment one at a time in the code), turn off the default directional light, try to set main camera clear flags to solid color or don't clear.   

recording sound
--

use the following script to record soundfiles from within unity. note: 16bit audio only.

* attach this script to main camera
* run and press 'r' to start/stop sound recording
* the file will end up in your scene folder
* code adapted from <https://forum.unity3d.com/threads/writing-audiolistener-getoutputdata-to-wav-problem.119295/>

```javascript
#pragma strict

import System.IO;
import System;

public var filename : String = "recTest.wav";
private var headerSize : int = 44;    //default for uncompressed wav
private var fileStream : FileStream;
private var listener : AudioListener;
private var isRecording : boolean = false;
private var written : boolean = false;

function Start() {
    listener= gameObject.GetComponent(AudioListener);
}
function Update() {
    if(Input.GetKeyDown("r")) {    //use key R to start/stop recording
        if(!isRecording) {
            Debug.Log("starting to record");
            StartWriting(filename);
            isRecording= true;
        } else {
            isRecording= false;
            WriteHeader();
            Debug.Log("done recording");
            written= true;
        }
    }
}
function OnApplicationQuit() {
    if(!written) {
        WriteHeader();
    }
}
function StartWriting(name : String) {
    fileStream = new FileStream(name, FileMode.Create);
    var emptyByte : byte = new byte();
    for(var i : int = 0; i<headerSize; i++) {
        fileStream.WriteByte(emptyByte);
    }
}
function OnAudioFilterRead(data : float[], channels : int) {
    if(isRecording) {
        ConvertAndWrite(data);    //audio data is interlaced
    }
}
function ConvertAndWrite(dataSource : float[]) {
    var intData : Int16[] = new Int16[dataSource.length];
    var bytesData : Byte[] = new Byte[dataSource.length*2];
    var rescaleFactor : int = 32767;
    for (var i : int = 0; i<dataSource.length;i++){
        intData[i] = dataSource[i]*rescaleFactor;
        var byteArr : Byte[] = new Byte[2];
        byteArr = BitConverter.GetBytes(intData[i]);
        byteArr.CopyTo(bytesData, i*2);
    }
    fileStream.Write(bytesData, 0, bytesData.length);
}
function WriteHeader() {
    fileStream.Seek(0, SeekOrigin.Begin);
    var riff : Byte[] = System.Text.Encoding.UTF8.GetBytes("RIFF");
    fileStream.Write(riff, 0, 4);
    var chunkSize : Byte[] = BitConverter.GetBytes(fileStream.Length-8);
    fileStream.Write(chunkSize, 0, 4);
    var wave : Byte[] = System.Text.Encoding.UTF8.GetBytes("WAVE");
    fileStream.Write(wave, 0, 4);
    var fmt : Byte[] = System.Text.Encoding.UTF8.GetBytes("fmt ");
    fileStream.Write(fmt, 0, 4);
    var subChunk1 : Byte[] = BitConverter.GetBytes(16);
    fileStream.Write(subChunk1, 0, 4);
    var two : UInt16 = 2;
    var one : UInt16 = 1;
    var audioFormat : Byte[] = BitConverter.GetBytes(one);
    fileStream.Write(audioFormat, 0, 2);
    var numChannels : Byte[] = BitConverter.GetBytes(two);
    fileStream.Write(numChannels, 0, 2);
    var sampleRate : Byte[] = BitConverter.GetBytes(AudioSettings.outputSampleRate);
    fileStream.Write(sampleRate, 0, 4);
    var byteRate : Byte[] = BitConverter.GetBytes(AudioSettings.outputSampleRate*4);
    fileStream.Write(byteRate, 0, 4);
    var four : UInt16 = 4;
    var blockAlign : Byte[] = BitConverter.GetBytes(four);
    fileStream.Write(blockAlign, 0, 2);
    var sixteen : UInt16 = 16;
    var bitsPerSample : Byte[] = BitConverter.GetBytes(sixteen);
    fileStream.Write(bitsPerSample, 0, 2);
    var dataString : Byte[] = System.Text.Encoding.UTF8.GetBytes("data");
    fileStream.Write(dataString, 0, 4);
    var subChunk2 : Byte[] = BitConverter.GetBytes(fileStream.Length-headerSize);
    fileStream.Write(subChunk2, 0, 4);
    fileStream.Close();
}
```

syphon
--

these instructions are ***mac osx*** only. for windows there is a similar texture sending application here... <http://spout.zeal.co>

below demonstrates how to set up Funnel to make your unity scene send graphics to a [syhpon](http://syphon.v002.info) client (i.e. to another application on your own mac or on another computer).

note: here we create a new scene but you can also use an old project.

* start unity and create a new 3D project
* select GameObject / Particle System
* download Funnel by clicking 'the plugin package' on [this](https://github.com/keijiro/Funnel/tree/gllegacy#setting-up) page
* doubleclick the file 'Funnel.unitypackage' to import it into unity
* in unity find the Funnel C# script and drag&drop it onto the main camera
* when you now run your screen should go black and you should see the text 'No cameras rendering'
* select 'Edit / Project Settings / Player' and tick 'Run In Background'
* optionally: in Main Camera inspector set screen width&height to 1920x1080 and Render Mode to 'Preview On Game View' 

![syphon](01syphon.png?raw=true "syphon")

* go to <https://github.com/Syphon/Simple/releases/tag/version-3> and download Syphon Simple Apps 3
* start 'Simple Client.app' and connect to your unity syphon server

![syphon2](02syphon2.png?raw=true "syphon2")

there are clients for processing, max, isadora and many more. see <http://syphon.v002.info>

you can also use it to record your scenes to disk. get [Syphon Recorder](http://syphon.v002.info/recorder/). don't forget to set the preferences (experiment with type of compression, framerate, dimensions etc) 

and to send over network try <http://techlife.sg/TCPSyphon/>

notes on optimisation: try to keep the resolution down, don't send more pixels than your video projector is eventually using, turn off preview if you can and use 'Send Only' mode, connect with ethernet cables instead of wifi if you're sharing textures between computers.

in this repository folder you will also find 'syphonmixer.maxpat' as well as openframeworks code (in folder 'src') that show how to build a simple 3 syphon server mixer.
note that the maxpatch depends on the Syhpon package (install it from package manager), and the openframeworks (needs ofxSyphon and ofxGui) and when the application is running press 'i' to set server and do the mixing (a good idea would be to add osc control to the code).

movie mask
--

* start unity and create a new 3D project
* select GameObject / 3D Object / Plane
* set the rotation to x: -90, y: 0, z: 0
* and set scale to x: -0.4, y: 1, z: -0.3 (this is for a 4:3 ratio movie, use x: -0.48, z: -0.27 for 16:9 etc)
* click 'Add Component' in the inspector and then 'New Script'
* give the script a name (here 'moviemask') and make sure language is JavaScript
* doubleclick the script symbol and paste in the code here below

```javascript
#pragma strict

function Start() {
    var player= gameObject.AddComponent.<UnityEngine.Video.VideoPlayer>();
    player.url= "/Users/asdf/Desktop/bbb.mp4";  //edit path to your movie file
    player.isLooping= true;
    //GetComponent.<Renderer>().material.shader= Shader.Find("MaskedTexture");
    //GetComponent.<Renderer>().material.SetTexture("_Mask", Resources.Load("mymask", Texture));
}
```

when you click run you should see your movie file on the plane object. if not double check the url in the code.

* select Assets / Create / Shader / Unlit Shader
* call the shader 'MaskedTexture' and double click its icon
* delete what is there and instead paste in the following shader code...

```
//from http://wiki.unity3d.com/index.php?title=Texture_Mask
Shader "MaskedTexture" {
    Properties {
        _MainTex ("Base (RGB)", 2D) = "white" {}
        _Mask ("Culling Mask", 2D) = "white" {}
        _Cutoff ("Alpha cutoff", Range (0,1)) = 0.1
    }
    SubShader {
        Tags {"Queue"="Transparent"}
        Lighting Off
        ZWrite Off
        Blend SrcAlpha OneMinusSrcAlpha
        AlphaTest GEqual [_Cutoff]
        Pass {
            SetTexture [_Mask] {combine texture}
            SetTexture [_MainTex] {combine texture, previous}
        }
    }
}
```

* uncomment the two `GetComponent` lines in the javascript above and save
* download this png file <https://github.com/redFrik/udk17-Digital_Harmony/raw/master/udk170706/mymask.png?raw=true> (note: it might look all white, but it's there. on mac alt+click the link to download)
* in unity create a folder in Assets called Resources
* drag&drop the mymask.png file into Resources

now when you run you should see the video as a rounded shape.

find 'Default-Material' in Plane's inspector and change the alpha cutoff slider. create your own masks in some image editing software (greyscale with black as transparent alpha channel) and load them. play around more with the Plane position and scaling, try with solid color instead of skybox.

supercollider
--

busses (here 50 and 52) can be used together with `In` and `Out` to route sound through effects. 

```supercollider
s.boot;

s.boot;

(
SynthDef(\efxVerb, {|in= 50, out= 0|
    var src= In.ar(in, 2);  //read sound from channels/busses 50+51
    var efx= FreeVerb.ar(src, 1, 0.9);
    Out.ar(out, efx);  //send sound to channels/busses 0+1
}).add;

SynthDef(\efxDist, {|in= 52, out= 0|
    var src= In.ar(in, 2);  //read sound from channels/busses 50+51
    var efx= (src*70).tanh*0.3;
    Out.ar(out, efx);  //send sound to channels/busses 0+1
}).add;
)

//use headphones!
a= Synth(\efxVerb);
b= Synth(\efxDist);
{SoundIn.ar.dup*MouseX.kr(0, 0.5)}.play(outbus: 50); //right side send to reverb
{SoundIn.ar.dup*MouseY.kr(0, 0.5)}.play(outbus: 52);  //up send to dist
//stop with cmd+.

//here we play a simple sound out to different busses - dry, dist, verb, dry, dist, verb ... etc 
a= Synth(\efxVerb);
b= Synth(\efxDist);
Pbind(\dur, 0.25, \degree, Pseq([0, 1, 2, 3, 4, 5], inf), \legato, 0.1, \out, Pseq([0, 52, 50], inf)).play
//stop with cmd+.
```

advanced example demonstrating busses, effects, patterns.

```supercollider
(
SynthDef(\verb, {|in, out, mix= 0.3, room= 0.5, damp= 0.5|
    Out.ar(out, FreeVerb.ar(In.ar(in, 2), mix, room, damp));
}).add;
SynthDef(\echo, {|in, out, del= 0.667, dec= 4|
    Out.ar(out, AllpassN.ar(In.ar(in, 2), 4, del.clip(0, 4), dec));
}).add;
SynthDef(\ping, {|out, atk= 0.001, rel= 0.1, amp= 0.3, freq= 400, mod= 8, pan= 0|
    var env= EnvGen.ar(Env.perc(atk, rel, amp), doneAction:2);
    var snd= SinOscFB.ar(freq, (env*mod).clip(0, 1), env);
    Out.ar(out, Pan2.ar(snd, pan));
}).add;
)
~verbBus= Bus.audio(s, 2);
~echoBus= Bus.audio(s, 2);
~verbSyn= Synth(\verb, [\in, ~verbBus, \out, 0]);
~echoSyn= Synth(\echo, [\in, ~echoBus, \out, 0]);
Pdef(\ping1, Pbind(\instrument, \ping, \out, Pseq([0, ~verbBus], inf), \freq, Pseq([400, 200, 100, 300], inf), \mod, Pseq([0, 4, 8, 16, 32], inf), \dur, Pstutter(8, Pseq([1, 0.5, 0.25], inf)))).play
Pdef(\ping2, Pbind(\instrument, \ping, \out, Pseq([0, ~verbBus, ~echoBus], inf), \freq, Pseq([500, 600], inf), \mod, Pseq([0, 4, 8, 16, 32], inf), \dur, Pstutter(6, Pseq([1, 0.5, 0.25], inf)))).play
~verbSyn.set(\mix, 0.8, \room, 0.8, \damp, 0.15)
~verbSyn.set(\mix, 0.8, \room, 0.1, \damp, 0.5)
~verbSyn.set(\mix, 0.99, \room, 0.01, \damp, 0.1)
~verbSyn.set(\mix, 0.1, \room, 0.9, \damp, 0.9)
~echoSyn.set(\dec, 8, \del, 0.125)
~echoSyn.set(\dec, 6, \del, 0.25)
~echoSyn.set(\dec, 2, \del, 0.125)
~echoSyn.set(\dec, 15, \del, 4/3)
Pdef(\ping3, Pbind(\instrument, \ping, \out, Pseq([0, ~verbBus, ~echoBus, 0], inf), \rel, Pseq([0.1, 0.2, 0.3], inf), \freq, Pseq([700, 800, 900], inf), \mod, Pseq([0, 4, 8, 16, 32], inf), \dur, Pstutter(4, Pseq([1, 0.5, 0.25], inf)), \amp, 0.05)).play
Pdef(\ping4, Pbind(\instrument, \ping, \out, Pseq([0, ~verbBus, ~echoBus, 0, 0], inf), \rel, Pseq([0.7, 0.1], inf), \freq, Pseq([1000, 1100, 1400], inf), \mod, Pseq([0, 4, 8, 16, 32], inf), \dur, Pstutter(2, Pseq([1, 0.5, 0.25], inf)), \amp, 0.03)).play
Pdef(\ping5, Pbind(\instrument, \ping, \out, Pseq([0, ~verbBus, ~echoBus, 0, 0, 0], inf), \rel, Pseq([0.4, 0.01], inf), \freq, Pseq([1500, 1500, 1800], inf), \mod, Pseq([0, 4, 8, 16, 32], inf), \dur, Pstutter(1, Pseq([1, 0.5, 0.25], inf)), \amp, 0.01, \pan, Pseq([-0.75, 0, 0.75], inf))).play
Pdef(\ping0, Pbind(\instrument, \ping, \out, Pseq([~echoBus], inf), \rel, 0.01, \freq, Pseq([50, 100], inf), \dur, Pseq([1, 1, 1, 0.5, 0.5], inf), \amp, 0.6)).play

//change the synthdef while the above is running
(
SynthDef(\ping, {|out, atk= 0.001, rel= 0.1, amp= 0.3, freq= 400, mod= 8, pan= 0|
    var env= EnvGen.ar(Env.perc(atk, rel, amp, -2), doneAction:2);
    var snd= Mix(SinOsc.ar(freq*([0.5, 0.75, 1, 1.25]+env), env*mod*2pi, env)).tanh;
    Out.ar(out, Pan2.ar(snd, pan));
}).add;
)

(
SynthDef(\ping, {|out, atk= 0.001, rel= 0.1, amp= 0.3, freq= 400, mod= 8, pan= 0|
    var env= EnvGen.ar(Env.perc(atk, rel, amp, -2), doneAction:2);
    var snd= Mix(VarSaw.ar(freq*([0.5, 0.75, 1, 1.25]+env), 0, 1-(env*mod).clip(0, 1), env));
    Out.ar(out, Pan2.ar(snd, pan));
}).add;
)

(
SynthDef(\ping, {|out, atk= 0.001, rel= 0.1, amp= 0.3, freq= 400, mod= 8, pan= 0|
    var env= EnvGen.ar(Env.perc(atk, rel, amp, -2), doneAction:2);
    var snd= Mix(SinOsc.ar(0, SinOsc.ar(freq, 0, mod*pi)*pi, env.lag(0.01))+VarSaw.ar(freq*([0.5, 0.75, 1, 1.25]+env), 0, 1-(env*mod).clip(0, 1), env));
    Out.ar(out, Pan2.ar(snd, pan));
}).add;
)

//to change one of the effects we need to free (release) the synth and start it again
(
~echoSyn.release;
SynthDef(\echo, {|in, out, del= 0.667, dec= 4, sweep= 0.5|
    Out.ar(out, AllpassC.ar(In.ar(in, 2), 4, (del*LFTri.ar(sweep, 0, 1, 2)).clip(0, 4), dec));
}).add;
)
//note the addToTail - this is needed for the In.ar to work
~echoSyn= Synth(\echo, [\in, ~echoBus, \out, 0], addAction:'addToTail');
~echoSyn.set(\sweep, 1);
~echoSyn.set(\sweep, 0.05);
~echoSyn.set(\sweep, 0.5);

//to see the order of currently running synts
s.plotTree;

//read the 'Order of execution' helpfile
```

extra
--

terrain heightmaps from real world data <https://www.youtube.com/watch?v=-vyNbalvXR4&t=4s>

advanced tutorials <http://catlikecoding.com/unity/tutorials/>
