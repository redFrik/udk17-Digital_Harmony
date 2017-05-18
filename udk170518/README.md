more shaders + more osc (leap)
--------------------

repetition and continuation of shaders and open sound control from last week

osc
--

to set up unity to listen to incoming osc messages do the following...

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
public var left : float = 0.0;
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

now switch over to supercollider and try sending some simple messages

```supercollider
//run each line separately
NetAddr("127.0.0.1", 8400).sendMsg(\fromSC, 0.1, 0.2);
NetAddr("127.0.0.1", 8400).sendMsg(\fromSC, -3.3, 2.2);
NetAddr("127.0.0.1", 8400).sendMsg(\fromSC, 0.004, 0.005);
```

you should see the left and right values in the inspector update and the camera should move.

in unity go into 'Edit / Project Settings / Player' and tick 'Run In Background' if you want to keep supercollider as your front application.

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

pitch tracking
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

leap
--

here are instructions on how to use the [leap motion controller](http://leapmotion.com) with unity.

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

links
--

<https://docs.unity3d.com/Manual/SL-VertexFragmentShaderExamples.html>

<http://www.alanzucconi.com/2015/06/10/a-gentle-introduction-to-shaders-in-unity3d/>

<https://www.youtube.com/channel/UCEklP9iLcpExB8vp_fWQseg/videos>
