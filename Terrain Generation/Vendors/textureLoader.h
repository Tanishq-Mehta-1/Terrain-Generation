#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>
#include <unordered_map>

class TextureLoader
{
public:

	std::unordered_map<std::string, unsigned int> textures;

	TextureLoader(std::vector<std::string> texturePaths) {
		for (std::string& it : texturePaths) {

			size_t pos = it.find_last_of("/\\");
			string name = it.substr(pos + 1);

			textures[name] = loadTexture(it);
			std::cout << name << ' ' << textures[name] << '\n';
		}

		//adding in the null texture
		textures["null"] = loadTexture("null");
	}

	~TextureLoader() {

		for (auto& it : textures)
			glDeleteTextures(1, &it.second);
	}

private:
	unsigned int loadTexture(const std::string& path) const
	{
		if (path == "null")
			return 0;

		stbi_set_flip_vertically_on_load(true);

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//loading image
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		GLint format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
			std::cout << "ERROR::TEXTURE::FAILED TO LOAD: " << path << '\n';

		stbi_image_free(data);
		stbi_set_flip_vertically_on_load(false);
		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	}

};

#endif // !TEXTURE_LOADER_H
