#include "TerrainRenderer.h"
#include <GLFW/glfw3.h>
#include <thread>

TerrainRenderer::TerrainRenderer(Shader shader, TerrainMesh& tMesh) {
	this->shader = shader;
	loadVerticesComp(tMesh);
	loadIndices(tMesh.map_dimensions.first, tMesh.map_dimensions.second, tMesh.indices);
	setupBuffersComp(tMesh.vertices, tMesh.indices);
};

void TerrainRenderer::RenderTerrain(TerrainMesh& tMesh, glm::vec3 sunCol, glm::vec3 sunDir, const Camera& c, int screenWidth, int screenHeight, int f) {
	setupSun(sunCol, sunDir);
	setupUniforms(c, screenWidth, screenHeight, tMesh, f);
	drawCall(f, tMesh.map_dimensions, tMesh.heightMap_texture);
};

void TerrainRenderer::setupBuffersComp(std::vector<float>& vertices, std::vector<unsigned int>& indices) {

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

void TerrainRenderer::drawCall(int f, std::pair<int, int> size, unsigned int heightmap) {

	int NUM_STRIPS = size.first - 1;
	int NUM_VERT_PER_STRIP = size.second * 2;

	//draw calls
	glBindVertexArray(terrainVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightmap);

	for (unsigned int strip = 0; strip < NUM_STRIPS; strip++) {
		if (f & WIREFRAME)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLE_STRIP, NUM_VERT_PER_STRIP, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * NUM_VERT_PER_STRIP * strip));
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::loadVerticesComp(TerrainMesh& tMesh) {
	int mapSize_x = tMesh.map_dimensions.first;
	int mapSize_z = tMesh.map_dimensions.second;
	float yScale = tMesh.yScale;
	float yShift = tMesh.yShift;
	float seaLevel = tMesh.seaLevel;

	tMesh.vertices = std::vector<float>(mapSize_x * mapSize_z * 5, 0);

	for (int i = 0; i < mapSize_x; i++) {
		for (int j = 0; j < mapSize_z; j++) {

			//load vertices
			int ind = (i * mapSize_z + j) * 5;
			tMesh.vertices[ind++] = -mapSize_x / 2.0f + i;
			tMesh.vertices[ind++] = 0;
			tMesh.vertices[ind++] = -mapSize_z / 2.0f + j;

			//load uv
			tMesh.vertices[ind++] = float(i) / (mapSize_x - 1);
			tMesh.vertices[ind++] = float(j) / (mapSize_z - 1);
		}
	}

	std::cout << "Loaded " << tMesh.vertices.size() << " vertices" << std::endl;
}

void TerrainRenderer::loadIndices(int mapSize_x, int mapSize_z, std::vector<unsigned int>& indices) {

	indices = std::vector<unsigned int>(mapSize_z * mapSize_x * 2, 0);

	const unsigned int max_threads = std::thread::hardware_concurrency();
	const unsigned int iterations_per_thread = mapSize_x / max_threads;

	std::vector<std::thread> threadpool;
	for (int i = 0; i < max_threads; i++) {

		int start = i * iterations_per_thread;
		int end = (i == max_threads - 1) ? mapSize_x : start + iterations_per_thread;
		threadpool.emplace_back(&TerrainRenderer::loadIndicesRange, this, mapSize_x, mapSize_z, std::ref(indices), start, end);
	}

	for (std::thread& t : threadpool) {
		t.join();
	}

	std::cout << "Loaded " << indices.size() << " indices" << std::endl;
}

void TerrainRenderer::loadIndicesRange(int mapSize_x, int mapSize_z, std::vector<unsigned int>& indices, int start, int end) {

	for (int i = start; i < end; i++) {
		for (int j = 0; j < mapSize_z; j++) {

			int ind = (i * mapSize_z + j) * 2;

			indices[ind] = i * mapSize_z + j;
			indices[ind + 1] = (i + 1) * mapSize_z + j;

		}
	}
}
