#ifndef IMAGE_2D
#define IMAGE_2D

#include <glad/glad.h>

class Image2D
{
public:

	Image2D(int size_x, int size_y, GLenum internal_format, unsigned int unit_to_bind, GLenum image_access)
	{
		glGenTextures(1, &this->id);
		glBindTexture(GL_TEXTURE_2D, this->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, size_x, size_y, 0, GL_RGBA, GL_FLOAT, 0);
		glBindImageTexture(unit_to_bind, this->id, 0, GL_FALSE, 0, image_access, GL_RGBA32F);
	}

	unsigned int getID()
	{
		return this->id;
	}

private:
	unsigned int id;

};

#endif // !IMAGE_2D	
