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
	void RenderTerrain(TerrainMesh& tMesh, glm::vec3 sunCol, glm::vec3 sunDir, const Camera& c, int screenWidth, int screenHeight, int f); 
	void setupBuffersTess(std::vector<float>& vertices);

private:

	unsigned int terrainVAO;
	unsigned int terrainVBO;
	unsigned int terrainEBO;
	Shader shader;

	void setupSun(glm::vec3 sunCol, glm::vec3 sunDir);
	void setupUniforms(const Camera& c, int screenWidth, int screenHeight, const TerrainMesh& tMesh, int f);
	void drawCallTess(int f, int rez, unsigned int heightmap);
	void loadVerticesTess(TerrainMesh& tMesh);
};

#endif