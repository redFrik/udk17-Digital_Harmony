characters + more osc
--------------------

this time we will try out characters (players) in unity and also look at sending out controller osc data. (so far we have only received.)

mixing osc
--

in this example we take in osc data from supercollider and *add* it to the position of a character (first person shooter prefab). this to demonstrate how we can both control the character using the standard keyboard and mouse controls, and at the same time mix in positional data from an oscillator via osc.

* start unity and create a new 3d project
* select GameObject / 3D Object / Terrain
* select Assets / Import Package / Characters
* just click 'import' in the window that pops up to import everything
* go to 'Project' tab and then expand Assets
* find Standard Assets / Characters / FirstPersonCharacter / Prefabs / FPSController
* drag&drop FPSController onto the terrain
* optionally delete or disable the Main Camera
* press the play button (mouse = look around, space = jump, ADWS or arrow keys to move)

you should be able to run and look around. press 'esc' key and then click the stop button.

* select 'Terrain' in the Hierarchy window
* click on the paint brush icon in the inspector
* click on the 'Edit Textures...' button and add a texture
* either pick one (here RollerBallAlbedo) or drag a picture from your harddrive

![fpsosc](00fpsosc.png?raw=true "fpsosc")

now we will add a script that receives osc and add x and y values to the player position.

* go to <https://github.com/heaversm/unity-osc-receiver> and click the green download button
* get the .zip file and uncompress it
* find the folder Plugins in the zip you just uncompressed (unity-osc-receiver-master / Assets)
* drag&drop 'Plugins' into unity's assets window (bottom)
* select 'GameObject / Create Empty'
* in the inspector select 'Add Component / Scripts / Osc'
* and again select 'Add Component / Scripts / UDP Packet IO'
* in the inspector select 'Add Component / New Script'
* call it something (here 'fpsreceiver'), make sure language is **javascript** and click 'Create and Add'
* double click the script to open it in MonoDevelop
* paste in the following code replacing what was there...

```javascript
#pragma strict

public var RemoteIP : String = "127.0.0.1";
public var SendToPort : int = 57120;
public var ListenerPort : int = 8400;
private var osc : Osc;
public var x : float = 0.0;  //note: we need these global variables because unity will not allow setting gameobject properties from inside the osc handler function
public var z : float = 0.0;

function Start() {
    var udp : UDPPacketIO = GetComponent("UDPPacketIO");
    udp.init(RemoteIP, SendToPort, ListenerPort);
    osc = GetComponent("Osc");
    osc.init(udp);
    osc.SetAllMessageHandler(AllMessageHandler);
}

function Update() {
    GameObject.Find("FPSController").transform.localPosition.x += x;
    GameObject.Find("FPSController").transform.localPosition.z += z;
    if(GameObject.Find("FPSController").transform.localPosition.y<0.98) {  //make sure not to fall off the cliff
        GameObject.Find("FPSController").transform.localPosition.y= 0.98;
    }
}

public function AllMessageHandler(oscMessage: OscMessage) {
    var msgAddress = oscMessage.Address;
    if(msgAddress == "/fromSC") {
        x= oscMessage.Values[0];
        z= oscMessage.Values[1];
    }
}
```

run and then in supercollider try these three examples of sending osc

