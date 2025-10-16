#include <stb_image_write.h>
#include <stb_image.h>

#include "TerrainGenerator/TerrainGenerator.h"
#include "TerrainMesh.h"
#include "TerrainRender/TerrainRenderer.h"

#include <camera.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>

//for testing
#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int generateFrameBuffer(unsigned int& fbo, unsigned int& color_buffer, bool sampling);
unsigned int generateScreenQuad(unsigned int& VAO);
static float getRandom(float min, float max);
void toggle_mode(GLFWwindow* window, int key, bool& toSwitch);
int handleToggle(bool wireframe, bool fog, bool atmos);

//setings
const unsigned int screenWidth = 1440;
const unsigned int screenHeight = 900;

//delta-time
float deltaTime{ 0.0f }; //time between curr and last frame
float lastFrame{ 0.0f }; //time of last frame

//mouse variables
float lastX = screenWidth / 2;
float lastY = screenHeight / 2;

float pitch{ 0.0f };
float yaw{ -90.0f };

bool firstMouse{ true };
bool toggleWireframe{ false };
bool toggleAtmosphere{ false };
bool toggleFog{ false };

constexpr bool write_to_file{ true };
constexpr bool load_from_image{ false };

//camera setup
Camera camera(glm::vec3(0.0f, 100.0f, 100.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), yaw, pitch);

Perlin perlin(1000);

float quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

int main()
{
	//initiating a window using GLFW
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//taking mouse inputs
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	gladLoadGL();

	glViewport(0, 0, screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	Shader objectShader(
		"C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Shaders\\object.vert",
		"C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Shaders\\object.frag");
	Shader screenShader(
		"C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Shaders\\post_process.vert",
		"C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Shaders\\post_process.frag");


	//GL_STATES
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//FBO
	unsigned int fbo;
	unsigned int fb_color_buffer;
	generateFrameBuffer(fbo, fb_color_buffer, false);

	//quad VAO
	unsigned int screenVAO;
	generateScreenQuad(screenVAO);

	//nice settigns: (16 octaves is nice)
	// 1024, p0.45, s0.0035, o16, scale 512, shift 256 white mountains
	//use dw with low yScale and persistence

	float yScale = 512.0f, yShift = 256.0f; //range from -256 to 256
	float seaLevel = -512.0f;

	auto start = std::chrono::high_resolution_clock::now();
	TerrainMesh tMesh(yScale, yShift, seaLevel);
	TerrainGenerator tGen(1000);

	if (load_from_image)
		tMesh.map_dimensions = tGen.loadHeightmap("textures\iceland_heightmap.png", tMesh.data);
	else
	{
		int map_size = 2048;
		int mapSize_x = map_size, mapSize_z = map_size; //only squares, rectangles cause strips
		double persistence = 0.45f, scale = 8.0f; //keep scale v small
		int octaves = 4;

		//tMesh.map_dimensions = tGen.generateHeightmap(mapSize_x, mapSize_z, persistence, scale, octaves, FBM, write_to_file, tMesh.data);
		tMesh.map_dimensions = { mapSize_x, mapSize_z} ;
		tMesh.heightMap_texture = tGen.generateHeightmapComp(mapSize_x, mapSize_z, persistence, scale, octaves);
	}

	//initialise Terrain Renderer after data has been generated
	TerrainRenderer tRen(objectShader, tMesh);

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Setup time: " << duration.count() << "ms\n";

	////setting constant uniforms
	glm::vec3 sun_color = glm::vec3(0.98, 0.85, 0.65);
	glm::vec3 sun_dir = glm::vec3(-0.2, -1.0f, -0.3f);

	//render loop
	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		//process the previous frame's inputs
		processInput(window);

		//first pass
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		objectShader.use();

		glm::vec3 bgCol = glm::vec3(1.0f);
		glClearColor(bgCol.x, bgCol.y, bgCol.z, 1.0);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		tRen.RenderTerrain(tMesh, sun_color, sun_dir, camera, screenWidth, screenHeight, handleToggle(toggleAtmosphere, toggleFog, toggleWireframe));

		//second pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT);
		screenShader.use();
		glBindVertexArray(screenVAO);
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, fb_color_buffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//swap buffers [front and back] and polls IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//deletes all the GLFW resources that were allocated
	glDeleteFramebuffers(1, &fbo);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

int handleToggle(bool wireframe, bool fog, bool atmos) {
	std::string res;

	res.push_back(wireframe ? '1' : '0');
	res.push_back(fog ? '1' : '0');
	res.push_back(atmos ? '1' : '0');

	return std::stoi(res, nullptr, 2);
}

unsigned int generateScreenQuad(unsigned int& VAO) {

	unsigned int VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

unsigned int generateFrameBuffer(unsigned int& fbo, unsigned int& color_buffer, bool sampling) {

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//texture attachment
	glGenTextures(1, &color_buffer);
	glBindTexture(GL_TEXTURE_2D, color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);

	if (sampling) {
		//depth and stencil attachment
		unsigned int depthStencil;
		glGenTextures(1, &depthStencil);
		glBindTexture(GL_TEXTURE_2D, depthStencil);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, screenWidth, screenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);
	}
	else {
		//Renderbuffer object
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER::framebuffer is not complete!";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return fbo;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.processMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processMouseScroll(yoffset);
}

void processInput(GLFWwindow* window)
{
	//static bool tWasDown = false;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	toggle_mode(window, GLFW_KEY_T, toggleWireframe);
	toggle_mode(window, GLFW_KEY_F, toggleFog);
	toggle_mode(window, GLFW_KEY_G, toggleAtmosphere);

	//camera controls
	const float cameraSpeed = 5.0f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		//cameraPos += cameraSpeed * cameraFront;
		camera.processKeyboard(FORWARD, cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		//cameraPos -= cameraSpeed * cameraFront;
		camera.processKeyboard(BACKWARD, cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		//cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront ,cameraUp));
		camera.processKeyboard(RIGHT, cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		//cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
		camera.processKeyboard(LEFT, cameraSpeed);
}

void toggle_mode(GLFWwindow* window, int key, bool& toSwitch) {

	//works only for alphabet keys
	static std::vector<bool> keys(26, false);
	int ind = key - GLFW_KEY_A;

	if (glfwGetKey(window, key) == GLFW_PRESS)
	{
		if (!keys[ind]) {
			keys[ind] = true;
			toSwitch = !toSwitch;
		}
	}
	else keys[ind] = false;
}

static float getRandom(float min, float max)
{
	if (min > max)
		return max;
	float num = rand();
	while (num < min || num > max)
		num = rand();

	return num;
}
