# Project_Template

This is my Repository for COMP3015 - CW2.

# About

The scene focusses on recreating a prison escape scenario.
You take on the control of a prison guard who is manning a drone with a light attachment.
You have to move around the scene to see if you can spot any escapees.
The ideas behind the shaders used came from attempting to recreate a really realistic looking light affect.
It was also inspired by the great escape.

# How to Use

Clone the repo and build the project using Visual Studio.
You can then run the scene and move the camera around as you so desire.

Controls -

WASD - Move Drone Horizontally
Space / Left Shift - Move Drone Vertically
P - Start / Stop animating the Drone to pan around the scene

# How it Works

The program relies on two separate pairs of shaders. The solid shaders and the regular uniform shaders.
The solid shaders handle rendering the frustum. The basic uniform shaders handle every other model rendered
in the scene. The vertex shader "basic_uniform.vert" utilises a shadow matrix for mapping shadows.
This is mainly used in the fragment shader "basic_uniform.frag", which uses ggxDistribution and MicroFacetModel functions
to realistically reflect how light acts in the real world in a process called Physically Based Rendering.
Depending on the material's rough attribute and whether or not its a metal affects how much light bounces off
the object or whether or not it emits specular.

The main render function in "scenebasic_uniform.cpp" runs through two loops to allow for shadow mapping.
This runs once from the light's perspective and culls faces to see how far away objects are from itself. It stores all these
Z values in a FBO (Frame Buffer Object). This is then used on a second render of the scene from the camera's perspective,
which allows each pixel to determine whether its cast in shadow or not based on the frame-buffer.

Controls are implemented through the GLFW library and these commands are found in the "scenerunner.h" file in the "mainLoop" function.
They apply vec3s or floats to the lights position in the scene creating the illusion of movement.

# What I'd do Differently

I wanted to recreate what I had before using texture maps however they had to be removed
due to the complicated nature of these two new lighting techniques.
In order to get GLFW to work with movement I also had to implement virtual functions in the Scene header file.
This felt hacky and I felt as if I was meant to introduce GLFW at a lower level,
So if had more time I would retry this to enable best practice.
