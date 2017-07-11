various
--------------------

more questions and ideas that have come up.

fader shader
--

here is a useful shader to have installed on the main camera. it can smooth your side borders as well as mask your screen to look like a circle in the middle. all edges have size settings so the mask is sharp or smooth.

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
    mat= new Material(Shader.Find("MyShaders/fadeshader"));  //should match name at the top in .shader file
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

now in the Main Camera inspector you should see some parameters to control. play around with them. 'Fade' works like a main fader. 0.0 is all black and 1.0 is normal. you can also go beyond 1.0 to brighten up the graphics. 'PosX' and 'PosY' are 0.0 to 1.0 positions of where the edges start, 'SizeX' and 'SizeY' are how wide or long the edge section should be. 'Rad' determines the radius of the circle (set this to >= 0.707 if not used) and 'RadSize' is again how much of a smooth edge there should be.

![mask](00mask.png?raw=true "mask")
