instantiation
--------------------

let us look at how to automatically create objects from code.

wall
--

one way to generate objects in unity is to first create a prefab and then in a script make unique objects of that.

* create a new 3d unity project
* select GameObject / 3D Object / Cube (or some other type)
* select Component / Physics / Rigidbody
* select Assets / Create / Prefab
* give the prefab a name (here 'myobj')
* drag&drop the Cube from the Hierarchy window onto the prefab
* control click the Cube in the Hierarchy window and delete it

now we have a very simple prefab to play around with. you can also import readymade prefabs.

* select GameObject / 3D Object / Plane
* select GameObject / Create Empty
* click 'Add Component' in the inspector and then 'New Script'
* give the script a name (here 'mycode') and make sure language is JavaScript
* doubleclick the mycode symbol and paste in the code here below

```javascript
#pragma strict

var prefab : Transform;
private var num= 5;
function Start() {
    for(var y= 0; y<num; y++) {
        for(var x= 0; x<num; x++) {
            Instantiate(prefab, Vector3(x, y, 0), Quaternion.identity);
        }
    }
}
function Update() {
}
```

save and switch back to Unity.

* select the 'GameObject' in the Hierarchy window
* drag&drop the prefab ('myobj') onto the Prefab slot in the inspector

your scene should look like this...

![prefab](00prefab.png?raw=true "prefab")

and when you run...

![wall](01wall.png?raw=true "wall")

investigate the code. try changing the num variable. try adding a 3rd dimention z. change the world gravity (Edit / Project Settings / Physics), try changing the myobj prefab's scale. add a material, play with the rigidbody settings etc.

balls
--

* create a new 3d unity project
* select GameObject / 3D Object / Sphere
* select Assets / Create / Material
* change the colour and some other parameters for the material in the inspector window
* drag&drop the material onto the Sphere in the Hierarchy window
* select Assets / Create / Prefab
* optionally give the prefab a name (here 'myball')
* drag&drop the Sphere from the Hierarchy window onto the prefab
* control click the Sphere in the Hierarchy window and delete it
* select GameObject / Create Empty
* click 'Add Component' in the inspector and then 'New Script'
* give the script a name (here 'mycode') and make sure language is JavaScript
* doubleclick the mycode symbol and paste in the code here below

```javascript
#pragma strict

var prefab : Transform;
var rad= 2.5;
private var num= 80;
function Start() {
    for(var i= 0; i<num; i++) {
        var fi : float = i;
        var obj= Instantiate(prefab, Vector3(Mathf.Sin(fi/num*Mathf.PI*2)*rad, Mathf.Cos(fi/num*Mathf.PI*2)*rad, 0), Quaternion.identity);
        obj.tag= "Player";  //use an already existing tag - could make a new dedicated
    }
}
function Update() {
}
```

save and switch back to Unity.

* select the 'GameObject' in the Hierarchy window
* drag&drop the prefab ('myobj') onto the Prefab slot in the inspector

when you click run in unity you should now see something like this...

![balls](02balls.png?raw=true "balls")

again play around with the code, try to add a depth (z), change the `*2` and observe the effect, mess with the prefab in unity - scale, material etc

change the Update function in the code to this...
```javascript
function Update() {
    var objects : GameObject[];
    objects= GameObject.FindGameObjectsWithTag("Player");
    for(var i= 0; i<num; i++) {
        objects[i].transform.localPosition.x= Mathf.Sin((Time.frameCount*0.03)+(i*0.06))*rad;
        objects[i].transform.localPosition.y= Mathf.Cos((Time.frameCount*0.04)+(i*0.05))*rad;
        objects[i].transform.localPosition.z= Mathf.Sin((Time.frameCount*0.06)+(i*0.04))*rad;
    }
}
```

* select the myball prefab in the Project window
* select Component / Physics / Spring Joint

that added both a rigidbody and an anchor (spring joint) to the prefab and thereby also to all the balls instances we create from it.

again change the Update function to the following...

```javascript
function Update() {
    var objects : GameObject[];
    objects= GameObject.FindGameObjectsWithTag("Player");
    var mousePos= Input.mousePosition;
    mousePos.z= 10;
    objects[0].transform.localPosition= Camera.main.ScreenToWorldPoint(mousePos);  //set first ball to mouse pos
    //objects[0].transform.localScale= Camera.main.ScreenToWorldPoint(mousePos);  //set first ball to mouse pos
}
```

synthdef
--

