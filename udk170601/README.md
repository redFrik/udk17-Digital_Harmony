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
    //also note that one can simplify the above with a function and/or arrays
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

spring balls
--

* create a new 3d project
* select GameObject / 3D Object / Sphere
* select Component / Physics / Spring Joint
* in the Hierarchy window duplicate the sphere four times
* go though and set the x position for each sphere to -6, -3, 0, 3 and 6

your scene should now look like this...

![springballs](01springballs.png?raw=true "springballs")

* create an empty game object, add the osc plug and the three scripts like above
* paste in the following javascript code...

```javascript
#pragma strict

public var RemoteIP : String = "127.0.0.1";
public var SendToPort : int = 57120;
public var ListenerPort : int = 8400;
private var osc : Osc;
private var ballIndex : int = -1;  //-1 means no ball selected
private var ballX : float = 0.0;
private var ballY : float = 0.0;
private var ballZ : float = 0.0;

function Start() {
    var udp : UDPPacketIO = GetComponent("UDPPacketIO");
    udp.init(RemoteIP, SendToPort, ListenerPort);
    osc = GetComponent("Osc");
    osc.init(udp);
    osc.SetAllMessageHandler(AllMessageHandler);
}

function Update() {
    var obj0= GameObject.Find("Sphere");  //should match name in Hierarchy window
    var obj1= GameObject.Find("Sphere (1)");
    var obj2= GameObject.Find("Sphere (2)");
    var obj3= GameObject.Find("Sphere (3)");
    var obj4= GameObject.Find("Sphere (4)");
    //note a better way would be to give all objects a tag and then find them here using that
    var objs= [obj0, obj1, obj2, obj3, obj4];

    //--receive position
    if(ballIndex>=0) {
        var obj : GameObject = objs[ballIndex];
        obj.transform.localPosition.x= ballX;
        obj.transform.localPosition.y= ballY;
        obj.transform.localPosition.z= ballZ;
        ballIndex= -1;	//reset flag
    }

    //--send positions
    var msg : OscMessage;
    for(var i= 0; i<objs.length; i++) {
        msg= Osc.StringToOscMessage("/objs");
        msg.Values.Add(i);
        msg.Values.Add(objs[i].transform.localPosition.x);
        msg.Values.Add(objs[i].transform.localPosition.y);
        msg.Values.Add(objs[i].transform.localPosition.z);
        osc.Send(msg);
    }
}

public function AllMessageHandler(oscMessage: OscMessage) {
    var msgAddress = oscMessage.Address;
    if(msgAddress == "/ballIndex") {
        ballIndex= oscMessage.Values[0];
        ballX= oscMessage.Values[1];
        ballY= oscMessage.Values[2];
        ballZ= oscMessage.Values[3];
    }
}
```

now set the scene to run in background (see above) and press play.

move over to supercollider and try sending some trigger messages like this...

```supercollider
NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, 0, -6, 2, 0);
NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, 1, -3, 2, 0);
NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, 2, 0, 2, 0);
NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, 3, 3, 2, 0);
NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, 4, 6, 2, 0);
```

you should see the balls start to dance. try experimenting with the values in the message.  the second value is which ball (0-4) and the last three its x, y, z position.

note that a more realistic method than to brutally set the objects to a position would be to briefly add a strong attractor or force for the object to react upon - a bit like hitting it to fly in some direction.

now with sound...

```supercollider
s.boot;

(
var unity= NetAddr("127.0.0.1", 8400);  //address of unity program
var num= 5;  //number of spheres
var ndefs= {|i|
    Ndef(("snd"++i).asSymbol, {|x, y, z, t_trig= 0|
        var snd= SinOscFB.ar(y.linexp(-2, 1, 100, 1000).lag+(i*100), z.linlin(-1, 1, 0, 1).clip(0, 1), z.linlin(-1, 1, 0, 1));
        var env= EnvGen.ar(Env.perc(0.01, 5), t_trig, 1, 0.1);
        Pan2.ar(snd*env, x/6);
    }).play;
}.dup(5);
//incoming osc should be: /objs, index(0-4), x, y, z
OSCdef(\objs, {|msg|
    ndefs[msg[1]].set(\x, msg[2], \y, msg[3], \z, msg[4]);
}, \objs);
)

//trigger...
NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, 0, 5.0.rand2, 5.0.rand2, 5.0.rand2);
NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, 1, 5.0.rand2, 5.0.rand2, 5.0.rand2);
NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, 2, 5.0.rand2, 5.0.rand2, 5.0.rand2);
NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, 3, 5.0.rand2, 5.0.rand2, 5.0.rand2);
NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, 4, 5.0.rand2, 5.0.rand2, 5.0.rand2);

//run this fast multiple times
NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, 2, 4.0.rand2, [0, -3, -6].choose, 3);

//sequencer
(
Routine.run({
    16.do{|i|
        NetAddr("127.0.0.1", 8400).sendMsg(\ballIndex, i%5, 0, 5, 2);
        0.2.wait;
    };
});
)
```

in unity try chaning the overall gravity under Edit / Project Settings / Physics. also try chaning the sizes for all the spheres (change scale x, y, z for each).
really fun is to mess with the 'Sphere Collider' center position (x, y, z) and radius for each sphere. you will get vibrato effects.

and here's another sound for supercollider...

```supercollider
(
var unity= NetAddr("127.0.0.1", 8400);  //address of unity program
var num= 5;  //number of spheres
var ndefs= {|i|
    Ndef(("snd"++i).asSymbol, {|x, y, z, t_trig= 0|
        var snd= LFSaw.ar(y.linexp(-2, 1, 50, 500).lag+(i*50), 0, z.linlin(-1, 1, 0, 1));
        var efx= BPF.ar(snd, z.linexp(-1, 1, 100, 1000).lag(0.1), 0.2, 4).tanh;
        var env= EnvGen.ar(Env.perc(0.01, 1), t_trig, 1, 0.1);
        Pan2.ar(efx*env, x/6);
    }).play;
}.dup(5);
//incoming osc should be: /objs, index(0-4), x, y, z
OSCdef(\objs, {|msg|
    ndefs[msg[1]].set(\x, msg[2], \y, msg[3], \z, msg[4]);
}, \objs);
)
```

and a last example using a soundfile...

```supercollider
b.free;
b= Buffer.readChannel(s, "/Users/asdf/musicradar-nu-disco-samples/125bpm Loops n Lines/Guitar/ND_EnvGuitar_125_A-01.wav", channels:[0]);

(
var unity= NetAddr("127.0.0.1", 8400);  //address of unity program
var num= 5;  //number of spheres
var ndefs= {|i|
    Ndef(("snd"++i).asSymbol, {|x, y, z, t_trig= 0|
        var snd= BufRd.ar(1, b, K2A.ar(z).linlin(-6, 6, 0, BufFrames.kr(b)).lag(y.abs), 0);
        var env= EnvGen.ar(Env.perc(0.01, 1), t_trig, 1, 0.1);
        Pan2.ar(snd*env, x/6);
    }).play;
}.dup(5);
//incoming osc should be: /objs, index(0-4), x, y, z
OSCdef(\objs, {|msg|
    ndefs[msg[1]].set(\x, msg[2], \y, msg[3], \z, msg[4]);
}, \objs);
)
```

extra
--

similar examples from last semester <https://github.com/redFrik/udk16-Immersive_Technologies/tree/master/udk161208>

sending the position and rotation of a htc vive headset and controllers <https://github.com/redFrik/udk16-Immersive_Technologies/tree/master/udk170105#virtual-reality>
