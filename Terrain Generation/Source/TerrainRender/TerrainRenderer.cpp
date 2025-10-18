#include "TerrainRenderer.h"
#include <GLFW/glfw3.h>
#include <thread>

TerrainRenderer::TerrainRenderer(Shader shader, TerrainMesh& tMesh) {
	this->shader = shader;
	loadVerticesTess(tMesh);
	setupBuffersTess(tMesh.vertices);
};

void TerrainRenderer::RenderTerrain(TerrainMesh& tMesh, glm::vec3 sunCol, glm::vec3 sunDir, const Camera& c, int screenWidth, int screenHeight, int f) {
	setupSun(sunCol, sunDir);
	setupUniforms(c, screenWidth, screenHeight, tMesh, f);
	drawCallTess(f, tMesh.rez, tMesh.heightMap_texture);
};

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

void TerrainRenderer::setupSun(glm::vec3 sunCol, glm::vec3 sunDir) {

	shader.use();
	glm::vec3 sun_color = sunCol;
	glm::vec3 sun_dir = sunDir;
	shader.setVec3("dir.specular", 1.0f * sun_color);
	shader.setVec3("dir.diffuse", 0.6f * sun_color);
	shader.setVec3("dir.ambient", 0.3f * sun_color);
	shader.setVec3("dir.direction", sun_dir);
}

void TerrainRenderer::setupUniforms(const Camera& c, int screenWidth, int screenHeight, const TerrainMesh& tMesh, int f) {

	shader.use();

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

	shader.setFloat("minHeight", tMesh.seaLevel);
	shader.setFloat("maxHeight", tMesh.yScale - tMesh.yShift);
	shader.setBool("toggleFog", (f & FOG));
	shader.setBool("toggleAtmosphere", (f & ATMOSHPHERE));
	shader.setVec3("viewPos", c.Position);
}

void TerrainRenderer::drawCallTess(int f, int rez, unsigned int heightmap) {

	//draw calls
	glBindVertexArray(terrainVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightmap);

	if (f & WIREFRAME)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::loadVerticesTess(TerrainMesh& tMesh) {
	int mapSize_x = tMesh.map_dimensions.first;
	int mapSize_z = tMesh.map_dimensions.second;
	float yScale = tMesh.yScale;
	float yShift = tMesh.yShift;
	float seaLevel = tMesh.seaLevel;
	float rez = tMesh.rez;

	tMesh.vertices.reserve(rez * rez * 4);

	for (int i = 0; i < rez; i++) {
		for (int j = 0; j < rez; j++) {

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
