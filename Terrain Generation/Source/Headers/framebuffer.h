#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Framebuffer {

public:

	Framebuffer(int sWidth, int sHeight);
	void const bind();


private:
	unsigned int fbo;
	unsigned int color_buffer;

	unsigned int generateFrameBuffer(unsigned int& fbo, unsigned int& color_buffer, bool sampling, int screenWidth, int screenHeight);
};


#endif 