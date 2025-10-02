#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <cstdio> //for stbi image write

#define STB_IMAGE_IMPLEMENTATION    
#include <stb_image.h>

#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int period) {
	perlin.repeat = period;

};

//with given parameters, give options for fbm, domain warp etc
std::pair<int,int> TerrainGenerator::generateHeightmap(int mapSize_x, int mapSize_z, double persistence, double scale, int octaves, GenerationFlags flag, bool write_to_file, std::vector<float>& data)
{
	data = std::vector<float>(mapSize_x * mapSize_z, 0);

	//check if already present
	std::string size = std::to_string(mapSize_x) + 'x' + std::to_string(mapSize_z);
	std::string p = std::to_string(persistence);
	std::string s = std::to_string(scale);
	std::string o = std::to_string(octaves);
	std::string f = std::to_string(flag);
	std::string name = "Media/Generated/perlin_" + size + "_p" + p + "_s" + s + "_o" + o + "_f" + f + ".bin";

	std::ifstream perlin_map(name, std::ios::binary);
	if (perlin_map.fail()) {

		std::cout << "Generating data.....\n";

		for (int i = 0; i < mapSize_x; i++) {
			for (int j = 0; j < mapSize_z; j++) {

				double x = i * scale;
				double y = j * scale;

				if (flag == DOMAIN_WARP)
					data[i * mapSize_z + j] = perlin.DW_Perlin(glm::vec2(x,y), octaves, persistence);
				else if (flag == FBM)
					data[i * mapSize_z + j] = perlin.octavePerlin(glm::vec2(x, y), octaves, persistence);
				else
					data[i * mapSize_z + j] = perlin.perlin(x, y);
			}
		}

		if (write_to_file) {
			std::ofstream file(name, std::ios::binary);
			if (!file) throw std::runtime_error("Could not open the file: " + name + '\n');

			//write to binary
			file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(float));
			file.close();
			std::cout << "Created file: " + name + '\n';
		}
	}
	else {

		//use the heightmap
		perlin_map.read(reinterpret_cast<char*>(data.data()), mapSize_x * mapSize_z * sizeof(float));
		perlin_map.close();
		std::cout << "loaded file: " + name + '\n';
	}

	return { mapSize_x, mapSize_z };
}

//given name
std::pair<int,int> TerrainGenerator::loadHeightmap(std::string texture_name, std::vector<float>& data )
{
	std::cout << "Loading " << texture_name << '\n';

	int width, height, nChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* temp = stbi_load(texture_name.c_str(), &width, &height, &nChannels, 0);
	if (!temp) throw std::runtime_error("Texture not loaded!\n");

	//std::vector<float>data(height * width, 0);
	data = std::vector<float>(width * height, 0);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {

			unsigned char* texel = temp + (j * width + i) * nChannels;
			data[j * width + i] = static_cast<float>(*(texel)) / 255.0f;
		}
	}

	std::vector<float> transposed(height * width, 0);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {

			int ind_old = j * width + i;
			int ind_new = i * height + j;

			transposed[ind_new] = data[ind_old];
		}
	}

	data = transposed;
	return { width, height };
}




