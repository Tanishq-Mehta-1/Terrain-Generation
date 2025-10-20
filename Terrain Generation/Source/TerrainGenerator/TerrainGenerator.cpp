#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <cstdio> //for stbi image write

#define STB_IMAGE_IMPLEMENTATION    
#include <stb_image.h>

#include "TerrainGenerator.h"

#include <shader.h>
#include <thread>

TerrainGenerator::TerrainGenerator() {};

unsigned int TerrainGenerator::generateHeightmapComp(int mapSize_x, int mapSize_z, double persistence, double scale, int octaves, GenerationFlags flag)
{
	Shader perlin("C:\\Users\\tanis\\source\\repos\\Terrain Generation\\Terrain Generation\\Source\\TerrainGenerator\\perlin.comp");

	//generate image texture
	unsigned int imgOut;
	glGenTextures(1, &imgOut);
	glBindTexture(GL_TEXTURE_2D, imgOut);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//geneerate an empty image object
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mapSize_x, mapSize_z, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, imgOut, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

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

 	return imgOut;
}


