various
--------------------

questions and ideas that have come up.

mesh distortion
--

an example of distorting or displacing mesh vertices.

* start unity and create a new 3D project
* select GameObject / 3D Objects / Sphere
* scale it up to x:5, y:5, z:5 in the inspector
* click 'Add Component' in the inspector and then 'New Script'
* give the script a name (here 'deform') and make sure language is JavaScript
* doubleclick the script symbol and paste in the code here below


```javascript
#pragma strict

public var scalex = 0.7;
public var scaley = 0.7;
public var scalez = 0.7;
public var distx = 2.0;
public var disty = 2.0;
public var distz = 2.0;
public var speedx = 1.1;
public var speedy = 1.2;
public var speedz = 1.3;
private var mesh : Mesh;
private var baseVertices : Vector3[];

function Start() {
    mesh = GetComponent(MeshFilter).mesh;
    baseVertices = mesh.vertices;
}
function Update() {
    var vertices = new Vector3[baseVertices.Length];
    for (var i= 0; i<vertices.Length; i++) {
        var vertex = baseVertices[i];
        vertex.x += Mathf.Sin((Time.time*speedx)+(vertex.x*distx))*scalex;
        vertex.y += Mathf.Sin((Time.time*speedy)+(vertex.y*disty))*scaley;
        vertex.z += Mathf.Sin((Time.time*speedz)+(vertex.z*distz))*scalez;
        vertices[i] = vertex;
    }
    mesh.vertices= vertices;
    mesh.RecalculateNormals();  //optional - can comment out if you don't care about light
    mesh.RecalculateBounds();  //also optional - comment out to save cpu
}
```

run and play around with the variables in the inspector. study the code and improve it. try attach this script to other models and prefabs.

![deform](00deform.png?raw=true "deform")

now let us add a texture coming from the built-in webcamera. 

* select the sphere (or whatever objects your distort script above is attached to)
* click 'Add Component' in the inspector and then 'New Script'
* give the script a name (here 'webcam') and make sure language is JavaScript
* doubleclick the script symbol and paste in the code here below

```javascript
#pragma strict

function Start() {
    var webcamTexture: WebCamTexture = new WebCamTexture();
    var renderer: Renderer = GetComponent.<Renderer>();
    renderer.material.mainTexture = webcamTexture;
    webcamTexture.Play();
    //GetComponent.<Renderer>().material.shader= Shader.Find("Mobile/Bumped Diffuse");
    //GetComponent.<Renderer>().material.shader= Shader.Find("Unlit/Transparent");  //this one is good if you also turn off the light
} 
```

when you click run your webcamera should start and you should see an image mapped onto the object moving around.

again play around with the variables, try the suggested shaders, turn off the default directional light, try to set main camera clear flags to solid color or don't clear.   

recording sound
--

use the following script to record soundfiles from within unity. note: 16bit audio only.

* attach this script to main camera
* run and press 'r' to start/stop sound recording
* the file will end up in your scene folder
* code adapted from <https://forum.unity3d.com/threads/writing-audiolistener-getoutputdata-to-wav-problem.119295/>

