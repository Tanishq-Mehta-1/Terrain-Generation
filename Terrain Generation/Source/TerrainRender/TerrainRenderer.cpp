#include "TerrainRenderer.h"
#include <GLFW/glfw3.h>

TerrainRenderer::TerrainRenderer(Shader shader, TerrainMesh& tMesh) {
	this->shader = shader;
	loadVertices(tMesh);
	loadIndices(tMesh.map_dimensions.first, tMesh.map_dimensions.second, tMesh.indices);
	setupBuffers(tMesh.vertices, tMesh.indices);
};

void TerrainRenderer::RenderTerrain(TerrainMesh& tMesh, glm::vec3 sunCol, glm::vec3 sunDir, const Camera& c, int screenWidth, int screenHeight, int f) {
	setupSun(sunCol, sunDir);
	setupUniforms(c, screenWidth, screenHeight, tMesh, f);
	drawCall(f, tMesh.map_dimensions);
};

void TerrainRenderer::setupBuffers(std::vector<float>& vertices, std::vector<unsigned int>& indices) {

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

	shader.setFloat("minHeight", tMesh.seaLevel);
	shader.setFloat("maxHeight", tMesh.yScale - tMesh.yShift);
	shader.setBool("toggleFog", (f & FOG));
	shader.setBool("toggleAtmosphere", (f & ATMOSHPHERE));
	shader.setVec3("viewPos", c.Position);
}

void TerrainRenderer::drawCall(int f, std::pair<int,int> size) {

	int NUM_STRIPS = size.first - 1;
	int NUM_VERT_PER_STRIP = size.second * 2;

	//draw calls
	glBindVertexArray(terrainVAO);
	for (unsigned int strip = 0; strip < NUM_STRIPS; strip++) {
		if (f & WIREFRAME)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLE_STRIP, NUM_VERT_PER_STRIP, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * NUM_VERT_PER_STRIP * strip));
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::loadVertices(TerrainMesh& tMesh) {

	int mapSize_x = tMesh.map_dimensions.first;
	int mapSize_z = tMesh.map_dimensions.second;
	float yScale = tMesh.yScale;
	float yShift = tMesh.yShift;
	float seaLevel = tMesh.seaLevel;

	for (int i = 0; i < mapSize_x; i++) {
		for (int j = 0; j < mapSize_z; j++) {

			double y = tMesh.data[i * mapSize_z + j];
			float height = y * yScale - yShift;
			if (height < seaLevel) height = seaLevel;

			tMesh.vertices.push_back(-mapSize_x / 2.0f + i);
			tMesh.vertices.push_back(height);
			tMesh.vertices.push_back(-mapSize_z / 2.0f + j);

			//calculate normals
			float hL, hR, hU, hD;
			hL = hR = hD = hU = height;

			if (i != 0)
				hL = tMesh.data[(i - 1) * mapSize_z + j] * yScale - yShift; // left
			if (i != mapSize_x - 1)
				hR = tMesh.data[(i + 1) * mapSize_z + j] * yScale - yShift; // right
			if (j != 0)
				hD = tMesh.data[i * mapSize_z + j - 1] * yScale - yShift; // down
			if (j != mapSize_z - 1)
				hU = tMesh.data[i * mapSize_z + j + 1] * yScale - yShift; // up

			glm::vec3 normal = glm::normalize(glm::vec3((hL - hR) / 2.0f, 1.0f, (hD - hU) / 2.0f));
			tMesh.vertices.push_back(normal.x);
			tMesh.vertices.push_back(normal.y);
			tMesh.vertices.push_back(normal.z);
		}
	}

	std::cout << "Loaded " << tMesh.vertices.size() << " vertices" << std::endl;
}

void TerrainRenderer::loadIndices(int mapSize_x, int mapSize_z, std::vector<unsigned int>& indices) {

	for (int i = 0; i < mapSize_z - 1; i++) {
		for (int j = 0; j < mapSize_x; j++) {
			indices.push_back(i * mapSize_z + j);
			indices.push_back(j + mapSize_z * (i + 1));
		}
	}

	std::cout << "Loaded " << indices.size() << " indices" << std::endl;
}
