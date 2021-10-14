# Kinect-RT-Avatar-Animation
A Computer Graphics and 3D Project: acquire depth sequences with a Kinect camera, and use skeleton joints to animate a 3D avatar – Using together 3D acquisition, skeleton, and computer graphics for visualization and rendering.

## Dependencies
With Visual Studio:
* Open the solution (.slt file);
* Create a new folder (in the project scope) named "external libraries";
* Install all the required libraries in "external libraries".


### Mainly Required Libraries
* Kinect.h
* glew.h
* glfw3.h
* OpenGL Math Libraries

## How to Use:
The main.cpp file is used only to initialize and execute istances of OpenGLApp.h; this class is the core of our program.\
OpenGLApp generates a window and renders the scene; if you want change the scene or the type of rendered avatar you must chage the OpenGLApp::Run method.
