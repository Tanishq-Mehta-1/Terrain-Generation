#include "TerrainRenderer.h"
#include <GLFW/glfw3.h>
//#include <thread>

TerrainRenderer::TerrainRenderer(Shader& shader, TerrainMesh& tMesh) {
	this->shader = shader;
	loadVerticesTess(tMesh);
	setupBuffersTess(tMesh.vertices);

	this->shadowShader = setupShadowShader();
	auto [fbo, map] = setupShadowBuffers(1024, 1024);
	this->shadowFBO = fbo;
	this->shadowDepthMap = map;

	std::cout << "Shadow FBO: " << this->shadowFBO << std::endl;
	std::cout << "Shadow Depth Map: " << this->shadowDepthMap << std::endl;
};

void TerrainRenderer::RenderTerrain(TerrainMesh& tMesh, glm::vec3 sunCol, glm::vec3 sunDir, const Camera& c, int screenWidth, int screenHeight, int f, glm::vec4 bgCol) {

	glm::mat4 lightTransform = createDepthMap(1024, 1024, sunDir, tMesh);
	setupSun(sunCol, sunDir);
	//setupUniforms(c, screenWidth, screenHeight, tMesh, f, bgCol, glm::mat4(-1.0f));
	setupUniforms(c, screenWidth, screenHeight, tMesh, f, bgCol, lightTransform);
	drawCallTess(f, tMesh.rez, tMesh.heightMap_texture, this->shadowDepthMap);
};

void TerrainRenderer::setupSun(glm::vec3 sunCol, glm::vec3 sunDir) {

	shader.use();

	glm::vec3 sun_color = sunCol;
	glm::vec3 sun_dir = sunDir;
	shader.setVec3("dir.specular", 1.0f * sun_color);
	shader.setVec3("dir.diffuse", 0.6f * sun_color);
	shader.setVec3("dir.ambient", 0.3f * sun_color);
	shader.setVec3("dir.direction", sun_dir);
}

void TerrainRenderer::setupUniforms(const Camera& c, int screenWidth, int screenHeight, const TerrainMesh& tMesh, int f, glm::vec4 bgCol , glm::mat4 lightTransform) {

	shader.use();

	glViewport(0, 0, screenWidth, screenHeight);
	glm::mat4 projection = glm::perspective(glm::radians(c.zoom), (float)screenWidth / (float)screenHeight, 0.1f, 10000.0f);
	glm::mat4 view = c.getViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);
	shader.setInt("heightMap", 0);
	shader.setFloat("yShift", tMesh.yShift);
	shader.setFloat("yScale", tMesh.yScale);
	shader.setFloat("seaLevel", tMesh.seaLevel);
	shader.setMat4("lightSpaceMatrix", lightTransform);

	shader.setInt("depthMap", 1);
	shader.setFloat("minHeight", tMesh.seaLevel);
	shader.setFloat("maxHeight", tMesh.yScale - tMesh.yShift);
	shader.setBool("toggleFog", (f & FOG));
	shader.setBool("toggleAtmosphere", (f & ATMOSHPHERE));
	shader.setVec4("bgCol", bgCol);
	shader.setVec3("viewPos", c.Position);
}

void TerrainRenderer::drawCallDepth(int rez, unsigned int heightmap) {

	//draw calls
	glBindVertexArray(terrainVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightmap);

	glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::drawCallTess(int f, int rez, unsigned int heightMap, unsigned int depthMap) {

	//draw calls
	glBindVertexArray(terrainVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	if (f & WIREFRAME)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

std::pair<unsigned int, unsigned int> TerrainRenderer::setupShadowBuffers(int dm_width, int dm_height) {
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	//create a texture to store the depth map
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, dm_width, dm_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// ADD THIS CHECK:
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return {depthMapFBO, depthMap};
}

Shader TerrainRenderer::setupShadowShader() {
	return Shader(
		"C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Shaders\\depth.vert",
		"C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Shaders\\depth.frag",
		"C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Shaders\\tess_tcs.glsl",
		"C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Shaders\\tess_tes.glsl");
}

glm::mat4 TerrainRenderer::createDepthMap(int dm_width, int dm_height, glm::vec3 sunDir, TerrainMesh& tMesh) {
	
	glm::mat4 lightView = glm::lookAt(sunDir, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 lightProjection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, -1000.0f, 1000.0f);
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	shadowShader.use();

	shadowShader.setMat4("projection", lightProjection);
	shadowShader.setMat4("view", lightView);
	shadowShader.setMat4("model", glm::mat4(1.0f));
	shadowShader.setInt("heightMap", 0);
	shadowShader.setFloat("yShift", tMesh.yShift);
	shadowShader.setFloat("yScale", tMesh.yScale);
	shadowShader.setFloat("seaLevel", tMesh.seaLevel);
	shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tMesh.heightMap_texture);

	//render
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowFBO);
	glViewport(0, 0, dm_width, dm_height);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawCallDepth(tMesh.rez, tMesh.heightMap_texture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return lightSpaceMatrix;
}

void TerrainRenderer::loadVerticesTess(TerrainMesh& tMesh) {
	int mapSize_x = tMesh.map_dimensions.first;
	int mapSize_z = tMesh.map_dimensions.second;
	float yScale = tMesh.yScale;
	float yShift = tMesh.yShift;
	float seaLevel = tMesh.seaLevel;
	float rez = tMesh.rez;

	tMesh.vertices.reserve(rez * rez * 4);

	for (int i = 0; i <= rez; i++) {
		for (int j = 0; j <= rez - 1; j++) {

			tMesh.vertices.push_back(-mapSize_x / 2.0f + mapSize_x * i / rez); // v.x
			tMesh.vertices.push_back(0.0f); // v.y
			tMesh.vertices.push_back(-mapSize_z / 2.0f + mapSize_z * j / rez); // v.z
			tMesh.vertices.push_back(i / rez); // u
			tMesh.vertices.push_back(j / rez); // v

			tMesh.vertices.push_back(-mapSize_x / 2.0f + mapSize_x * (i + 1) / rez); // v.x
			tMesh.vertices.push_back(0.0f); // v.y
			tMesh.vertices.push_back(-mapSize_z / 2.0f + mapSize_z * j / rez); // v.z
			tMesh.vertices.push_back((i + 1) / rez); // u
			tMesh.vertices.push_back(j / rez); // v

			tMesh.vertices.push_back(-mapSize_x / 2.0f + mapSize_x * i / rez); // v.x
			tMesh.vertices.push_back(0.0f); // v.y
			tMesh.vertices.push_back(-mapSize_z / 2.0f + mapSize_z * (j + 1) / rez); // v.z
			tMesh.vertices.push_back(i / rez); // u
			tMesh.vertices.push_back((j + 1) / rez); // v

			tMesh.vertices.push_back(-mapSize_x / 2.0f + mapSize_x * (i + 1) / rez); // v.x
			tMesh.vertices.push_back(0.0f); // v.y
			tMesh.vertices.push_back(-mapSize_z / 2.0f + mapSize_z * (j + 1) / rez); // v.z
			tMesh.vertices.push_back((i + 1) / rez); // u
			tMesh.vertices.push_back((j + 1) / rez); // v
		}
	}

	std::cout << "Loaded " << tMesh.vertices.size() << " vertices" << std::endl;
}

void TerrainRenderer::setupBuffersTess(std::vector<float>& vertices) {

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	// register VAO
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(float),       // size of vertices buffer
		vertices.data(),                          // pointer to first element
		GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}