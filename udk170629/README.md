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

you can also select the scene tab if you want to get an overview and see how the sphere moves around.

![sphere](00sphere.png?raw=true "sphere")

* stop and open Window / Audio Mixer
* click '+' to add a mixer and call it something (here 'main')
* click the 'Add Effect' in the inspector
* add a few (here Distortion and Echo)
* go back to Sphere inspector window and set audio output to Master (main)
* play
* go back to audio mixer window and select 'Edit in Playmode'

![effects](01effects.png?raw=true "effects")

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

multichannel
--

```supercollider
s.options.numOutputBusChannels= 8;
s.reboot;
s.meter;

Splay
PlayAz
```

