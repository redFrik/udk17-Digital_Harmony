samples
--------------------

again we will use textures in unity.

audio samples
--

* start unity and create a new 3D project
* select GameObject / 3D Objects / Plane
* set rotation x to -45 in inspector
* click 'Add Component' in the inspector and then 'Audio / Audio Source'
* click 'Add Component' in the inspector and then 'New Script'
* give the script a name (here 'audio') and make sure language is JavaScript
* doubleclick the script symbol and paste in the code here below

```javascript
#pragma strict

private var tex : Texture2D;
private var samples : float[];
private var snd : AudioSource;

function Start() {
    snd= GetComponent.<AudioSource>();
    snd.clip= Microphone.Start(null, true, 1.0, 44100);  //1.0 is buffersize in seconds, 44100 samplerate
    snd.loop= true;
    while(!(Microphone.GetPosition(null)>0)) {}
    //snd.Play();	//monitor
    samples= new float[snd.clip.samples*snd.clip.channels];
    tex= new Texture2D(256, snd.clip.channels);
    GetComponent.<Renderer>().material.mainTexture= tex;
}
function Update() {
    var pos= Microphone.GetPosition(null);
    snd.clip.GetData(samples, 0);
    for(var y= 0; y<tex.height; y++) {
        for(var x= 0; x<tex.width; x++) {
            var sampleIndex= pos-x;
            if(sampleIndex<0) {
                sampleIndex= sampleIndex+samples.Length;
            }
            var sample= samples[sampleIndex];
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

click play in unity and tap the microphone. you should see something like this...

![mic](00mic.png?raw=true "mic")

if not the check that you have 'built-in microphone' selected as sound input in your system preferences.
