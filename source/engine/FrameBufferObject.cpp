//! FrameBufferObject.cpp
/*!
 * @date 	02.01.2013
 * @author	Guido Schmidt
 */

#include "FrameBufferObject.h"

//! Constructor
/*!
 * @param width
 * @param height
 */
FrameBufferObject::FrameBufferObject(int width, int height)
{
	m_width = Singleton<Context>::Instance()->GetWidth();
	m_height = Singleton<Context>::Instance()->GetHeight();
	m_attachmentCounter = 0;
	glGenFramebuffers(1, &m_FBO_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO_ID);

	CreateGBuffer();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//! Destructor
FrameBufferObject::~FrameBufferObject()
{
}

//! Creates a G-Buffer for deferred rendering.
/*!
 * Creates a G-Buffer = Attaches three color attachments (position, color, normal) and one
 * depth attachment texture to the frame buffer object.
 */
void FrameBufferObject::CreateGBuffer(void)
{
	//! Positions
	AddColorAttachment(0);
	//! Colors
	AddColorAttachment(1);
	//! Normals
	AddColorAttachment(2);
	//! MaterialIDs
	AddColorAttachment(3);
	//! Reflectivity
	AddColorAttachment(4);
	//! Depth
	AddDepthAttachment_Texture(5);
}


//! Adds a color attachment to the framebuffer object
/*!
 * Adds a color attachment to the framebuffer object, generating a texture used as render target.
 */
void FrameBufferObject::AddColorAttachment(GLint textureUnit)
{
	GLuint renderTexture;

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	//! Bind texture to color attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_attachmentCounter, GL_TEXTURE_2D, renderTexture, 0);
	m_attachmentCounter++;
	m_renderTargets.push_back(renderTexture);
}


//! Adds a depth buffer to the framebuffer object
/*!
 * Adds a depth buffer to the framebuffer object, using a renderbuffer.
 */
void FrameBufferObject::AddDepthAttachment_Buffer(void)
{
	//! Create a depth buffer
	glGenRenderbuffers(1, &m_depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);

	//! Bind depth buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
}


//! Adds a depth map to the framebuffer object
/*!
 * Adds a depth map to the framebuffer object, using a renderbuffer and a generated texture as render target.
 * @param textureUnit
 */
void FrameBufferObject::AddDepthAttachment_Texture(int textureUnit)
{
	//! Create depth texture
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
}


//! Activated the framebuffer object
/*!
 * Binds the framebuffer object and draws it's buffers.
 */
void FrameBufferObject::Use(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO_ID);

	GLenum drawBuffers[5] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
	glDrawBuffers(5, drawBuffers);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
}

//! Unbinds the framebuffer object
void FrameBufferObject::Unuse(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//! Returns a render target texture
/*!
 * @param index
 * @return a render target texture
 */
GLuint FrameBufferObject::GetTexture(unsigned int index)
{
	if(index > m_renderTargets.size()-1){
		std::cout << "ERROR | FrameBufferObject: texture index out of bounds!" << std::endl;
	}
	return m_renderTargets[index];
}


//! Returns the frame buffers depth map
/*!
 *
 * @return the framebuffers depth map texture
 */
GLuint FrameBufferObject::GetDepthTexture(void)
{
	return m_depthTexture;
}