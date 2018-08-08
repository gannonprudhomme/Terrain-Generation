# Terrain-Generation
Simple terrain generator that uses Perlin noise(through libnoise) 

# How it works
1. createHeightMap is called, creates an image file
2. Vertices are filled with simple x and z coordinates(no height)
3. The vertex shader reads the height map and updates the correct height. Also uses bump mapping to calculate the surface normals for lighting(not working completely)

# Controls
- W-A-S-D for movement
- Shift to increase movement speed
- F to show wireframes
- G to hide wireframes
- Escape to close the program

# Add
 - Restrictions/Limitations
    - Stuff
 - Different types of noise
 - Controls
