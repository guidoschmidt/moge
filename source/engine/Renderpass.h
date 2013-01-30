//! Renderpass.h
/*!
 * @date	30.01.2013
 * @author	Guido Schmidt
 */
#include "ShaderProgram.h"
#include "FrameBufferObject.h"

#ifndef RENDERPASS_H_
#define RENDERPASS_H_

class Renderpass {
	private:
		std::string m_name;
		ShaderProgram* m_shaderProgram_ptr;
		FrameBufferObject* m_frameBufferObject_ptr;

	public:
		Renderpass(std::string name);
		Renderpass(std::string name, ShaderProgram sp, FrameBufferObject fbo);
		virtual ~Renderpass();

		void AddShaderProgram(ShaderProgram sp);
		void AddFrameBufferObject(FrameBufferObject fbo);

		void Run(void);
};

#endif //! RENDERPASS_H_
