#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "../Erosion_Uniforms.h"
#include <iostream>
#include <fstream>

#include <vector>
#include <utility>
#include <string>

enum GenerationFlags {
	DEFAULT = 0,
	FBM = 1,
	DOMAIN_WARP = 2,
};

class TerrainGenerator {

public:

	TerrainGenerator();
	unsigned int generateHeightmapComp(int mapSize_x, int mapSize_z, double persistence, double scale, int octaves, int iterations, GenerationFlags flag, Erosion_Uniforms eu);
	unsigned int fluvialErosion(Erosion_Uniforms eu, unsigned int heightmap, int mapSize_x, int mapSize_z, int iterations);

};

#endif


