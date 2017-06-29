panning
--------------------

best to use headphones for the following.

sphere
--

* start unity and create a new 3D project
* set the Main Camera position in inspector window to x:0, y:0, z: 0 (reset position in drop down meny)
* select Assets / Create / Folder
* call the folder 'Resources'
* open the folder and drag&drop some soundfiles into it (or a whole folder with soundsfiles)
* select GameObject / 3D Objects / Sphere
* click 'Add Component' in the inspector and then 'Audio / Audio Source'
* select a soundfile in inspector under AudioSource / AudioClip
* select 'Loop' right below
* set 'Spacial Blend' to 1
* optionally set 'Max Distance' to 50
* click 'Add Component' in the inspector and then 'New Script'
* give the script a name (here 'move') and make sure language is JavaScript
* doubleclick the script symbol and paste in the code here below

```javascript
#pragma strict

public var xspeed = 0.05;
public var zspeed = 0.05;
public var xdist = 10;
public var zdist = 10;
public var zoffset = 10;

function Start() {
}

function Update() {
    transform.localPosition.x= Mathf.Sin(Time.frameCount*xspeed)*xdist;
    transform.localPosition.z= Mathf.Cos(Time.frameCount*zspeed)*zdist+zoffset;
}
```

press play and fiddle around with the parameters. try changing xspeed, play with doppler level, draw other curves in the 3d sound settings graph

note: if you do not get any sound try with other soundfiles. (files recorded with supercollider is 32bit by default and unity seem not happy with them - convert to 16bit or mp3 and it should work)

to get a better overview you can also select the scene tab while playing (first deselect maximize on play). you should now see how the sphere moves around the main camera. like this...

![sphere](00sphere.png?raw=true "sphere")

* stop and open Window / Audio Mixer
* click '+' to add a mixer and call it something (here 'main')
* click the 'Add Effect' in the inspector
* add a few (here Distortion and Echo)
* go back to Sphere inspector window and set audio output to Master (main)
* play
* go back to audio mixer window and select 'Edit in Playmode'

![effects](01effects.png?raw=true "effects")

you should be able to change the effect parameters in realtime. if you don't hear any effect double check that you set the audio output to Master like in the 5th step above.

play around with the effect settings, also change the javascript code and try different movements.

try setting xspeed to 0.0 and the sphere should only move away and towards you.

add more objects with more soundfiles and effects (simplest is to just duplicate the sphere and then change soundfile, speeds etc)

many textures
--

change the script to this... (similar but with y control and also added texture sample data from last week)

```javascript
#pragma strict

private var tex : Texture2D;
private var samples : float[];
public var snd : AudioSource;

public var xspeed = 0.05;
public var yspeed = 0.015;
public var zspeed = 0.05;
public var xdist = 10;
public var ydist = 3;
public var zdist = 10;
public var xoffset = 0;
public var yoffset= 0;
public var zoffset = 10;
public var amp= 5;  //scale the sound data

function Start() {
    snd= GetComponent.<AudioSource>();
    samples= new float[snd.clip.samples*snd.clip.channels];
    tex= new Texture2D(32, snd.clip.channels);
    GetComponent.<Renderer>().material.mainTexture= tex;
}

function Update() {
    transform.localPosition.x= Mathf.Sin(Time.frameCount*xspeed)*xdist+xoffset;
    transform.localPosition.y= Mathf.Cos(Time.frameCount*yspeed)*zdist+yoffset;
    transform.localPosition.z= Mathf.Sin(Time.frameCount*zspeed)*zdist+zoffset;
    snd.clip.GetData(samples, snd.timeSamples);
    for(var y= 0; y<tex.height; y++) {
        for(var x= 0; x<tex.width; x++) {
            var sample= samples[x]*amp;
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
try setting the clear flag in main camera inspector to 'do not clear'.

![multiple](02multiple.png?raw=true "multiple")

remember you can also select a few objects and let them be the children of a single object. so some spheres circle around other spheres. do this by drag&drop in the hierarchy window. see left hand side of the screenshot right above.

note that if you have a complex scene or slow computer the graphics might slow down the audio panning and it will not sound as good (if the framerate is low or unregular we hear that the sound is jumping). also your sounds might pan around faster on a faster computer or have hickups if you run something else in the background.
so in general it is not a good idea to move sounds around using a graphics program - it'll work if you watch the framerate and keep the scene and objects simple, but better is to use an audio program like supercollider... 

multichannel
--

by default supercollider uses 2 channels for output (stereo: left and right).
to use more channels (many speakers in the room) we need to set an option before we boot the soundserver.

```supercollider
s.options.numInputBusChannels= 8;  //should match number of inputs on your soundcard
s.options.numOutputBusChannels= 8;  //should match number of outputs on your soundcard
s.reboot;  //make sure to start with the new options
s.meter;

