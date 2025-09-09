#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Framebuffer {

public:

	Framebuffer(int sWidth, int sHeight) {
		generateFrameBuffer(fbo, color_buffer, false, sWidth, sHeight);
	}

	void bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}


private:
	unsigned int fbo;
	unsigned int color_buffer;

	unsigned int generateFrameBuffer(unsigned int& fbo, unsigned int& color_buffer, bool sampling, int screenWidth, int screenHeight) {

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//texture attachment
		glGenTextures(1, &color_buffer);
		glBindTexture(GL_TEXTURE_2D, color_buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);

		if (sampling) {
			//depth and stencil attachment
			unsigned int depthStencil;
			glGenTextures(1, &depthStencil);
			glBindTexture(GL_TEXTURE_2D, depthStencil);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, screenWidth, screenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);
		}
		else {
			//Renderbuffer object
			unsigned int rbo;
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER::framebuffer is not complete!";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return fbo;
	}

};


#endif 