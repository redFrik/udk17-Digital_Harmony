introduction and overview
--------------------

* links to previous semesters... <https://redfrik.github.io/udk00-Audiovisual_Programming/>
* and dates + times for this course... <https://github.com/redFrik/udk17-Digital_Harmony> <-save this page

unity3d
--

* install [unity](https://unity3d.com)
* create a new 2d project and call it something (here 'picture')

![picture](01picture.png?raw=true "picture")

* find a picture file on your harddrive or online. `.png` or `.jpg` will work.
* drag and drop it onto the main scene area.
* click and drag the picture to position it somewhere in the middle.
* select menu 'GameObject/Particle System'. should now look like this...

![particles](02particles.png?raw=true "particles")

* on the right hand side find the 'Inspector' tab
* set 'Simulation Space' to 'World'
* expand 'Emission' and set 'Rate over Time' to 100
* expand 'Shape' and set 'Shape' to 'Sphere'

you might notice strange flickering here - it is due to the background picture and the particle system are rendered on the same layer and the ordering is unclear (both 0). so next we fix that problem by making sure the particle system is one layer higher than the background picture.

* expand 'Renderer' and set 'Order in Layer' to 1 (also try -1 to see what happens)

now we will add a script to make the particle position follow our mouse pointer.

* scroll to the bottom of the inspector and find 'Add Component'
* select 'New Script', call it something (here 'mouse'), make sure language is set to JavaScript and click 'Create and Add'
* in your Project tab window you should now see a white Js icon like this...

![script](03script.png?raw=true "script")

* double click the icon to start MonoDevelop (first time it might ask you to install it - takes a while)
* replace the code that is there with the following...

```javascript
#pragma strict

function Update() {
    var ray: Ray= Camera.main.ScreenPointToRay(Input.mousePosition);
    transform.position= ray.GetPoint(120);
}
```

* save and go back to Unity.
* press the play button (top center of screen) and you should see the pacticles following your mouse.

now experiment with the settings for the particle system. to bring up the Inspector tab again select the 'Particle System' in the Hierarchy window on the left.

**NOTE:** any changes you make while the scene is playing will not be saved. to make permanent changes click the play button again to stop, perform the changes in the inspector and then press play again.

suggested changes:

* compare 'Simulation Space' set to 'Local' vs 'World'
* tick and expand 'Trails' and set 'Lifetime' to 0.2 (also scroll down to 'Renderer' and find 'Trail Material'. click on the circle and set it to 'Default-Particle')

play with 'Noise', 'Gravity Modifier', 'Size over Lifetime' etc.

supercollider
--

* install [supercollider](https://supercollider.github.io/download)
* start it and type...

```supercollider
s.boot;
```
* find the 'Language' menu and select 'Evaluate Selection or Line'.

your sound server should now start and in the lower right corner you should see some text in green.

* now type the following and evaluate (run) it...

```supercollider
s.meter;
```

you should see a level meter window.

* drag and drop a soundfile onto the code window. `.aif` or `.wav` will work (not `mp3`).
* edit the code below and replace the `/Users/asdf/...` with the result of the text that showed up when you drag your soundfile.

```supercollider
b= Buffer.readChannel(s, "/Users/asdf/Desktop/Main Beats/ND_BeatA128-01.wav", channels:[0]);
b.play;  //test that it is working - you should here the file playing
//stop with cmd+. on osx, ctrl+. on windows

Ndef(\player1, {Pan2.ar(GrainBuf.ar(1, Dust.kr(MouseX.kr(1, 20)), 1, b, MouseY.kr(0, 20)))}).play;

Ndef(\player2, {Pan2.ar(GrainBuf.ar(1, Impulse.kr(MouseX.kr(1, 20)), 1, b, MouseY.kr(0, 20)))}).play

Ndef(\player3, {Pan2.ar(GrainBuf.ar(1, Impulse.kr(MouseX.kr(1, 20)*MouseButton.kr), 1, b, 1, MouseY.kr(0, 1)))}).play
```

run the code and explore by changing different numbers.

the following line will generate a gui for your three ndefs...

```supercollider
NdefMixer(s);
```

now run supercollider in the background with the unity scene on top - notice how you now can relate sound settings with the help of mouse position on the background picture.

extra
--

to add mouse button control to the particle system, open the script in MonoDevelop and replace what is there with the following...

```javascript
#pragma strict

function Update() {
    var ray: Ray= Camera.main.ScreenPointToRay(Input.mousePosition);
    transform.position= ray.GetPoint(120);

    var ps: ParticleSystem= GetComponent.<ParticleSystem>();
    var em= ps.emission;
    if(Input.GetMouseButton(0)) {
        em.enabled= true;
    } else {
        em.enabled= false;
    }
}
```
