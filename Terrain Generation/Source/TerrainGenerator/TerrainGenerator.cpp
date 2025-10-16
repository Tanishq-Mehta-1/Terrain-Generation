#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <cstdio> //for stbi image write

#define STB_IMAGE_IMPLEMENTATION    
#include <stb_image.h>

#include "TerrainGenerator.h"

#include <shader.h>
#include <thread>

TerrainGenerator::TerrainGenerator(int period) {
	perlin.repeat = period;
};

//with given parameters, give options for fbm, domain warp etc
std::pair<int, int> TerrainGenerator::generateHeightmap(int mapSize_x, int mapSize_z, double persistence, double scale, int octaves, GenerationFlags flag, bool write_to_file, std::vector<float>& data)
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

		const unsigned int max_threads = std::thread::hardware_concurrency();
		const unsigned int iterations_per_thread = mapSize_x / max_threads;

		std::vector<std::thread> threadpool;
		for (int i = 0; i < max_threads; i++) {

			int start = i * iterations_per_thread;
			int end = (i == max_threads - 1) ? mapSize_x : start + iterations_per_thread;
			threadpool.emplace_back(&TerrainGenerator::generateData, this, start, end, scale, octaves, persistence, mapSize_z, flag, std::ref(data));
		}

		for (std::thread& t : threadpool) {
			t.join();
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

unsigned int TerrainGenerator::generateHeightmapComp(int mapSize_x, int mapSize_z, double persistence, double scale, int octaves)
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
	perlin.setInt("repeat", 1000);
	perlin.setInt("octaves", octaves);
	perlin.setFloat("scale", scale); // Increased scale for more visible features
	perlin.setFloat("persistence", persistence);
	
	perlin.useCompute(ceil(mapSize_x / 8), ceil(mapSize_z / 8), 1);

	int w, h;
	glBindTexture(GL_TEXTURE_2D, imgOut); 
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w); 
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
	std::cout << w << ' ' << h << '\n';

 	return imgOut;
}

void TerrainGenerator::generateData(int start, int end, double scale, int octaves, double persistence, int mapSize_z, GenerationFlags flag, std::vector<float>& data) {

	for (int i = start; i < end; i++) {
		for (int j = 0; j < mapSize_z; j++) {

			double x = i * scale;
			double y = j * scale;

			if (flag == DOMAIN_WARP)
				data[i * mapSize_z + j] = perlin.DW_Perlin(glm::vec2(x, y), octaves, persistence);
			else if (flag == FBM)
				data[i * mapSize_z + j] = perlin.octavePerlin(glm::vec2(x, y), octaves, persistence);
			else
				data[i * mapSize_z + j] = perlin.perlin(x, y);

		}
	}
}

//given name
std::pair<int, int> TerrainGenerator::loadHeightmap(std::string texture_name, std::vector<float>& data)
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




