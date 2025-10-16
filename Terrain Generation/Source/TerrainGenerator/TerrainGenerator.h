#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "perlin.h"
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

	TerrainGenerator(int period);
	std::pair<int,int> generateHeightmap(int mapSize_x, int mapSize_z, double persistence, double scale, int octaves, GenerationFlags flag, bool write_to_file, std::vector<float>& data);
	unsigned int generateHeightmapComp(int mapSize_x, int mapSize_z, double persistence, double scale, int octaves);
	std::pair<int,int> loadHeightmap(std::string texture_name, std::vector<float>& data);

private:
	void generateData(int start, int end, double scale, int octaves, double persistence, int mapSize_z, GenerationFlags flag, std::vector<float>& data);
	Perlin perlin;
};

#endif


