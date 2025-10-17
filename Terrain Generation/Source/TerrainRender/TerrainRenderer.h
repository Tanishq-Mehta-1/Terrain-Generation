#ifndef TERRAIN_RENDERER_H
#define TERRAIN_RENDERER_H

#include <shader.h>
#include <camera.h>

#include "../TerrainMesh.h"

enum RenderFlags {
	WIREFRAME = 1 ,
	FOG = 1 << 1,
	ATMOSHPHERE = 1 << 2,
};

class TerrainRenderer {

public:

	TerrainRenderer(Shader shader, TerrainMesh& tMesh);
	void RenderTerrain(TerrainMesh& tMesh, glm::vec3 sunCol, glm::vec3 sunDir, const Camera& c, int screenWidth, int screenHeight, int f); //give data and render terrain, draw calls done here

	void setupBuffersComp(std::vector<float>& vertices, std::vector<unsigned int>& indices);

private:

	unsigned int terrainVAO;
	unsigned int terrainVBO;
	unsigned int terrainEBO;
	Shader shader;

	void setupSun(glm::vec3 sunCol, glm::vec3 sunDir);
	void setupUniforms(const Camera& c, int screenWidth, int screenHeight, const TerrainMesh& tMesh, int f);
	void drawCall(int f, std::pair<int, int> size, unsigned int heightmap);
	void loadVerticesComp(TerrainMesh& tMesh);
	void loadIndices(int mapSize_x, int mapSize_z, std::vector<unsigned int>& indices);
	void loadIndicesRange(int mapSize_x, int mapSize_z, std::vector<unsigned int>& indices, int start, int end);
};

#endif