```supercollider
s.boot;

(
SynthDef(\thursday, {|amp= 1, freq= 500|
    //var env= EnvGen.ar(Env.perc(0.01, 1), doneAction:2);
    var env= EnvGen.ar(Env.sine(1), doneAction:2);
    var snd= SinOsc.ar(freq, 0, amp);
    Out.ar(0, Pan2.ar(snd*env, 0));
}).add;
)

Env.perc(0.01, 1).plot;
Env.sine(1).plot;

Synth(\thursday);
Synth(\thursday, [\freq, 600]);
Synth(\thursday, [\freq, 400.rand+100]);

(
40.do{
    Synth(\thursday, [\freq, 400.rand+100, \amp, 1.0.rand]);
};
)

(
SynthDef(\ping, {|freq= 400, atk= 0.1, rel= 0.5, amp= 1, pan= 0|
    var env= EnvGen.ar(Env.perc(atk, rel, amp), doneAction:2);
    var snd= SinOsc.ar(freq, env*2pi);
    Out.ar(0, Pan2.ar(snd*env, pan));
}).add;
)

Synth(\ping);
Synth(\ping, [\freq, 900]);
Synth(\ping, [\freq, 900, \pan, 0.75, \amp, 0.3]);
Synth(\ping, [\freq, 1900, \pan, 0.75, \amp, 0.3, \atk, 0.001, \rel, 2, \pan, -0.75]);
```


```supercollider
(
SynthDef(\snare, {|atk= 0.01, rel= 0.1, amp= 1, pan= 0|
    var env= EnvGen.ar(Env.perc(atk, rel, amp), doneAction:2);
    var snd= ClipNoise.ar;
    Out.ar(0, Pan2.ar(snd*env, pan));
}).add;
SynthDef(\kick, {|freq= 60, atk= 0.01, rel= 0.1, amp= 1, pan= 0|
    var env= EnvGen.ar(Env.perc(atk, rel, amp), doneAction:2);
    var snd= SinOscFB.ar(freq, 1);
    Out.ar(0, Pan2.ar(snd*env, pan));
}).add;
SynthDef(\hat, {|freq= 3000, atk= 0.01, rel= 0.05, amp= 1, pan= 0|
    var env= EnvGen.ar(Env.perc(atk, rel, amp), doneAction:2);
    var snd= BPF.ar(ClipNoise.ar, freq, 0.5, 2);
    Out.ar(0, Pan2.ar(snd*env, pan));
}).add;
)

Synth(\snare)
Synth(\kick)
Synth(\hat)
```

pbind
--