```supercollider
s.boot;

//mic input to 'shake' the player
(
var unity= NetAddr("127.0.0.1", 8400);  //address of unity program
OSCdef(\trk, {|msg| unity.sendMsg(\fromSC, msg[3], msg[4])}, \data); //get osc from sendreply & send it to unity
//0.1 is smooth filter, 3 is amount
Ndef(\tracker, {SendReply.kr(Impulse.kr(60), '/data', SoundIn.ar([0, 1]).lag(0.1)*5); DC.ar(0)});
)


//lfo example - circles
(
var unity= NetAddr("127.0.0.1", 8400);
OSCdef(\trk, {|msg| unity.sendMsg(\fromSC, msg[3], msg[4])}, \data);
//0.15 and 0.2 are speed, 0.1 is circle radius
Ndef(\tracker, {SendReply.kr(Impulse.kr(60), '/data', SinOsc.ar([0.15, 0.2], 0, 0.1)); DC.ar(0)});
)


//lfo example - random jumps on sound trigger
(
var unity= NetAddr("127.0.0.1", 8400);
OSCdef(\trk, {|msg| unity.sendMsg(\fromSC, msg[3], msg[4])}, \data);
//0.5 is trigger sound level, -3 and 3 are maxrange, 2 is stiffness
Ndef(\tracker, {
    var trig= Amplitude.kr(SoundIn.ar([0, 1]))>0.5;
    SendReply.kr(Impulse.kr(60), '/data', HPF.kr(TRand.kr(-3, 3, trig), 2));
    DC.ar(0);
});
)
```

so with the last sc example you should be able to run around freely in unity, but when there is a loud sound detected your character will bounce in some random direction.

try to write your own sound code. perhaps send a trigger osc message when a certain note in a melody is played, or use the position of the soundfile.
also try spinning (rotating) the character instead of adding to its position. or scaling the characters size.

remember that you can in unity go into 'Edit / Project Settings / Player' and tick 'Run In Background' if you want to keep supercollider as your front application.

send osc
--

in this example we send a lot of data from unity to supercollider.

* create a new 3d project
* select GameObject / 3D Object / Plain
* select Assets / Import Package / Characters
* just click 'import' in the window that pops up to import everything
* go to 'Project' tab and then expand Assets
* find Standard Assets / Characters / ThirdPersonCharacter / Prefabs / ThirdPersonController
* drag&drop ThirdPersonController onto the Hierarchy window
* press the play button (ADWS or arrow keys to run around, + shift to walk, space to jump)

now we need that osc plugin again...

* go to <https://github.com/heaversm/unity-osc-receiver> and click the green download button
* get the .zip file and uncompress it
* find the folder Plugins in the zip you just uncompressed (unity-osc-receiver-master / Assets)
* drag&drop 'Plugins' into unity's assets window (bottom)
* select 'GameObject / Create Empty'
* in the inspector select 'Add Component / Scripts / Osc'
* and again select 'Add Component / Scripts / UDP Packet IO'
* in the inspector select 'Add Component / New Script'
* call it something (here 'sender'), make sure language is **javascript** and click 'Create and Add'
* double click the script to open it in MonoDevelop
* paste in the following code replacing what was there...

