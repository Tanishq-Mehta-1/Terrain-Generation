#ifndef TERRAIN_RENDERER_H
#define TERRAIN_RENDERER_H

#include <shader.h>
#include <camera.h>

#include "../TerrainMesh.h"

enum RenderFlags {
	WIREFRAME = 1,
	FOG = 1 << 1,
	ATMOSHPHERE = 1 << 2,
};

class TerrainRenderer {

public:

	TerrainRenderer(Shader& shader, TerrainMesh& tMesh);
	void RenderTerrain(TerrainMesh& tMesh, glm::vec3 sunCol, glm::vec3 sunDir, const Camera& c, int screenWidth, int screenHeight, int f, glm::vec4 bgCol);
	void setupBuffersTess(std::vector<float>& vertices);

private:

	unsigned int terrainVAO;
	unsigned int terrainVBO;
	unsigned int terrainEBO;
	unsigned int shadowFBO;
	unsigned int shadowDepthMap;
	Shader shadowShader;
	Shader shader;

	void setupSun(glm::vec3 sunCol, glm::vec3 sunDir);
	void setupUniforms(const Camera& c, int screenWidth, int screenHeight, const TerrainMesh& tMesh, int f, glm::vec4 bgCol, glm::mat4 lightSpaceMatrix);
	void drawCallTess(int f, int rez, unsigned int heightMap, unsigned int depthMap);
	void drawCallDepth(int rez, unsigned int heightmap);
	void loadVerticesTess(TerrainMesh& tMesh);

	Shader setupShadowShader();
	glm::mat4 createDepthMap(int dm_width, int dm_height,glm::vec3 sunDir, TerrainMesh& tMesh);
	std::pair<unsigned int, unsigned int> setupShadowBuffers(int dm_width, int dm_height);
};

#endif