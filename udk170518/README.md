more shaders + more osc (leap)
--------------------

repetition and continuation of shaders and open sound control from last week

osc
--

to set up unity to listen to incoming osc messages do the following (as we did so many times)...

* create a new 3d unity project
* select 'GameObject / 3D Object / Cube' to add a simple cube
* go to <https://github.com/heaversm/unity-osc-receiver> and click the green download button
* get the .zip file and uncompress it
* find the folder Plugins in the zip you just uncompressed (unity-osc-receiver-master / Assets)
* drag&drop 'Plugins' into unity's assets window (bottom)
* select 'GameObject / Create Empty'
* in the inspector select 'Add Component / Scripts / Osc'
* and again select 'Add Component / Scripts / UDP Packet IO'
* in the inspector select 'Add Component / New Script'
* call it something (here 'receiver'), make sure language is **javascript** and click 'Create and Add'
* double click the script to open it in MonoDevelop
* paste in the following code replacing what was there...

```javascript
#pragma strict

public var RemoteIP : String = "127.0.0.1";
public var SendToPort : int = 57120;
public var ListenerPort : int = 8400;
private var osc : Osc;
public var left : float = 0.0;  //note: we need these global variables because unity will not allow setting gameobject properties from inside the osc handler function
public var right : float = 0.0;

function Start() {
    var udp : UDPPacketIO = GetComponent("UDPPacketIO");
    udp.init(RemoteIP, SendToPort, ListenerPort);
    osc = GetComponent("Osc");
    osc.init(udp);
    osc.SetAllMessageHandler(AllMessageHandler);
}

function Update() {
    //here we can find game objects and send the left/right data to them
    //experiment and try different objects, use multiply and add to change the ranges
    
    GameObject.Find("Main Camera").transform.localPosition.x= left;
    GameObject.Find("Main Camera").transform.localPosition.y= right;
    
    //GameObject.Find("Main Camera").transform.localRotation.x= left;  //should use * Mathf.PI
    //GameObject.Find("Main Camera").transform.localRotation.z= right;
    
    //GameObject.Find("Main Camera").GetComponent.<Camera>().fieldOfView= (1-Mathf.Max(left, right))*150;
    
    //GameObject.Find("Cube").transform.localPosition.x= left;
    //GameObject.Find("Cube").transform.localPosition.y= right;
}

public function AllMessageHandler(oscMessage: OscMessage) {
    var msgAddress = oscMessage.Address;
    if(msgAddress == "/fromSC") {
        left= oscMessage.Values[0];
        right= oscMessage.Values[1];
    }
}
```

your scene should look like this...

![osc](01osc.png?raw=true "osc")

now run and switch over to supercollider. try sending some simple messages...

```supercollider
//run each line separately
NetAddr("127.0.0.1", 8400).sendMsg(\fromSC, 0.1, 0.2);
NetAddr("127.0.0.1", 8400).sendMsg(\fromSC, -3.3, 2.2);
NetAddr("127.0.0.1", 8400).sendMsg(\fromSC, 0.004, 0.005);
```

you should see the left and right values in the inspector update and the camera should move.

in unity go into 'Edit / Project Settings / Player' and tick 'Run In Background' if you want to keep supercollider as your front application.

try this little gui widget. it will also send osc - just like the lines of code above.

```supercollider
Slider2D().front.action= {|view| NetAddr("127.0.0.1", 8400).sendMsg(\fromSC, view.x, view.y)};
```

osc amplitude tracker
--

here we use the amplitude (volume) of the sound in left and right channel and send them over to unity. you can play any sound and the ```Amplitude.kr``` will track its amplitude. 60 times per second send the ```SendReply.kr``` will read the output of ```Amplitude.kr``` and send it to the ```OSCdef``` - that will in turn send it over to unity.

