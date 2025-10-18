#ifndef TERRAIN_MESH_H
#define TERRAIN_MESH_H

#include <vector>
#include <utility>

class TerrainMesh {

public:
	std::vector<float> vertices;
	std::pair<int, int> map_dimensions;

	unsigned int heightMap_texture;
	float rez;
	float yScale;
	float yShift;
	float seaLevel;
	int NUM_STRIPS;
	int NUM_VERT_PER_STRIP;

	TerrainMesh(float yScale, float yShift, float seaLevel, float rez) {
		this->yScale = yScale;
		this->yShift = yShift;
		this->seaLevel = seaLevel;
		this->rez = rez;
	};
};

#endif

