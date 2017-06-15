textures
--------------------

here we try coding generative textures from scratch

```javascript
#pragma strict

public var tex : Texture2D;

function Start() {
    tex= new Texture2D(2, 2);
    GetComponent.<Renderer>().material.mainTexture= tex;  //connect texture to material of GameObject this script is attached to
}
function Update() {
    tex.SetPixel(0, 1, Color(
        (Mathf.Sin(Time.frameCount*0.04)+1)/2,
        (Mathf.Sin(Time.frameCount*0.05)+1)/2,
        (Mathf.Sin(Time.frameCount*0.06)+1)/2,
        1.0
    ));
    tex.Apply();
}
```

MORE TODO



sound textures
--

wall of sound, granular synthesis, drone music

can be done in many ways, but here we use `Ndef` and its `.spawn` method.

```supercollider
s.boot;

Env.sine(5).plot;  //the shape of the sound in this first example
{SinOsc.ar(500, 0, SinOsc.ar(2))*0.1}.play;  //the sound (simple oscillator - mono)

(
Ndef(\snd, {|amp= 0.1, dur= 15|
    var env= EnvGen.kr(Env.sine(dur), doneAction:2);
    var snd= SinOsc.ar(ExpRand(300, 3000), 0, SinOsc.ar(LinRand(1, 10)));  //
    Pan2.ar(snd, Rand(-1, 1), amp*env);
});
)

Ndef(\snd).play;  //start the first one - total duration 15 seconds
Ndef(\snd).spawn;  //add more - run this line multiple times
```

notice how we pick a random frequency for each spawned synth, as well as random tremolo speed between 1 and 10 (wobble rate). we also give a random panning position in the stereo field (values between -1.0 and 1.0 = left to right channel)

the classes `ExpRand` and `LinRand` are similar. `ExpRand` give random values in a exponential distribution and `LinRand` gives more low than high values.

the `.kr` method means control rate signals and can be used for slow changing signals to save cpu (a good idea here if we make many many synths). so perfect for envelopes and low-frequency oscillators (lfos).

```
//more elaborate sound - also using linen instead of sine envelope
(
Ndef(\snd, {|amp= 0.1, atk= 3, sus= 5, rel= 8|
    var env= EnvGen.kr(Env.linen(atk, sus, rel), doneAction:2);
    var snd= SinOsc.ar(ExpRand(200, 4000), 0, Decay2.ar(Impulse.ar(LinRand(1, 10)), 0.01, 2));
    //var snd= SinOsc.ar(ExpRand(200, 4000), 0, Decay2.ar(Dust.ar(LinRand(1, 10)), 0.01, 2));
    //var snd= SinOsc.ar(ExpRand(200, 1800).round(50), 0, Decay2.ar(Impulse.ar(LinRand(1, 10)), 0.01, 0.1));
    //var snd= SinOsc.ar(ExpRand(200, 1800).round(50), 0, Decay2.ar(Dust.ar(LinRand(10, 100)), 0.01, 0.1));
    //var snd= SinOscFB.ar(ExpRand(100, 1200).round(50), LFNoise2.kr(1)+1/2, Decay2.ar(Impulse.ar(LinRand(1, 100)), 0.01, 0.1));
    //var snd= SinOscFB.ar(LFNoise1.kr(0.01).exprange(100, 1000), 0.5, Decay2.ar(Impulse.ar(LinRand(1, 500)), 0.01, 0.05));
    Pan2.ar(snd, Rand(-1, 1), amp*env);
});
)

Ndef(\snd).play;  //one
Ndef(\snd).spawn; //add more - run multiple times
```

read the code. try out the different versions by uncomment different `var snd` lines (note: there can only by one `var snd` line uncommented at a time).

change the values and write your own variants.

a `Impulse.kr(2)` gives a regular trigger signal twice per second while `Dust.kr(2)` gives random triggers with an average of 2 impulses per second.

```
//with more noisy sounds instead of pitched oscillators
(
Ndef(\snd, {|amp= 0.1, atk= 4, sus= 6, rel= 8|
    var env= EnvGen.kr(Env.linen(atk, sus, rel, 1, 4), doneAction:2);
    var snd= BLowPass4.ar(Impulse.ar(ExpRand(1, 1000)), ExpRand(100, 1500), 1, 10);
    Pan2.ar(snd, SinOsc.kr(ExpRand(0.01, 1)), amp*env);
});
)

Ndef(\snd).play;  //one
Ndef(\snd).spawn; //add more - run multiple times
```

we can use any sound - also soundfiles.

```
b.free; b= Buffer.readChannel(s, "/Users/asdf/musicradar-nu-disco-samples/125bpm Loops n Lines/Beat Mixes/ND_BeatMixA125-01.wav", channels:[0]);

(
Ndef(\snd, {|amp= 0.1, atk= 2, sus= 8, rel= 4|
    var env= EnvGen.kr(Env.linen(atk, sus, rel), doneAction:2);
    var snd= PlayBuf.ar(1, b, 1, Impulse.ar(LinRand(0.1, 10)), Rand(0, BufFrames.kr(b)), loop:1);
    //var snd= PlayBuf.ar(1, b, LFSaw.kr(LinRand(0.001, 0.1)).exprange(0.5, 1.5), 1, 0, loop:1);
    //var snd= PlayBuf.ar(1, b, IRand(1, 3)-0.5, 1, 0, loop:1)*SinOsc.kr(ExpRand(1, 5));
    //var snd= PlayBuf.ar(1, b, env*Rand(0.95, 1.05), 1, 0, loop:1);
    //var snd= PlayBuf.ar(1, b, Rand(0.667, 1.1), Impulse.kr(env*10), LFNoise2.kr(LinRand(0.001, 0.5)).range(0, BufFrames.kr(b)), loop:1);
    Pan2.ar(snd, Rand(-0.75, 0.75), amp*env);
});
)

Ndef(\snd).play;  //one
Ndef(\snd).spawn; //add more - run multiple times
```

above we take a mono buffer and experiment with different ways of playing back the sample data. the `env` still shape the sound but in some versions we also use that shape to set the playback rate.
looping is always on, but it could also be off so that the file stops playing when it reached the end.
start position can be random or 0 to always begin from the beginning of the file.
the trigger makes the PlayBuf jump to the start position. this can be modulated with a regular trigger signal (Impulse - will give a steady rhythm) or a random trigger (Dust).