```javascript
#pragma strict

public var RemoteIP : String = "127.0.0.1";
public var SendToPort : int = 57120;
public var ListenerPort : int = 8400;
private var osc : Osc;

function Start () {
    var udp : UDPPacketIO = GetComponent("UDPPacketIO");
    udp.init(RemoteIP, SendToPort, ListenerPort);
    osc = GetComponent("Osc");
    osc.init(udp);
}

function Update () {
    var obj= GameObject.Find("ThirdPersonController");
    var msg : OscMessage;
    msg= Osc.StringToOscMessage("/tpc");
    msg.Values.Add(obj.transform.localPosition.x);
    msg.Values.Add(obj.transform.localPosition.y);
    msg.Values.Add(obj.transform.localPosition.z);
    msg.Values.Add(obj.transform.localRotation.x);
    msg.Values.Add(obj.transform.localRotation.y);
    msg.Values.Add(obj.transform.localRotation.z);
    osc.Send(msg);
    obj= obj.Find("EthanHips");
    msg= Osc.StringToOscMessage("/tpc/hips");
    msg.Values.Add(obj.transform.localPosition.x);
    msg.Values.Add(obj.transform.localPosition.y);
    msg.Values.Add(obj.transform.localPosition.z);
    msg.Values.Add(obj.transform.localRotation.x);
    msg.Values.Add(obj.transform.localRotation.y);
    msg.Values.Add(obj.transform.localRotation.z);
    osc.Send(msg);
    obj= obj.Find("EthanLeftUpLeg");
    msg= Osc.StringToOscMessage("/tpc/leftupleg");
    msg.Values.Add(obj.transform.localPosition.x);
    msg.Values.Add(obj.transform.localPosition.y);
    msg.Values.Add(obj.transform.localPosition.z);
    msg.Values.Add(obj.transform.localRotation.x);
    msg.Values.Add(obj.transform.localRotation.y);
    msg.Values.Add(obj.transform.localRotation.z);
    osc.Send(msg);
    obj= obj.Find("EthanLeftLeg");
    msg= Osc.StringToOscMessage("/tpc/leftleg");
    msg.Values.Add(obj.transform.localPosition.x);
    msg.Values.Add(obj.transform.localPosition.y);
    msg.Values.Add(obj.transform.localPosition.z);
    msg.Values.Add(obj.transform.localRotation.x);
    msg.Values.Add(obj.transform.localRotation.y);
    msg.Values.Add(obj.transform.localRotation.z);
    osc.Send(msg);
    obj= obj.Find("EthanLeftFoot");
    msg= Osc.StringToOscMessage("/tpc/leftfoot");
    msg.Values.Add(obj.transform.localPosition.x);
    msg.Values.Add(obj.transform.localPosition.y);
    msg.Values.Add(obj.transform.localPosition.z);
    msg.Values.Add(obj.transform.localRotation.x);
    msg.Values.Add(obj.transform.localRotation.y);
    msg.Values.Add(obj.transform.localRotation.z);
    osc.Send(msg);
    obj= obj.Find("EthanLeftToe1");
    msg= Osc.StringToOscMessage("/tpc/lefttoe1");
    msg.Values.Add(obj.transform.localPosition.x);
    msg.Values.Add(obj.transform.localPosition.y);
    msg.Values.Add(obj.transform.localPosition.z);
    msg.Values.Add(obj.transform.localRotation.x);
    msg.Values.Add(obj.transform.localRotation.y);
    msg.Values.Add(obj.transform.localRotation.z);
    osc.Send(msg);
    //etc
    //also note that one can simplify the above with a function
}

function OnDisable() {
    osc.Cancel();
    osc = null;
}
```

play and switch over to supercollider. the following should post a lot of numbers in the post window.

```supercollider
(
OSCFunc({|msg| msg.postln}, '/tpc');
OSCFunc({|msg| msg.postln}, '/tpc/hips');
OSCFunc({|msg| msg.postln}, '/tpc/leftupleg');
OSCFunc({|msg| msg.postln}, '/tpc/leftleg');
OSCFunc({|msg| msg.postln}, '/tpc/leftfoot');
OSCFunc({|msg| msg.postln}, '/tpc/lefttoe1');
)
```

and here is a more advanced example that build up a Ndef (sound) and an OSCFunc (receiver) for each address in a list.  position and rotation of some of the joins are mapped to frequency and amplitude respectively.

```supercollider
s.boot;

(
['/tpc', '/tpc/hips', '/tpc/leftupleg', '/tpc/leftleg', '/tpc/leftfoot', '/tpc/lefttoe1'].do{|x|
    Ndef(x, {
        Splay.ar(SinOsc.ar(\pos.kr([0, 0, 0]).linexp(-1, 1, 300, 3000), 0, HPF.kr(\rot.kr([0, 0, 0]),1)));
    }).play;
    OSCFunc({|msg|
        Ndef(x).set(\pos, msg[1..3], \rot, msg[4..6]);
    }, x);
};
)
```

extra
--

similar examples from last semester <https://github.com/redFrik/udk16-Immersive_Technologies/tree/master/udk161208>

sending the position and rotation of a htc vive headset and controllers <https://github.com/redFrik/udk16-Immersive_Technologies/tree/master/udk170105#virtual-reality>