```supercollider
s.boot;

//amplitude tracker
//takes the sound from channels 0 and 1, read the amplitudes and send via osc to unity (or some other program)
//the sent message will be: \fromSC, leftAmp, rightAmp
(
var unity= NetAddr("127.0.0.1", 8400);  //address of unity program
OSCdef(\trk, {|msg| unity.sendMsg(\fromSC, msg[3], msg[4])}, \amp); //get osc from sendreply & send it to unity
Ndef(\tracker, {SendReply.kr(Impulse.kr(60), '/amp', Amplitude.kr(InFeedback.ar(0, 2), 0.01, 0.1))});  //0.01 and 0.1 are attack and release - try changing them
)

//some sound to test with
Ndef(\snd, {SinOsc.ar([200, 404], 0, SinOsc.ar([0.3, 0.2]).max(0))}).play;
Ndef(\snd).stop;
Ndef(\snd, {DelayN.ar(SoundIn.ar, 1, [0.9, 0.8])}).play;
Ndef(\snd).stop;
Ndef(\snd, {DelayC.ar(SoundIn.ar, 1, LFTri.ar([1, 1.1])/3+0.5)}).play;
Ndef(\snd).stop;
Ndef(\snd, {AllpassC.ar(Saw.ar(MouseX.kr(50, 500, 1, [1, 2]).round(25)), 1, LFTri.ar([1, 2])/3+0.5, 3)}).play;
Ndef(\snd).stop;

//etc - try with your own
```

now go back to unity and add more objects, try make them move with the amplitude of the sound.

osc lfo
--

we can also use the direct sound. but because our screen framerate is 60fps, it will only make sense to send the direct sample values of low frequency oscillators (lfo).

so let us just change the \tracker line a little bit and remove the ```Amplitude.kr```. this will now send the samples (usually in the range -1.0 to 1.0 while the amplitude was usually 0.0 to 1.0). also we will read from a node definition instead of from channels 0 and 1 although you could still use ```InFeedback.ar``` if you wanted to.

```supercollider
(
var unity= NetAddr("127.0.0.1", 8400);  //address of unity program
OSCdef(\trk, {|msg| unity.sendMsg(\fromSC, msg[3], msg[4])}, \amp); //get osc from sendreply & send it to unity
Ndef(\tracker, {SendReply.kr(Impulse.kr(60), '/amp', Ndef(\lfo).ar)});  //send direct sound samples
)

//some lfos to test with
Ndef(\lfo, {SinOsc.ar([0.2, 0.3])});
Ndef(\lfo, {LFSaw.ar([0.5, 0.6])});
Ndef(\lfo, {LFNoise2.ar([5, 6])});
Ndef(\lfo, {LFPulse.ar([5, 6]).lag(MouseX.kr(0, 1))});
Ndef(\lfo, {LFSaw.ar([1, 2.01]).round(MouseX.kr(0, 1))});

//some sound to test with that also uses the lfo
Ndef(\snd, {SinOsc.ar([200, 404], 0, Ndef(\lfo).ar)}).play;
Ndef(\snd).stop;
Ndef(\snd, {LPF.ar(Impulse.ar(4**Ndef.ar(\lfo)+1*40), 1000)}).play;
Ndef(\snd).stop;
Ndef(\snd, {CombC.ar(Saw.ar(Ndef.ar(\lfo).exprange(50, 500).round(25)), 0.1, 0.1, 3)/2}).play;
Ndef(\snd).stop;
```

osc pitch tracking
--

another possibility is to track the pitch (frequency) of the sound. this is a bit more advanced as the pitch tracker isn't perfect and you will get errors. also the values reported are in Hertz (typically 60-4000) and you will want to scale that to a range suitable for unity (here -1 to 1).

```supercollider
(
var unity= NetAddr("127.0.0.1", 8400);  //address of unity program
OSCdef(\trk, {|msg| unity.sendMsg(\fromSC, msg[3], msg[4])}, \amp); //get osc from sendreply & send it to unity
Ndef(\tracker, {SendReply.kr(Impulse.kr(60), '/amp', Pitch.kr(InFeedback.ar(0, 2)).flop[0].explin(60, 4000, -1, 1))});  //send pitch tracker data
)

Ndef(\snd, {SinOsc.ar([MouseX.kr(100, 1000, 1), MouseY.kr(100, 1000, 1)], 0, 0.2)}).play;
Ndef(\snd).stop;
Ndef(\snd, {DelayN.ar(SoundIn.ar([0, 1]), 1, 1)}).play;  //try to whistle
Ndef(\snd).stop;
Ndef(\snd, {LFTri.ar(Duty.kr([0.3, 0.13], 0, Dseq([100, 500, 400, 1000, 200, 240], inf)))/2}).play;
Ndef(\snd).stop;
```

