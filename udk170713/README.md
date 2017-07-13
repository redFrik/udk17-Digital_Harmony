various
--------------------

more questions and ideas that have come up.

fader shader
--

here is a useful shader to have installed on the main camera. it can smooth your side borders as well as mask your screen to look like a circle in the middle. all edges have size settings so the mask can be either sharp or smooth.

* add the following javascript to your Main Camera 

```javascript
#pragma strict

private var mat: Material;
public var fade: float = 1.0;
public var posx: float = 0.05;
public var posy: float = 0.05;
public var sizex: float = 0.1;
public var sizey: float = 0.1;
public var rad: float = 0.707;
public var radsize: float = 0.1;

function Start() {
    mat= new Material(Shader.Find("MyShaders/fadeshader"));  //should match name at the top in the .shader file
}
function OnRenderImage(src: RenderTexture, dest: RenderTexture) {
    mat.SetFloat("_Fade", fade);
    mat.SetFloat("_PosX", posx);
    mat.SetFloat("_PosY", posy);
    mat.SetFloat("_SizeX", Mathf.Max(sizex, 0.0));
    mat.SetFloat("_SizeY", Mathf.Max(sizey, 0.0));
    mat.SetFloat("_Rad", rad);
    mat.SetFloat("_RadSize", radsize);
    Graphics.Blit(src, dest, mat);
}
```

* select Assets / Create / Shader / Image Effect Shader
* optionally give it a name (here 'fadeshader')
* doubleclick the shader icon to open it in MonoDevelop
* replace what is there with the code below

```
Shader "MyShaders/fadeshader" {
    Properties {
        _MainTex ("Texture", 2D) = "white" {}
        _Fade ("Fade", float) = 1.0
        _PosX ("PosX", float) = 0.05
        _PosY ("PosY", float) = 0.05
        _SizeX ("SizeX", float) = 0.1
        _SizeY ("SizeY", float) = 0.1
        _Rad ("Rad", float) = 0.1
        _RadSize ("RadSize", float) = 0.1
    }
    SubShader {
        Cull Off ZWrite Off ZTest Always
        Pass {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #include "UnityCG.cginc"
            struct appdata {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };
            struct v2f {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };
            v2f vert (appdata v) {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }
            sampler2D _MainTex;
            float _Fade, _PosX, _PosY, _SizeX, _SizeY, _Rad, _RadSize;
            fixed4 frag (v2f i) : SV_Target {
                fixed4 col = tex2D(_MainTex, i.uv);
                float fadex1= smoothstep(_PosX*0.5, _PosX*0.5+_SizeX, i.uv.x);
                float fadex2= smoothstep(_PosX*0.5, _PosX*0.5+_SizeX, 1-i.uv.x);
                float fadey1= smoothstep(_PosY*0.5, _PosY*0.5+_SizeY, i.uv.y);
                float fadey2= smoothstep(_PosY*0.5, _PosY*0.5+_SizeY, 1-i.uv.y);
                float faderad= 1-smoothstep(_Rad, _Rad+_RadSize, distance(float2(0.5, 0.5), i.uv));
                return col*_Fade*fadex1*fadex2*fadey1*fadey2*faderad;
            }
            ENDCG
        }
    }
}
```

now in the Main Camera inspector you should see some parameters to control. play around with them. 'Fade' works like a main fader. 0.0 is all black and 1.0 is normal. you can also go beyond 1.0 to brighten up the graphics. 'Posx' and 'Posy' are 0.0 to 1.0 positions of where the edges start, 'Sizex' and 'Sizey' are how wide or long the edge section should be. 'Rad' determines the radius of the circle (set this to >= 0.707 if not used) and 'Radsize' is again how much of a smooth edge there should be.

![mask](00mask.png?raw=true "mask")

