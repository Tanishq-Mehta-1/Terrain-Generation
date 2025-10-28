#include "TerrainGenerator.h"
#include "../Headers/Image_2D.h"

#include <shader.h>
#include <thread>

TerrainGenerator::TerrainGenerator() {};

unsigned int TerrainGenerator::generateHeightmapComp(int mapSize_x, int mapSize_z, double persistence, double scale, int octaves, int iterations, GenerationFlags flag, Erosion_Uniforms eu)
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

	fluvialErosion(eu, heightMap.getID(), mapSize_x, mapSize_z, iterations);

	/*int w, h;
	glBindTexture(GL_TEXTURE_2D, imgOut);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
	std::cout << w << ' ' << h << '\n';*/

	return heightMap.getID();
}

unsigned int TerrainGenerator::fluvialErosion(Erosion_Uniforms eu, unsigned int heightmap, int mapSize_x, int mapSize_z, int iterations)
{
	Shader fluvial("C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Source\\TerrainGenerator\\fluvial.comp");
	fluvial.use();
	fluvial.setFloat("dt", eu.dt);
	fluvial.setFloat("K_rain", eu.K_rain);
	fluvial.setFloat("flux_pipe_cross_section", eu.flux_pipe_cross_section);
	fluvial.setFloat("flux_pipe_length", eu.flux_pipe_length);
	fluvial.setFloat("K_gravity", eu.K_gravity);
	fluvial.setFloat("K_capacity", eu.K_capacity);
	fluvial.setFloat("K_dissolving", eu.K_dissolving);
	fluvial.setFloat("K_deposition", eu.K_deposition);
	fluvial.setFloat("K_evaporation", eu.K_evaporation);

	//generate and load textures
	Image2D PingPong(mapSize_x, mapSize_z, GL_RGBA32F, 1, GL_READ_WRITE);
	Image2D OutfluxMap_R(mapSize_x, mapSize_z, GL_RGBA32F, 2, GL_READ_WRITE);
	Image2D OutfluxMap_W(mapSize_x, mapSize_z, GL_RGBA32F, 3, GL_READ_WRITE);
	Image2D VelociyField_R(mapSize_x, mapSize_z, GL_RGBA32F, 4, GL_READ_WRITE);
	Image2D VelociyField_W(mapSize_x, mapSize_z, GL_RGBA32F, 5, GL_READ_WRITE);

	unsigned int to_read = heightmap;
	unsigned int to_write = PingPong.getID();
	unsigned int out_r = OutfluxMap_R.getID();
	unsigned int out_w = OutfluxMap_W.getID();
	unsigned int vel_r = VelociyField_R.getID();
	unsigned int vel_w = VelociyField_W.getID();

	int batch_size = 1000;
	for (int i = 0; i < iterations; i += batch_size)
	{
		int current_batch = std::min(batch_size, iterations - i);
		for (int j = 0; j < current_batch; j++)
		{
			glBindImageTexture(0, to_read, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			glBindImageTexture(1, to_write, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			glBindImageTexture(2, out_r, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			glBindImageTexture(3, out_w, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			glBindImageTexture(4, vel_r, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			glBindImageTexture(5, vel_w, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

			
			fluvial.useCompute(ceil((float)mapSize_x / 16.0f), ceil((float)mapSize_z / 16.0f), 1, GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			std::swap(to_read, to_write);
			std::swap(out_r, out_w);
			std::swap(vel_r, vel_w);
		}
		glFlush();
	}

	return iterations % 2 == 0 ? heightmap : PingPong.getID();
}

