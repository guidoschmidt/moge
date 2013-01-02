//! FrameBufferObjet.h
/*!
 * @date 	02.01.2013
 * @author	Guido Schmidt
 */

#ifndef FRAMEBUFFEROBJECT_H_
#define FRAMEBUFFEROBJECT_H_

//! C++ includes
#include <vector>
#include <iostream>
//! OpenGL includes
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

class FrameBufferObject
{
	private:
		int WIDTH, HEIGHT;
		int attachmentCounter;
		GLuint FBO_ID;
		GLuint depthBuffer;
		GLuint depthTexture;
		std::vector<GLuint> renderTargets;

	public:
		FrameBufferObject(int width, int height);
		~FrameBufferObject();

		void AddColorAttachment(int textureUnit);
		void AddDepthAttachment_Buffer();
		void AddDepthAttachment_Texture(int textureUnit);

		void CreateGBuffer(void);

		void Use(void);
		void Unuse(void);

		//! Getters
		GLuint GetTexture(unsigned int index);
		GLuint GetDepthTexture(void);
};

#endif /* FRAMEBUFFEROBJECT_H_ */