this script can also be used as a main fader for your live visuals. by setting posx, posy, sizex and sizey to 0.0 and then only controlling the fade parameter, you can bring in and out your graphics to/from black in a nice way. perhaps set up midi or osc receiver code to control this fade parameter from a midi controller or from your supercollider sound patch (by adapting the code [here](https://github.com/redFrik/udk16-Immersive_Technologies/tree/master/udk170112#midi-input) or [here](https://github.com/redFrik/udk17-Digital_Harmony/tree/master/udk170511#sc-to-unity))

fullscreen
--

connect a video projector, turn off screen mirroring and set its resolution to 1920x1080 (= 1080p). on mac osx you do all this in System Preferences / Displays. if projector or monitor does not have 1920x1080, try to figure out its native resolution by reading the manual and then adapt the numbers in the script below.

* use an existing project or create a new unity project (add at least a cube to see something)
* select the Main Camera, 'Add Component' and then 'New Script'
* give the script a name (here 'display2') and make sure language is JavaScript
* doubleclick the script symbol and paste in the code here below

```javascript
#pragma strict

function Start() {
    var beamerwidth= 1920;  //edit to match video projector width
    var beamerheight= 1080;	//edit to match video projector height
    Debug.Log(Screen.currentResolution);
    var laptopwidth= Screen.currentResolution.width;
    Screen.SetResolution(laptopwidth+beamerwidth, beamerheight, false);
    if(Display.displays.Length>1) {
        Display.displays[1].Activate();
        Display.displays[1].SetParams(beamerwidth, beamerheight, laptopwidth, 0);
    }
}
```

* save and close
* back in unity select File / Build Settings...
* click 'Player Settings...'
* set 'Display Resolution Dialog' to 'Disabled'
* deselect 'Default Is Full Screen'
* enter 1920 and 1080 for default screen width and height
* click 'Build And Run', give your application a name and save

![playersettings](01playersettings.png?raw=true "playersettings")

NOTE: at least on mac osx this will now only work every second time you run the app (weird bug). to get around that open Terminal and type `/Users/asdf/unity/2nd_screen/2nd.app/Contents/MacOS/2nd -screen-height 900 -screen-width 1440` but adapt the path and app name to match your system (i.e. where you saved your app when you built it). quit with ctrl+c

NOTE: all the above is kind of a hack and temporary solution in current unity (5.6). hopefully in the future unity will have better multi-display support. see discussion [here](https://stackoverflow.com/questions/43066541/unity-multiple-displays-not-working)

syphon multi display
--

NOTE: this section is for ***mac osx*** only. for windows there is a similar texture sending application here... <http://spout.zeal.co> that should be able to do something similar.

because unity have big problems with multi-display setups, one can use syphon and some simple max patches (or openframeworks, madmapper, processing or whatever) to deal with the window management.

in this directory you will find four max patches that help run your unity scenes with 2-3 projectors. if you don't have max they should run in the demo version of [max](http://cycling74.com/downloads). you will need to install the Syphon package in max's package manager.

syphonmulti2 and syphonmulti3 takes two or three syphon inputs and display them on as many projectors/displays. so with this you can for example have three cameras looking in your scene, project them next to each other and get kind of three separate views of the same object. to try it out install the Funnel package (see [here](https://github.com/redFrik/udk17-Digital_Harmony/tree/master/udk170706#syphon)) and then add the Funnel C# script to all three cameras. set the resolution to 1920x1080, and importantly tick run in background. run and then open the max patch (syphonmulti2 for two cameras + two projectors, syphonmulti3 for three). click start and select a different syphone server (unity camera) for each syphon client. last click set up windows.

syphonwide2 and syphonwide3 takes a single syphon input and spreads it over two or three projectors. so with this you can for example have a unity camera/scene that fills a whole room, projecting on three walls. to try it out install the Funnel package (see [here](https://github.com/redFrik/udk17-Digital_Harmony/tree/master/udk170706#syphon)) and then add the Funnel C# script to all three cameras. for two projectors set the funnel resolution to 3840x1080 and for three projectors use 5760x1080 (so we will be sending very wide textures from unity). next tick run in background, open the max patch (syphonwide2 for two projectors, syphonwide3 for three). click start and select a different syphone server (unity camera) for each syphon client. last click set up windows.

NOTE: the above might be running slow on your computer (specially the multi camera patches). watch the framerate and set the funnel script to 'Send Only'. it's not an optimal solution but might work in some cases (until unity is updated and the multi display problems are solved).

extra
--

unity youtube tutorials by Board To Bits Games <https://www.youtube.com/channel/UCifiUB82IZ6kCkjNXN8dwsQ/videos>

more on shaders <https://thebookofshaders.com>
