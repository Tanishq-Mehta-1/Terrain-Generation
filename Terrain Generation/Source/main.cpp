#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <cstdio>

#include <camera.h>
#include <vector>

#include <GLFW/glfw3.h>

#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <iostream>
#include "perlin.h"
#include <fstream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int generateFrameBuffer(unsigned int& fbo, unsigned int& color_buffer, bool sampling);
unsigned int generateScreenQuad(unsigned int& VAO);
static float getRandom(float min, float max);

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

//camera setup
Camera camera(glm::vec3(0.0f, 100.0f, 100.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), yaw, pitch);

PerlinGen perlin(1000);

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

	//loading heightMap
	/*int width, height, nChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("textures/iceland_heightmap.png", &width, &height, &nChannels, 0);

	std::vector<float> vertices;
	float yScale = 64.0f / 256.0f, yShift = 16.0f;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			unsigned char* texel = data + (j + width * i) * nChannels;
			unsigned char y = texel[0];

			vertices.push_back(-height / 2.0f + i);
			vertices.push_back((int)y * yScale - yShift);
			vertices.push_back(-width / 2.0f + j);
		}
	}*/
	int mapSize_x = 1024, mapSize_z = 1024; //only squares, rectangles cause strips
	double persistence = 0.5, scale = 0.004; //keep scale v small
	int octaves = 16;
	std::vector<float> data(mapSize_x * mapSize_z, 0);
	std::vector<unsigned char> image(mapSize_x * mapSize_z);

	for (int i = 0; i < mapSize_x; i++) {
		for (int j = 0; j < mapSize_z; j++) {

			double x = i * scale;
			double y = j * scale;

			//Island Generation
			//double n = perlin.octavePerlin(x * scale, y * scale, octaves, persistence);

			//float nx = (i - mapSize_x / 2.0f) / (mapSize_x / 2.0f);  // -1 → 1
			//float ny = (j - mapSize_z / 2.0f) / (mapSize_z / 2.0f);  // -1 → 1
			//float dist = sqrt(nx * nx + ny * ny);   
			//
			//float falloff = pow(dist, 3.0f);  // try exponents 2–4
			//float height = n - falloff;
			//data[i * mapSize_z + j] = std::max(0.0f, std::min(1.0f, height));

			data[i * mapSize_z + j] = perlin.octavePerlin(x, y, octaves, persistence); //output is between 0,1
			image[i * mapSize_z + j] = static_cast<unsigned char>(data[i * mapSize_z + j] * 255.0f);
		}
	}

	//write to png
	std::string size = std::to_string(mapSize_x) + 'x' + std::to_string(mapSize_z);
	std::string p = std::to_string(persistence);
	std::string s = std::to_string(scale);
	std::string name = "Media/Generated/perlin_" + size + "_p" + p + "_s" + s + '_' + ".png";
	stbi_write_png(name.c_str(), mapSize_x, mapSize_z, 1, image.data(), mapSize_x);

	//TODO: if the file is already generated, just look up the texture and load

	std::vector<float> vertices;
	std::ofstream out("normal.txt");

	float yScale = 256.0f, yShift = 128.0f; //range from -256 to 256
	float seaLevel = -25.0f;

	for (int i = 0; i < mapSize_x; i++) {
		for (int j = 0; j < mapSize_z; j++) {

			double y = data[i * mapSize_z + j];
			float height = y * yScale - yShift;
			if (height < seaLevel) height = seaLevel;

			vertices.push_back(-mapSize_x / 2.0f + i);
			vertices.push_back(height);
			vertices.push_back(-mapSize_z / 2.0f + j);

			//calculate normals
			float hL, hR, hU, hD;
			hL = hR = hD = hU = height;

			if (i != 0)
				hL = data[(i - 1) * mapSize_z + j] * yScale - yShift; // left
			if (i != mapSize_x - 1)
				hR = data[(i + 1) * mapSize_z + j] * yScale - yShift; // right
			if (j != 0)
				hD = data[i * mapSize_z + j - 1] * yScale - yShift; // down
			if (j != mapSize_z - 1)
				hU = data[i * mapSize_z + j + 1] * yScale - yShift; // up

			glm::vec3 normal = glm::normalize(glm::vec3((hL - hR) / 2.0f, 1.0f, (hD - hU) / 2.0f));
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
		}
	}

	std::cout << "Loaded " << vertices.size() << " vertices" << std::endl;

	//stbi_image_free(data);
	std::vector<unsigned int> indices;
	for (int i = 0; i < mapSize_z - 1; i++) {
		for (int j = 0; j < mapSize_x; j++) {
				indices.push_back(i * mapSize_z + j);
				indices.push_back(j + mapSize_z * (i + 1));
		}
	}

	std::cout << "Loaded " << indices.size() << " indices" << std::endl;

	const unsigned int NUM_STRIPS = mapSize_x - 1;
	const unsigned int NUM_VERT_PER_STRIP = mapSize_z * 2;

	// register VAO
	GLuint terrainVAO, terrainVBO, terrainEBO;
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(float),       // size of vertices buffer
		&vertices[0],                          // pointer to first element
		GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &terrainEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int), // size of indices buffer
		&indices[0],                           // pointer to first element
		GL_STATIC_DRAW);

	//setting constant uniforms
	objectShader.use();
	glm::vec3 sun_color = glm::vec3(0.98, 0.85, 0.65);
	glm::vec3 sun_dir = glm::vec3(-0.2, -1.0f, -0.3f);
	objectShader.setVec3("dir.specular", 1.0f * sun_color);
	objectShader.setVec3("dir.diffuse", 0.6f * sun_color);
	objectShader.setVec3("dir.ambient", 0.3f * sun_color);
	objectShader.setVec3("dir.direction", sun_dir);

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

		objectShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)screenWidth / (float)screenHeight, 0.1f, 10000.0f);
		glm::mat4 view = camera.getViewMatrix();
		objectShader.setMat4("projection", projection);
		objectShader.setMat4("view", view);
		objectShader.setFloat("minHeight", seaLevel);
		objectShader.setFloat("maxHeight", yScale - yShift);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		objectShader.setMat4("model", model);

		//camera pos uniform
		objectShader.setVec3("viewPos", camera.Position);

		//draw calls
		glBindVertexArray(terrainVAO);
		for (unsigned int strip = 0; strip < NUM_STRIPS; strip++) {
			if (toggleWireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_TRIANGLE_STRIP, NUM_VERT_PER_STRIP, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * NUM_VERT_PER_STRIP * strip));
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
	static bool tWasDown = false;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		if (!tWasDown) {
			tWasDown = true;
			toggleWireframe = !toggleWireframe;
		}
	}
	else tWasDown = false;


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

static float getRandom(float min, float max)
{
	if (min > max)
		return max;
	float num = rand();
	while (num < min || num > max)
		num = rand();

	return num;
}
