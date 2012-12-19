

#include "FrameBufferObject.h"

//! Constructor
FrameBufferObject::FrameBufferObject(int width, int height)
{
	WIDTH = width;
	HEIGHT = height;
	attachmentCounter = 0;
	glGenFramebuffers(1, &FBO_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);

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
	AddColorAttachment(1);
	//! Colors
	AddColorAttachment(2);
	//! Normals
	AddColorAttachment(3);
	//! Depth
	AddDepthAttachment_Texture(4);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	//! Bind texture to color attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentCounter, GL_TEXTURE_2D, renderTexture, 0);
	attachmentCounter++;
	renderTargets.push_back(renderTexture);
}


//! Adds a depth buffer to the framebuffer object
/*!
 * Adds a depth buffer to the framebuffer object, using a renderbuffer.
 */
void FrameBufferObject::AddDepthAttachment_Buffer(void)
{
	//! Create a depth buffer
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);

	//! Bind depth buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
}

//! Adds a depth map to the framebuffer object
/*!
 * Adds a depth map to the framebuffer object, using a renderbuffer and a generated texture as render target.
 * @param textureUnit
 */
void FrameBufferObject::AddDepthAttachment_Texture(int textureUnit)
{
	//! Create depth texture
	glActiveTexture(GL_TEXTURE4);
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
}


//! Activated the framebuffer object
/*!
 * Binds the framebuffer object and draws it's buffers.
 */
void FrameBufferObject::Use(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);

	GLenum drawBuffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(3, drawBuffers);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
}

//! Unbinds the framebuffer object
void FrameBufferObject::Unuse(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//! Returns a render target texture
/*!
 *
 * @param index
 * @return a render target texture
 */
GLuint FrameBufferObject::GetTexture(unsigned int index)
{
	if(index > renderTargets.size()-1){
		std::cout << "ERROR | FrameBufferObject: texture index out of bounds!" << std::endl;
	}
	return renderTargets[index];
}

//! Returns the frame buffers depth map
/*!
 *
 * @return the framebuffers depth map texture
 */
GLuint FrameBufferObject::GetDepthTexture(void)
{
	return depthTexture;
}
