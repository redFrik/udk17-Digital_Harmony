shaders
--------------------

more will come here


leap
--

here are instructions on how to use the [leap motion controller](http://leapmotion.com) with unity.

* go to <https://github.com/leapmotion/LeapMotionCoreAssets> and download the library as zip
* unpack the zip file and find the three folders: `LeapMotion`, `LeapMotionVR` and `Plugins` (they are all inside the Assets folder)
* drag and drop these three into a new empty unity 3d project
* find the demo `FlowerAndRobot` under 'LeapMotion / Scenes', doubleclick it and run

the demo should work and you should be able to interact with the flower.

to use the controller inside an empty scene do the following...

* create a new scene and add the `HandController` prefab and `HandControllerSandBox`
* add a `Cube`
* select the Cube and click `Add Compoment` and then select `Physics / Rigidbody`
* select the HandController and set the position to `X= 0`, `Y= -2`, `Z= -2` and scale to `X= 10`, `Y= 10`, `Z= 1`

links
--

<https://docs.unity3d.com/Manual/SL-VertexFragmentShaderExamples.html>

<http://www.alanzucconi.com/2015/06/10/a-gentle-introduction-to-shaders-in-unity3d/>
