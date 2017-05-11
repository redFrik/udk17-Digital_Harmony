shaders
--------------------

today we will explore shaders. shaders can among other things be used as post processing effects.

basically shaders are small programs that run in parallel on the gpu (graphics card). they are super efficient for things like blurring, distortion, displacement, masking etc but can also be used to generate patterns and other graphics from scratch.

fullscreen shader
--

* create a new unity project
* add some cubes or other objects
* select the Main Camera and click 'Add Component' in the inspector
* select 'New Script', make sure language is set to Javascript and give it a name (here 'post')
* doubleclick the script icon to open it in MonoDevelop
* replace what is there with the code below

```javascript
#pragma strict

var mat: Material;

function OnRenderImage(src: RenderTexture, dest: RenderTexture) {
    Graphics.Blit(src, dest, mat);
}
```

* press run to test

![black](01black.png?raw=true "black")

your 'game' should now be completely black. this is because we did not set the material `mat` in the code above. you should also have gotten the error "ArgumentException: material is null".

the javascript code above uses [Blit](https://docs.unity3d.com/ScriptReference/Graphics.Blit.html) to make a copy of what the main camera see and then applies a *master-out* effect. so whatever we do in our effect (the material/shader mat) will be applied to the whole screen.

* in the Assets menu, select 'Create / Shader / Image Effect Shader' 
* optionally give it a name (here 'postshader')
* doubleclick the shader icon to open it in MonoDevelop
* in the first line change `"Hidden/NewImageEffectShader"` to `"MyShaders/FirstExperiments"`
* save and switch back to unity
* in the Assets menu, select 'Create / Material' 
* optionally give it a name (here 'postmaterial')
* in the material's inspector set 'Shader' to 'MyShaders/FirstExperiments'

![material](02material.png?raw=true "material")

* last select the Main Camera and drag&drop your material to 'Mat'
* press run and it should look like this...

![mat](03mat.png?raw=true "mat")

now we are set up and ready to experiment with the shader code.

* doubleclick to open the shader (here 'postshader') in MonoDevelop
* change the line near the bottom `col = 1 - col;` to `col = (sin(_Time[1]*10)*0.5) + col;`
* in unity click run

the line `col = 1 - col` was responsible for inverting all the colours. we replaced it with a sin function using the global time (in seconds) to get the pulsating fullscreen colour effect.
bascially this line is running for each pixel in the whole screen and add or subtract a bit to the pixel's colour.
in a 1920x1080 window this means that this shader program will run 2073600 times each frame. but because the graphic card in your computer do all these calculations (in parallel) we will not notice any dropped frames or higher cpu load.

* find and tick the small 'maximize on play' option
* click stop and run again and you should be seeing your scene in a bigger window
* keep this running in background while you experiment further with the code
* switch back to MonoDevelop and play with the code in the col line - try changing values, save and switch back to unity
* if your shader code has any error your screen will turn pink

here are some lines to try...

```
col = col.r;  //greyscale - only use the red component
col = sin(_Time[1]*3)*0.5+0.5;  //overwrite everything with 0.0 to 1.0
col = col%(col*cos(_Time[1]*2)*0.5+0.5);  //weird modulor effect
col = _Time[1]/50%col;  //more modulor
col = tan(col);  //simple colour adjustment
col = tan(col+sin(_Time[1]*0.1));  //night and day
```

frag
--

so far we only used the fragment shader program to change the colours of all the pixels in the same manner. we will get much more interesting results if we start change pixels according to their position. with the `i.uv` argument passed into the fragment shader we can get the pixel's normalized screen coordinates.

```
col = i.uv.r;  //simple gradiant - overwrite col with pixel x postion (0-1)
col = i.uv.y;  //same but for y
col = col*i.uv.x;  //scale original pixel colour with pixel x position (0-1)
col = col*sin(i.uv.x*30);  //fade in out many times
col = col*(sin(i.uv.x*40)+cos(i.uv.y*30))*0.5;  //holes
col = col*sin(i.uv.x*3.1415)*sin(i.uv.y*3.1415);  //black mask
```

and to construct a new colour you can use a float4...

```
col = float4(col.r, col.g, col.b, 1.0);  //no change
col = float4(col.r, 0, col.b, 1.0);  //set green to 0 in all pixels
col = float4(col.r, i.uv.x, col.b, 1.0);  //green goes from 0 to 1 depending on x position
col = float4(sin(_Time[1]*20+i.uv.x), cos(_Time[1]*8+i.uv.y), col.b, 1.0);  //usch
```


let us make some simple patterns ignoring the original 'col'...

```
col = sin(i.uv.x*100+(_Time[1]*10));  //scrolling bars
col = sin(i.uv.x*100+(_Time[1]*10))*sin(i.uv.y*30*(_Time[1]*8));
col = float4(sin(i.uv.x*2), sin(i.uv.x*3), sin(i.uv.y*4), 1.0);
col = float4(sin(i.uv.x*20+(_Time[1]*2)), sin(i.uv.y*3+(_Time[1]*3)), _Time[1]*4%(i.uv.x*2), 1.0);
```

a more common and useful way to mess with the pixels is to change the offset of the position.  so first set back the 'col' line to `col = col;` so that we see the original scene again.
then change line above to...

```
fixed4 col = tex2D(_MainTex, i.uv+float2(sin(_Time[1])/3, 0.2));
```

this will shift all positions by adding a float2. we get a panning effect because the code add a value between -0.3 and 0.3 to all x positions. let us explore more...

```
fixed4 col = tex2D(_MainTex, i.uv+float2(sin(_Time[1])/3, sin(_Time[1]*8)/3));
fixed4 col = tex2D(_MainTex, i.uv+float2(sin(i.uv.x*10), 0));
fixed4 col = tex2D(_MainTex, i.uv+float2(sin(i.uv.x*30+_Time[1]), sin(i.uv.y*20+_Time[1])));
fixed4 col = tex2D(_MainTex, i.uv*float2(sin(i.uv.x*3.1415), sin(i.uv.y*3.1415)));
fixed4 col = tex2D(_MainTex, i.uv%float2(sin(i.uv.x*3.1415+_Time[1]), sin(i.uv.y*3.1415)));
fixed4 col = tex2D(_MainTex, i.uv+float2(sin(i.uv.x*50+_Time[1])*0.3, sin(i.uv.y*6+_Time[1])*0.1));
fixed4 col = tex2D(_MainTex, i.uv*float2(sin(i.uv.y*5+_Time[1]), cos(i.uv.x*6+_Time[1])));
fixed4 col = tex2D(_MainTex, i.uv*float2(sin(i.uv.y), cos(i.uv.x)));
fixed4 col = tex2D(_MainTex, i.uv*float2(i.uv.x*20%1, i.uv.y*10%1));  //mirror effects
```

this is how the pass-through fragment shader program look if you want to go back to normal...
```
fixed4 frag (v2f i) : SV_Target
{
    fixed4 col = tex2D(_MainTex, i.uv);
    col = col;
    return col;
}
```


vert
--

now let us try also manipulating the vertices. this is another program that run before the fragment program that we played with above.
think of vertices as all 'corner positions' defining all shapes in the scene.  there are a lot fewer vertices as pixels (fragments) in a scene.

find the vertex program in the same shader file...
```
v2f vert (appdata v)
{
    v2f o;
    o.vertex = UnityObjectToClipPos(v.vertex);
    o.uv = v.uv;
    return o;
}
```

if we invert all the values the picture will be flipped left/right upside down...

```
o.uv = 1-v.uv;
```

again we can build up from scratch using a float2...

```
o.uv = float2(v.vertex.x, v.vertex.y+(sin(_Time[1]*4)*0.2));
o.uv = float2(v.vertex.x+(cos(_Time[1]*4)*0.1), v.vertex.y+(sin(_Time[1]*4)*0.2));
o.uv = float2(v.vertex.y*(_Time[1]%3), v.vertex.x+(sin(_Time[1]*40)*0.5));
```

etc.

opensoundcontrol
--

```supercollider
n= NetAddr("127.0.0.1", 57120);
```
