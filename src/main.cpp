#if 1
// Originally based on ThinMatrix tutorials, but have divered from it pretty significantly

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "utils/stb_image.h"
#include "framework/camera.h"
#include "framework/shader.h"
#include "framework/texture.h"
#include "framework/light.h"
#include "framework/heightsgenerator.h"
#include "framework/perlinheightsgenerator.h"
#include <noise/noise.h>
#include "utils/noiseutils.h"
#include "utils/imageloader.h"

using namespace noise;

#define SCREEN_WIDTH 1920.0f
#define SCREEN_HEIGHT 1080.0f

#define FACTOR 0.35 // Increase to make flatter
#define AMPLITUDE 400 / FACTOR

void createHeightMap(float noiseWidth, float noiseHeight, float vertWidth, float vertHeight);

float getHeight(int x, int z);
glm::vec3 calculateNormal(int x, int z, int upperBounds);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInputCamera(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
unsigned char* readBMP(char* filename);

Camera camera(glm::vec3(20.0f, 100.0f, 3.0));

float delta = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float mouseLastX = 0.0f;
float mouseLastY = 0.0f;
bool firstMouse = true; // If the mouse has not entered the window yet

bool wireFrame = false;

module::Perlin perlinModule;

utils::NoiseMap heightMap;
utils::NoiseMapBuilderPlane heightMapBuilder;
float heights[50][50];

unsigned char* data;
Image *image;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Terrain-Generation", NULL, NULL);
	if(window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// learnopengl.com uses GLAD, I like GLEW better
	if(glewInit() != GLEW_OK) {
		std::cout << "Error! " << std::endl;
	}

	// Tell OpenGL the size of the rendering window, might not need when using matrices + shaders?
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Tell GLFW that this is the function we want ran when the window changes size
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Tell GLFW that we want the cursor to be disabled
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// So the window doesn't open in an awkward place
	glfwSetWindowPos(window, 0, 0);

	// Tell GLFW that we want the mouse_callback function to called when the cursor pos changes
	glfwSetCursorPosCallback(window, mouse_callback);

	// Tell GLFW that we want scroll_callback to be called when the scroll wheel is acted upon
	glfwSetScrollCallback(window, scroll_callback);

	glEnable(GL_DEPTH_TEST);

	// Create the height map before loading in the texture
	createHeightMap(256, 256, 2, 2);

	Shader *terrainShader = new Shader("src/shaders/terrain.vert", "src/shaders/terrain.frag");
	Texture *texture = new Texture("res/grass.png");
	Texture *heightMap = new Texture("res/heightmap.bmp");
	Texture *normalMap = new Texture("normalmap.png"); // Unused
	Light *light = new Light(glm::vec3(20000, 20000, 20000), glm::vec3(1, 1, 1));

	/********************************/
	//  LOAD TERRAIN MESH/VERTICES
	/*******************************/

	const float MAX_PIXEL_COLOR = 256 * 256 * 256;

	const float SIZE = 10000;
	const float VERTEX_COUNT = 500; // Default 2000

	// Map of [VERTEX_COUNT, VERTEX_COUNT]
	std::vector<float> vertices(VERTEX_COUNT * VERTEX_COUNT * 3);
	std::vector<float> textureCoords(VERTEX_COUNT * VERTEX_COUNT * 2);
	std::vector<unsigned int> indices(6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1));

	// What is happening here
	int vertexPointer = 0;
	for(float i = 0; i < VERTEX_COUNT; i++) { // z
		for(float j = 0; j < VERTEX_COUNT; j++) { // x
			//vertices[vertexPointer * 3] = j * 2;
			vertices[vertexPointer * 3] = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
			vertices[vertexPointer * 3 + 1] = 0;
			vertices[vertexPointer * 3 + 2] = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;
			//vertices[vertexPointer * 3 + 2] = i * 2;

			textureCoords[vertexPointer * 2] = (float)j / ((float)VERTEX_COUNT - 1);
			textureCoords[vertexPointer * 2 + 1] = (float)i / ((float)VERTEX_COUNT - 1);
			vertexPointer++;
		}
	}

	// What is happening heredio
	int pointer = 0;
	for(int gz = 0; gz < VERTEX_COUNT - 1; gz++) {
		for(int gx = 0; gx < VERTEX_COUNT - 1; gx++) {
			int topLeft = (gz * VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = bottomRight;
		}
	}

	unsigned int vao, pVBO, nVBO, tcVBO, ibo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &pVBO);
	glGenBuffers(1, &nVBO);
	glGenBuffers(1, &tcVBO);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Positions
	glBindBuffer(GL_ARRAY_BUFFER, pVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Tex coords
	glBindBuffer(GL_ARRAY_BUFFER, tcVBO);
	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(float), textureCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	terrainShader->use();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightMap->getID());
	terrainShader->setTexture("heightMap", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, normalMap->getID());
	terrainShader->setTexture("normalMap", 2);

	terrainShader->setFloat("AMPLITUDE", AMPLITUDE);

	glm::mat4 projection;
	
	// Render loop
	while(!glfwWindowShouldClose(window)) {
		// Per frame time logic
		float currentFrame = glfwGetTime();
		delta = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check for inputs, etc
		processInputCamera(window);

		// Rendering here
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // State-Setting function
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // State-using function

		glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100000.0f);
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0, 0, 0));

		// Set up Shader
		terrainShader->use();
		terrainShader->setMat4("projection", projection);
		terrainShader->setMat4("view", view);
		terrainShader->setMat4("model", model);
		terrainShader->setVec3("lightPosition", light->position);
		terrainShader->setVec3("lightColor", light->color);
		terrainShader->setFloat("shineDamper", 1);
		terrainShader->setFloat("reflectivity", 0);

		// Bind the mountain/grass texture
		terrainShader->setInt("tex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->getID());

		// Draw the vertices/indices (send them to the graphics card to be processed)
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		// Checks if any events are triggered(like keyboard input or mouse movement events), 
		// updates window states, calls corresponding functions
		glfwPollEvents();

		// Swaps the color buffer that has been used to draw in during this iteration and show it as output to the screen
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

// Pretty sure I got this from libutils
void createHeightMap(float noiseWidth, float noiseHeight, float vertWidth, float vertHeight) {
	// Produces 3D ridged multifractal noise, similar to mountains
	module::RidgedMulti baseMountainTerrain;
	module::Voronoi plateauTerrain;

#if 1
	// Generates "Billowy" noise suitable for clouds and rocks?
	module::Billow baseFlatTerrain;
	baseFlatTerrain.SetFrequency(4.0);
#endif

#if 0
	module::Spheres baseFlatTerrain;
	baseFlatTerrain.SetFrequency(2.0);

#endif

#if 0
	// Produces polygon-like formations
	module::Voronoi baseFlatTerrain;
	baseFlatTerrain.SetFrequency(2.0);
	baseFlatTerrain.SetDisplacement(0.25);
#endif

	// Applies a scaling factor to the output value from the source module
	// Scales the flat terrain, adds noise to it
	module::ScaleBias flatTerrain;
	flatTerrain.SetSourceModule(0, baseFlatTerrain);
	flatTerrain.SetScale(0.125); // Default is 1
	flatTerrain.SetBias(-0.75); // Default is 0

	// Scales the mountain terrain
	// Can use if needed, but not right now
	/*
	module::ScaleBias mountainTerrain;
	mountainTerrain.SetSourceModule(0, baseMountainTerrain);
	mountainTerrain.SetScale(0.75);
	mountainTerrain.SetBias(-0.75); */

	module::Perlin terrainType;

	module::Select terrainSelector;
	terrainSelector.SetSourceModule(0, flatTerrain);
	terrainSelector.SetSourceModule(1, baseMountainTerrain);
	terrainSelector.SetControlModule(terrainType);
	terrainSelector.SetBounds(0.0, 1000); //1000
	terrainSelector.SetEdgeFalloff(0.125); // .125

	// pseudo-random displacement of the input value
	module::Turbulence finalTerrain;
	finalTerrain.SetSourceModule(0, terrainSelector);
	finalTerrain.SetFrequency(4.0); // How rapidly the displacement changes
	finalTerrain.SetPower(0.125); // The scaling factor that is applied to the displacement amount

	// Output the noise map
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(finalTerrain);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(noiseWidth, noiseHeight);
	heightMapBuilder.SetBounds(0, vertWidth, 0, vertHeight);
	heightMapBuilder.Build();

	utils::RendererImage renderer;
	utils::Image image;
	renderer.SetSourceNoiseMap(heightMap);
	renderer.SetDestImage(image);
	renderer.Render();

	utils::WriterBMP writer;
	writer.SetSourceImage(image);
	writer.SetDestFilename("res/heightmap.bmp");
	writer.WriteDestFile();
}

// MARK: 
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInputCamera(GLFWwindow* window) {
	float cameraSpeed = 400.0f;
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, delta);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, delta);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, delta);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, delta);
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.processKeyboard(UP, delta);
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.processKeyboard(DOWN, delta);

	if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.movementSpeed = cameraSpeed * 4;
	if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		camera.movementSpeed = cameraSpeed;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if(firstMouse) {
		mouseLastX = xpos;
		mouseLastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - mouseLastX;
	float yOffset = mouseLastY - ypos; // Reversed b/c y-coordinates range from bottom to top

	mouseLastX = xpos;
	mouseLastY = ypos;

	camera.processMouseMovement(xOffset, yOffset);
	//std::cout << xOffset << " " << yOffset << std::endl;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.processMouseScroll(yoffset);
}
#endif