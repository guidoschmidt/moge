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
//! Local includes
#include "../utilities/Singleton.h"
#include "../engine/Context.h"

class FrameBufferObject
{
	private:
		GLuint m_FBO_ID;
		bool m_isGBuffer;
		int m_width, m_height;
		GLsizei m_attachmentCounter;

		GLuint m_depthBuffer;
		GLuint m_depthTexture;

		std::vector<GLuint> m_renderTargets;
		std::vector<GLenum> m_drawBuffers;

	public:
		FrameBufferObject(bool gBuffer);
		~FrameBufferObject();

		void AddColorAttachment(int textureUnit);
		void AddDepthAttachment_Buffer();
		void AddDepthAttachment_Texture(int textureUnit);

		void CreateGBuffer(void);
		void CreateBuffers(int count);


		void Use(void);
		void Unuse(void);

		//! Getters
		GLuint GetTexture(unsigned int index);
		GLuint GetDepthTexture(void);
		int GetRenderTargetCound(void);
};

#endif /* FRAMEBUFFEROBJECT_H_ */