//play sinetones in two channels - 400 in left, 500 in right
a= {SinOsc.ar([400, 500], 0, 0.4)}.play;
a.free;

//play sinetones in three channels
//although without a soundcard we will only hear the two first sines (400 & 500) 
a= {SinOsc.ar([400, 500, 600], 0, 0.4)}.play;
a.free;

//play sinetones in all eight channels
a= {SinOsc.ar([400, 500, 600, 700, 800, 900, 1000, 1100], 0, 0.4)}.play;
a.free;

//this will take sound input from 8 channels (8 mics?) and play out in 8 speakers
//all have slightly different delaytime
a= {DelayN.ar(SoundIn.ar([0, 1, 2, 3, 4, 5, 6, 7]), 2, [2, 0.95, 1.8, 1.7, 0.6, 0.9, 1.1, 1.3]}.play;
a.free;
```

to move sound between speakers we can use the built in panning ugens...

```supercollider
//pan a sound left and right (pan2) using the mouse 
a= {Pan2.ar(SinOsc.ar(400), MouseX.kr(-1, 1))}.play;
a.free;

//same but with automation
a= {Pan2.ar(SinOsc.ar(400), SinOsc.kr(0.1))}.play;
a.free;

//faster
a= {Pan2.ar(SinOsc.ar(400), SinOsc.kr(1))}.play;
a.free;

//even faster
a= {Pan2.ar(SinOsc.ar(400), SinOsc.kr(10))}.play;
a.free;

//really fast can give interesting artifacts (modulation)
a= {Pan2.ar(SinOsc.ar(400), SinOsc.kr(100))}.play;
a.free;

//often noise sounds are easier to locate
//bass tones and static tones is hard for us humans to position
a= {Pan2.ar(PinkNoise.ar(0.4), SinOsc.kr(0.1))}.play;
a.free;
```
a nice feature of supercollider is that it is relatively easy to change your code for different number of speakers.

```supercollider
//for the following to work you'll need a soundcard + 8 speakers
//to just see the effect look at the level meter window `s.meter` 

//move sound around in 8 channels (speakers should be in a circle)
a= {PanAz.ar(8, SinOsc.ar(400), MouseX.kr(-1, 1))}.play;
a.free;

//same but with 6 speakers
a= {PanAz.ar(6, SinOsc.ar(400), MouseX.kr(-1, 1))}.play;
a.free;

//automatically move 3 microphones around in 8 speakers...
a= {PanAz.ar(8, SoundIn.ar(0), SinOsc.kr(0.06))}.play;
b= {PanAz.ar(8, SoundIn.ar(1), SinOsc.kr(0.05))}.play;
c= {PanAz.ar(8, SoundIn.ar(2), SinOsc.kr(0.04))}.play;
a.free; b.free; c.free;  //stop all
```

externals
--

to do more advanced sound spacialization it is recommended to use some kind of plugin.

* VBAP (part of sc3-plugins) <https://github.com/supercollider/sc3-plugins/releases>
* Beast <http://www.birmingham.ac.uk/facilities/ea-studios/research/mulch.aspx>
* ATK <http://www.ambisonictoolkit.net>
* Zirkonium <http://zkm.de/en/institutes-research-centers/zkm-institute-for-music-and-acoustics/software/zirkonium>
* etc.

soundcards
--

* Focusrite (scarlett) - good
* motu - have many good cards
* rme - very great but expensive
* komplete audio - don't touch
* etc.

extra
--

in unity select Edit / Project Settings / Quality and then turn off 'V Sync Count'. watch how your scene now run much faster. you can see the framerate if you click the little 'Stats' button.
vertical sync should always be *on* though. when vsync is on the program will never update the screen when the scanline is in the middle of the window drawing. it will draw the complete frame as rendered. if vsync is off you will get tearing artifacts.

![framerate](03framerate.png?raw=true "framerate")

now also try changing the quality levels. this might be useful to know about if you want to render some hi res stills.

to change the general framerate you need to use a script. see <https://docs.unity3d.com/ScriptReference/Application-targetFrameRate.html>
