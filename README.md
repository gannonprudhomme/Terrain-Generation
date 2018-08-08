# Terrain-Generation
A terrain generator written in C++ that uses Perlin noise([libnoise](http://libnoise.sourceforge.net/index.html)) to output a height map, and OpenGL to render it.
- Self-contained heightmap generation
- Specular lighting
- Bump mapping for normal maps, calculated within vertex shader from the height map

## Requires
- GLFW for window creation
- GLEW for using OpenGL
- GLM for math utilities
- libnoise for Perlin noise / terrain generation
- [stb_image](for loading image files into a texture)

## Controls
- W-A-S-D for movement
- Shift to increase movement speed
- F to show wireframes
- G to hide wireframes
- Escape to close the program

## Screen Shots
![Imgur](https://i.imgur.com/CgmFHQX.png)

Wireframe mode
![Imgur](https://i.imgur.com/ljAyiwQ.png)

## Add
 - Restrictions/Limitations
    - Stuff
 - Different types of noise
 - Controls

## How it works
1. 
2. Vertices are filled with simple x and z coordinates(no height)
3. The vertex shader reads the height map and updates the correct height. Also uses bump mapping to calculate the surface normals for lighting. 