```javascript
#pragma strict

import System.IO;
import System;

public var filename : String = "recTest.wav";
private var headerSize : int = 44;    //default for uncompressed wav
private var fileStream : FileStream;
private var listener : AudioListener;
private var isRecording : boolean = false;

function Start() {
    listener= gameObject.GetComponent(AudioListener);
}
function Update() {
    if(Input.GetKeyDown("r")) {    //use key R to start/stop recording
        if(!isRecording) {
            Debug.Log("starting to record");
            StartWriting(filename);
            isRecording= true;
        } else {
            isRecording= false;
            WriteHeader();
            Debug.Log("done recording");
        }
    }
}
function StartWriting(name : String) {
    fileStream = new FileStream(name, FileMode.Create);
    var emptyByte : byte = new byte();
    for(var i : int = 0; i<headerSize; i++) {
        fileStream.WriteByte(emptyByte);
    }
}
function OnAudioFilterRead(data : float[], channels : int) {
    if(isRecording) {
        ConvertAndWrite(data);    //audio data is interlaced
    }
}
function ConvertAndWrite(dataSource : float[]) {
    var intData : Int16[] = new Int16[dataSource.length];
    var bytesData : Byte[] = new Byte[dataSource.length*2];
    var rescaleFactor : int = 32767;
    for (var i : int = 0; i<dataSource.length;i++){
        intData[i] = dataSource[i]*rescaleFactor;
        var byteArr : Byte[] = new Byte[2];
        byteArr = BitConverter.GetBytes(intData[i]);
        byteArr.CopyTo(bytesData, i*2);
    }
    fileStream.Write(bytesData, 0, bytesData.length);
}
function WriteHeader() {
    fileStream.Seek(0, SeekOrigin.Begin);
    var riff : Byte[] = System.Text.Encoding.UTF8.GetBytes("RIFF");
    fileStream.Write(riff, 0, 4);
    var chunkSize : Byte[] = BitConverter.GetBytes(fileStream.Length-8);
    fileStream.Write(chunkSize, 0, 4);
    var wave : Byte[] = System.Text.Encoding.UTF8.GetBytes("WAVE");
    fileStream.Write(wave, 0, 4);
    var fmt : Byte[] = System.Text.Encoding.UTF8.GetBytes("fmt ");
    fileStream.Write(fmt, 0, 4);
    var subChunk1 : Byte[] = BitConverter.GetBytes(16);
    fileStream.Write(subChunk1, 0, 4);
    var two : UInt16 = 2;
    var one : UInt16 = 1;
    var audioFormat : Byte[] = BitConverter.GetBytes(one);
    fileStream.Write(audioFormat, 0, 2);
    var numChannels : Byte[] = BitConverter.GetBytes(two);
    fileStream.Write(numChannels, 0, 2);
    var sampleRate : Byte[] = BitConverter.GetBytes(AudioSettings.outputSampleRate);
    fileStream.Write(sampleRate, 0, 4);
    var byteRate : Byte[] = BitConverter.GetBytes(AudioSettings.outputSampleRate*4);
    fileStream.Write(byteRate, 0, 4);
    var four : UInt16 = 4;
    var blockAlign : Byte[] = BitConverter.GetBytes(four);
    fileStream.Write(blockAlign, 0, 2);
    var sixteen : UInt16 = 16;
    var bitsPerSample : Byte[] = BitConverter.GetBytes(sixteen);
    fileStream.Write(bitsPerSample, 0, 2);
    var dataString : Byte[] = System.Text.Encoding.UTF8.GetBytes("data");
    fileStream.Write(dataString, 0, 4);
    var subChunk2 : Byte[] = BitConverter.GetBytes(fileStream.Length-headerSize);
    fileStream.Write(subChunk2, 0, 4);
    fileStream.Close();
}
```

syphon
--

these instructions are ***mac osx*** only. for windows there is a similar texture sending application here... <http://spout.zeal.co>

below demonstrates how to set up Funnel to make your unity scene send graphics to a [syhpon](http://syphon.v002.info) client (i.e. to another application on your own mac or on another computer).

note: here we create a new scene but you can also use an old project.

* start unity and create a new 3D project
* select GameObject / Particle System
* download Funnel by clicking 'the plugin package' on [this](https://github.com/keijiro/Funnel/tree/gllegacy#setting-up) page
* doubleclick the file 'Funnel.unitypackage' to import it into unity
* in unity find the Funnel C# script and drag&drop it onto the main camera
* when you now run your screen should go black and you should see the text 'No cameras rendering'
* select 'Edit / Project Settings / Player' and tick 'Run In Background'
* optionally: in Main Camera inspector set screen width&height to 1920x1080 and Render Mode to 'Preview On Game View' 

![syphon](01syphon.png?raw=true "syphon")

* go to <https://github.com/Syphon/Simple/releases/tag/version-3> and download Syphon Simple Apps 3
* start 'Simple Client.app' and connect to your unity syphon server

![syphon2](02syphon2.png?raw=true "syphon2")

there are clients for processing, max, isadora and many more. see <http://syphon.v002.info>

you can also use it to record your scenes to disk. get [Syphon Recorder](http://syphon.v002.info/recorder/). don't forget to set the preferences (experiment with type of compression, framerate, dimensions etc) 

and to send over network try <http://techlife.sg/TCPSyphon/>

notes on optimisation: try to keep the resolution down, don't send more pixels than your video projector is eventually using, turn off preview if you can and use 'Send Only' mode, connect with ethernet cables instead of wifi.

extra
--

terrain heightmaps from real world data <https://www.youtube.com/watch?v=-vyNbalvXR4&t=4s>

advanced tutorials <http://catlikecoding.com/unity/tutorials/>