```supercollider
a= Pbind(\instrument, \ping).play
a.stop
a= Pbind(\instrument, \ping, \freq, Pseq([400, 200, 600, 700], inf)).play
a.stop
a= Pbind(\instrument, \ping, \freq, Pseq([400, 200, 600, 700], inf), \rel, 2, \dur, 0.3).play
a.stop

(
Pbind(\instrument, \hat, \dur, Pseq([0.25, 0.5], inf)).play;
Pbind(\instrument, \snare, \dur, 0.25).play;
Pbind(\instrument, \kick, \dur, 0.5).play;
)

(
Pbind(\instrument, \hat, \dur, Pseq([0.25, 0.5], inf)).play;
Pbind(\instrument, \snare, \dur, 0.25).play;
Pbind(\instrument, \kick, \dur, 0.5, \freq, Pseq([50, 500, 60, 99, 60], inf)).play;
)

//change the sound while the pbind is playing
(
SynthDef(\hat, {|amp= 1, pan= 0|
    var env= EnvGen.ar(Env.perc(0.01, 0.1), doneAction:2);
    var snd= Saw.ar(500*(env+1));
    Out.ar(0, Pan2.ar(snd*env, pan));
}).add;
)

(
SynthDef(\hat, {|amp= 1, pan= 0|
    var env= EnvGen.ar(Env.perc(0.001, 0.1, 1, -6), doneAction:2);
    var snd= HPF.ar(WhiteNoise.ar, 2-env*1000);
    Out.ar(0, Pan2.ar(snd*env, pan));
}).add;
)

(
//amplitude patterns - 8 steps
Pbind(\instrument, \snare, \dur, 0.125, \amp, Pseq([0, 0, 0, 1, 0, 0, 0.1, 1], inf)).play;
Pbind(\instrument, \kick,  \rel, Pseq([1, 0.5, 0.2, 0.1, 0.01], inf), \dur, Pseq([0.125, 0.125, 0.25], inf), \amp, Pseq([1, 0, 0, 0, 1, 0, 0.5], inf), \freq, 60).play;
Pbind(\instrument, \hat,   \dur, 0.125, \amp, Pseq([0, 1, 0, 1, 0, 1, 0, 1], inf)).play;
)


TempoClock.default.tempo=80/60


(
//pdef - change while playing (no need to stop after changing the pattern code - just reevaluate and keep modeling the patterns)
Pdef(\snarep, Pbind(\instrument, \snare, \dur, 0.25, \amp, Pseq([0, 0, 0, 1, 0, 0, 0.1, 1], inf))).play;
Pdef(\kickp, Pbind(\instrument, \kick,  \rel, Pseq([1, 0.5, 0.2, 1.1, 0.01, 0.1, 0.1], inf), \dur, Pseq([0.125, 0.125, 0.25], inf), \amp, Pseq([1, 0, 0, 0, 1, 0, 0.5], inf), \freq, Pseq([60, 80, 90], inf))).play;
Pdef(\hatp, Pbind(\instrument, \hat,   \dur, 0.25, \amp, Pseq([0, 1, 0, 1, 0, 1, 0, 1], inf))).play;
)

(
//patterns can be nested - see snare's \dur
Pdef(\snarep, Pbind(\instrument, \snare, \dur, Pseq([0.25, 0.25, 0.25, Pseq([0.125], 4)], inf), \amp, Pseq([0, 0, 0, 1, 0, 0, 0.1, 1], inf))).play;
Pdef(\kickp, Pbind(\instrument, \kick,  \rel, Pseq([1, 0.5, 0.2, 1.1, 0.01, 0.1], inf), \dur, Pseq([0.125, 0.125, 0.25], inf), \amp, Pseq([1, 0, 0, 0, 1, 0, 0.5], inf), \freq, Pseq([60, 80, 90], inf))).play;
Pdef(\hatp, Pbind(\instrument, \hat,   \dur, 0.25, \amp, Pseq([0, 1, 0, 1, 0, 1, 0, 1], inf))).play;
)

(
//patterns can also be added, multiplied, subtracted etc - see kick's \freq
Pdef(\snarep, Pbind(\instrument, \snare, \dur, Pseq([0.25, 0.25, 0.25, Pseq([0.125], 4)], inf), \amp, Pseq([0, 0, 0, 1, 0, 0, 0.1, 1], inf))).play;
Pdef(\kickp, Pbind(\instrument, \kick,  \rel, Pseq([1, 0.5, 0.2, 1.1, 0.01, 0.1], inf), \dur, Pseq([0.125, 0.125, 0.25], inf), \amp, Pseq([1, 0, 0, 0, 1, 0, 0.5], inf), \freq, Pseq([60, 80, 90], inf)*Pstutter(4, Pseq([1, 1, 1, 1, 2, 3], inf)))).play;
Pdef(\hatp, Pbind(\instrument, \hat,   \dur, 0.25, \amp, Pseq([0, 1, 0, 1, 0, 1, 0, 1], inf))).play;
)

(
//or use randomness - see hat's \freq and \instrument
Pdef(\snarep, Pbind(\instrument, \snare, \dur, Pseq([0.25, 0.25, 0.25, Pseq([0.125], 4)], inf), \amp, Pseq([0, 0, 0, 1, 0, 0, 0.1, 1], inf))).play;
Pdef(\kickp, Pbind(\instrument, \kick,  \rel, Pseq([1, 0.5, 0.2, 1.1, 0.01, 0.1], inf), \dur, Pseq([0.125, 0.125, 0.25], inf), \amp, Pseq([1, 0, 0, 0, 1, 0, 0.5], inf), \freq, Pseq([60, 80, 90], inf)*Pstutter(4, Pseq([1, 1, 1, 1, 2, 3], inf)))).play;
Pdef(\hatp, Pbind(\instrument, Pwrand([\hat, \ping], [0.6, 0.4], inf), \freq, Prand([320, 600, 2400], inf),  \rel, 0.6, \dur, 0.25, \amp, Pseq([0, 1, 0, 1, 0, 1, 0, 1], inf))).play;
)

//make your changes start at the next bar
Pdef(\snarep).quant=4;
Pdef(\kickp).quant=4;
Pdef(\hatp).quant=4;

Pdef(\snarep).stop;
Pdef(\kickp).stop;
Pdef(\hatp).stop;
```

extra
--

study 'spacelab.scd' in SuperCollider / examples / pieces folder

links
--

<https://docs.unity3d.com/Manual/InstantiatingPrefabs.html>

<https://github.com/redFrik/udk16-Immersive_Technologies/tree/master/udk170112#falling-objects>