pitch tracking works best with monophonic and simple sounds. noisy or rich sounds or chords will mess up the analysis.

there are much more data you can extract from sound. for example see [SCMIR](https://composerprogrammer.com/code.html) and other plugins by Nick Collins.

shader control
--

switch back to unity and let us now control a post processing (fullscreen) shader using osc. so like last week...

* select the Main Camera and click 'Add Component' in the inspector
* select 'New Script', make sure language is set to Javascript and give it a name (here 'postscript')
* doubleclick the script icon to open it in MonoDevelop
* replace what is there with the code below

```javascript
#pragma strict

var mat: Material;

function OnRenderImage(src: RenderTexture, dest: RenderTexture) {
    Graphics.Blit(src, dest, mat);
}
```

* in the Assets menu, select 'Create / Shader / Image Effect Shader' 
* optionally give it a name (here 'postshader')
* doubleclick the shader icon to open it in MonoDevelop
* in the first line change `"Hidden/NewImageEffectShader"` to `"MyShaders/post"`
* save and switch back to unity
* in the Assets menu, select 'Create / Material' 
* optionally give it a name (here 'postmaterial')
* (if needed) in the material's inspector set 'Shader' to 'MyShaders/post'
* select the Main Camera and drag&drop your material to 'Mat' in the inspector

test it by running the scene. you should see all colours inverted.

we can now edit the fragment and vertex programs in the 'postshader' file. see [frag](https://github.com/redFrik/udk17-Digital_Harmony/tree/master/udk170511#frag) and [vert](https://github.com/redFrik/udk17-Digital_Harmony/tree/master/udk170511#vert) sections from last week.

to control shader parameters via osc we need to do some more work.

first replace the code in the receiver.js script with this...

```javascript
#pragma strict

public var RemoteIP : String = "127.0.0.1";
public var SendToPort : int = 57120;
public var ListenerPort : int = 8400;
private var osc : Osc;
public var left : float = 0.0;
public var right : float = 0.0;
var mat: Material;  //material that holds our shader code

function Start() {
    var udp : UDPPacketIO = GetComponent("UDPPacketIO");
    udp.init(RemoteIP, SendToPort, ListenerPort);
    osc = GetComponent("Osc");
    osc.init(udp);
    osc.SetAllMessageHandler(AllMessageHandler);
}

function Update() {
    mat.SetFloat("_Left", left);
    mat.SetFloat("_Right", right);
}

public function AllMessageHandler(oscMessage: OscMessage) {
    var msgAddress = oscMessage.Address;
    if(msgAddress == "/fromSC") {
        left= oscMessage.Values[0];
        right= oscMessage.Values[1];
    }
}
```

(the only new thing is that we added a material variable and use the `SetFloat` methods on the material)

next select the GameObject in unity and set the material variable by dragging&dropping the 'postmaterial' onto 'Mat' in the inspector

![mat](02mat.png?raw=true "mat")

the last thing to do is to change the shader code to accept the incomming floats (here called "_Left" and "_Right"). so we need to define two floats in 'Properties' (also other types possible - see [here](https://docs.unity3d.com/Manual/SL-PropertiesInPrograms.html) ), we also need to declare them as `uniform float` and then we should use them somehow in the code (here as simple offset in this line `fixed4 col = tex2D(_MainTex, i.uv+float2(_Left, _Right));`)

so copy&paste the code below into your 'postshader' file replacing what was there.

```
Shader "MyShaders/post"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _Left ("left value from osc", Float) = 0
        _Right ("right value from osc", Float) = 0
    }
    SubShader
    {
        // No culling or depth
        Cull Off ZWrite Off ZTest Always
        
        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            
            #include "UnityCG.cginc"
            
            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };
            
            struct v2f
            {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };
            
            uniform float _Left;
            uniform float _Right;
            
            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }
            
            sampler2D _MainTex;
            
            fixed4 frag (v2f i) : SV_Target
            {
                fixed4 col = tex2D(_MainTex, i.uv+float2(_Left, _Right));
                return col;
            }
            ENDCG
        }
    }
}
```

the supercollider osc examples should now control the offset x/y positions of the whole scene. experiment with `_Left` and `_Right` in the shader code while some supercollider example is running in the background.

some ideas for the fragment program...

```
fixed4 col = tex2D(_MainTex, i.uv+float2(sin(_Left*0.3), sin(_Right*0.3)));
fixed4 col = tex2D(_MainTex, i.uv)+float4(_Left, _Right, 0, 1);
fixed4 col = (tex2D(_MainTex, i.uv)+_Right)%float4(_Left, _Left, _Left, 1);
fixed4 col = tex2D(_MainTex, float2(sin(i.uv.x*20)*_Left, sin(i.uv.y*10)*_Right));
```

or just overwrite colours and create your own patterns, ignoring what is in the scene...
```
fixed4 col = float4(sin(i.uv.x*20)*_Left, sin(i.uv.y*10)*_Right, 1, 1);
fixed4 col = float4(sin(i.uv.x*_Left*100), sin(i.uv.y*_Right*100), 1, 1);
fixed4 col = float4(_Left%i.uv.x, _Left%i.uv.x, _Left%i.uv.x, 1)+float4(_Right%i.uv.y, _Right%i.uv.y, _Right%i.uv.y, 1);
```

and likewise you can use these osc controlled variables in the vertex program...

```
o.uv = v.uv+float2(_Left*0.3, _Right*0.3);  //same as above
o.uv = v.uv+float2(sin(o.vertex.x*_Left), sin(o.vertex.y*_Right));
```

![pattern](03pattern.png?raw=true "pattern")

maxmsp
--

if you rather want to control unity from some other program you can just ignore supercollider and send osc like in this maxmsp example...

![maxmsp](04maxmsp.png?raw=true "maxmsp")

so the osc message should be the address `/fromSC` together with two float values.

leap
--

for linux see [leap in Linux](#leap-in-linux)

here are instructions on how to use the [leap motion controller](https://leapmotion.com) with unity.

* go to <https://github.com/leapmotion/LeapMotionCoreAssets> and download the library as zip
* unpack the zip file and find the three folders: `LeapMotion`, `LeapMotionVR` and `Plugins` (they are all inside the Assets folder)
* drag and drop these three into a new empty unity 3d project
* find the demo `FlowerAndRobot` under 'LeapMotion / Scenes', doubleclick it and run

the demo should work and you should be able to interact with the flower.

to use the controller inside an empty scene do the following...

* create a new scene and add the `HandController` prefab and `HandControllerSandBox`
* add a `Cube`
* select the Cube and click `Add Compoment` and then select `Physics / Rigidbody`
* select the HandController and set the position to `X= 0`, `Y= -2`, `Z= -2` and scale to `X= 10`, `Y= 10`, `Z= 1`

Leap in Linux
--

on linux you will need the repository <https://github.com/leapmotion/UnityModules>, the dynamic
libraries are only available on the v2 directory.

download the repository, enter the Pendulum example and prepare the project by
copying the right files:

```
$ cd Pendulum
$ mv Assets/Plugins/x86_64/libLeap* .
$ mv Assets/Plugins/LeapCSharp.NET3.5.dll Assets/
```

also you might need to do a workaround because of the unity versions and the lower case:

(see here: <https://issuetracker.unity3d.com/issues/linux-editor-api-updater-fails-to-upgrade-this-dot-component-expressions>)

```
$ cd /opt/Unity/Editor/Data/Mono/lib/mono/2.0/
$ sudo ln -sf System.Xml.dll System.xml.dll
```

then open `unity-editor`, open the Pendulum project, inside the `Project` select `Assets -> LeapMotion -> Scenes`, double click one of the scenes and click play.

links
--

<https://docs.unity3d.com/Manual/SL-VertexFragmentShaderExamples.html>

<https://www.alanzucconi.com/2015/06/10/a-gentle-introduction-to-shaders-in-unity3d/>

<https://www.youtube.com/channel/UCEklP9iLcpExB8vp_fWQseg/videos>
