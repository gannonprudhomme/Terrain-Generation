# Terrain-Generation
A terrain generator written in C++ that uses Perlin noise([libnoise](http://libnoise.sourceforge.net/index.html)) to output a height map, and OpenGL to render it.
- Self-contained heightmap generation
- Specular lighting
- Bump mapping

## Requires
- GLFW for window creation
- GLEW for using OpenGL
- GLM for math utilities
- libnoise for Perlin noise / terrain generation
- [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) for loading image files into a texture

## How it works
1. Create heightmap with libnoise and export it to an image file
2. Vertices are filled with simple x and z coordinates(no height)
3. OpenGL is set up, vertices are sent to vertex-shader through draw call.
4. The vertex shader updates each vertex with the according height by reading it from the height map image file.
5. It then calculates the normal values by looking up the heights for the surrounding vertices and creates a normal vector, which is sent to the fragment shader.
6. Fragment shader calculates light value using normal map and light location, blends the resulting color with the terrain texture, then outputs the final color.


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

