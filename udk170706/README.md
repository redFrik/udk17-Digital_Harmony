various
--------------------

various questions and ideas that have come up.

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

these instructions are ***mac osx*** only. for windows there is <http://spout.zeal.co>

below demonstrates how to send your unity graphics to a [syhpon](http://syphon.v002.info) client (i.e. to another application on your own mac or on another computer).

note: here we create a new scene but you can also use an old project.

* start unity and create a new 3D project
* select GameObject / Particle System
* download 'the plugin package' from [here](https://github.com/keijiro/Funnel/tree/gllegacy#setting-up)
* doubleclick the file 'Funnel.unitypackage' to import it into unity
* in unity find the Funnel C# script and drag&drop it onto the main camera
* when you now run your screen should go black and you should see the text 'No cameras rendering'
* select 'Edit / Project Settings / Player' and tick 'Run In Background'
* optionally: in Main Camera inspector set screen width&height to 1920x1080 and Render Mode to 'Preview On Game View' 

![syphon](00syphon.png?raw=true "syphon")

* go to <https://github.com/Syphon/Simple/releases/tag/version-3> and download Syphon Simple Apps 3
* start 'Simple Client.app' and connect to your unity syphon server

![syphon2](01syphon2.png?raw=true "syphon2")

there are clients for processing, max, isadora etc. see <http://syphon.v002.info>

and to send over network try <http://techlife.sg/TCPSyphon/>

notes on optimisation: try to keep the resolution down, don't send more pixels than your video projector is using at the end of the chain, turn off preview if you can, use ethernet cables instead of wifi.
