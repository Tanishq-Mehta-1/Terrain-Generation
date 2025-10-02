#ifndef TERRAIN_MESH_H
#define TERRAIN_MESH_H

#include <vector>
#include <utility>

class TerrainMesh {

public:
	std::vector<float> data;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::pair<int, int> map_dimensions;

	float yScale;
	float yShift;
	float seaLevel;
	int NUM_STRIPS;
	int NUM_VERT_PER_STRIP;

	TerrainMesh(float yScale, float yShift, float seaLevel) {
		this->yScale = yScale;
		this->yShift = yShift;
		this->seaLevel = seaLevel;
	};
};

#endif

