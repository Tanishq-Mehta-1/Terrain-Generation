#include "TerrainGenerator.h"
#include "../Headers/Image_2D.h"

#include <shader.h>
#include <thread>

TerrainGenerator::TerrainGenerator() {};

unsigned int TerrainGenerator::generateHeightmapComp(int mapSize_x, int mapSize_z, double persistence, double scale, int octaves, GenerationFlags flag, Erosion_Uniforms eu)
{
	Shader perlin("C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Source\\TerrainGenerator\\perlin.comp");

	//generate image texture
	Image2D heightMap(mapSize_x, mapSize_z, GL_RGBA32F, 0, GL_READ_WRITE);
	perlin.use();
	perlin.setInt("repeat", 0);
	perlin.setInt("octaves", octaves);
	perlin.setFloat("scale", scale); // Increased scale for more visible features
	perlin.setFloat("persistence", persistence);
	perlin.setInt("flag", flag);

	perlin.useCompute(ceil((float)mapSize_x / 16.0f), ceil((float)mapSize_z / 16.0f), 1, GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	std::cout << "Generated Heightmap\n";

	return fluvialErosion(eu, heightMap.getID(), mapSize_x, mapSize_z);

	/*int w, h;
	glBindTexture(GL_TEXTURE_2D, imgOut);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
	std::cout << w << ' ' << h << '\n';*/
}

unsigned int TerrainGenerator::generateHeightmapComp(int mapSize_x, int mapSize_z, double persistence, double scale, int octaves, GenerationFlags flag)
{
	Shader perlin("C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Source\\TerrainGenerator\\perlin.comp");

	//generate image texture
	Image2D heightMap(mapSize_x, mapSize_z, GL_RGBA32F, 0, GL_READ_WRITE);
	perlin.use();
	perlin.setInt("repeat", 0);
	perlin.setInt("octaves", octaves);
	perlin.setFloat("scale", scale); // Increased scale for more visible features
	perlin.setFloat("persistence", persistence);
	perlin.setInt("flag", flag);

	perlin.useCompute(ceil((float)mapSize_x / 16.0f), ceil((float)mapSize_z / 16.0f), 1, GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	std::cout << "Generated Heightmap\n";
	/*int w, h;
	glBindTexture(GL_TEXTURE_2D, imgOut);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
	std::cout << w << ' ' << h << '\n';*/

	return heightMap.getID();
}

unsigned int TerrainGenerator::fluvialErosion(Erosion_Uniforms eu, unsigned int heightmap, int mapSize_x, int mapSize_z)
{
	Shader eroder("C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Source\\TerrainGenerator\\fluvial.comp");

	eroder.use();
	eroder.setFloat("p_inertia", eu.p_inertia);
	eroder.setFloat("p_min_slope", eu.p_min_slope);
	eroder.setFloat("p_capacity", eu.p_capacity);
	eroder.setFloat("p_deposition", eu.p_deposition);
	eroder.setFloat("p_erosion", eu.p_erosion);
	eroder.setFloat("p_gravity", eu.p_gravity);
	eroder.setFloat("p_evaporation", eu.p_evaporation);
	eroder.setFloat("p_radius", eu.p_radius);
	eroder.setInt("p_max_iteration", eu.p_max_iteration);
	glBindImageTexture(0, heightmap, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	eroder.useCompute(mapSize_x / 16, ceil(mapSize_z / 16), 1, GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	return heightmap;
}

