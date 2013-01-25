//! ShaderProgram.h
/*!
 * @date	30.11.2012
 * @author	Guido Schmidt
 */

#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

//! C++ includes
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
//! OpenGL includes
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
//! glm include
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace GLSL{
	enum GLSLShaderType{
		VERTEX = 0,
		FRAGMENT = 1,
		GEOMETRY = 2,
		TESS_CONTROL = 3,
		TESS_EVALUATION = 4
	};

	std::string ReadShaderSource(const std::string filename);
	void PrintShaderInfoLog(GLuint shader);
	void PrintShaderProgramInfoLog(GLuint shaderProgram);
}

class ShaderProgram
{
	private:
		GLint m_shaderProgram_ID;
		std::vector<GLint> m_shader_IDs;
		std::vector<std::string> m_shader_sources;
		bool m_islinked;
		bool m_activeUniformsWritten;
		bool m_activeAttributesWritten;
		GLenum m_errorCheckValue;
		std::string m_log;

		int GetUniformLocation(std::string name);
		bool CheckFileExist(const std::string &filename);

	public:
		ShaderProgram();
		ShaderProgram(GLSL::GLSLShaderType shaderType0, std::string filename0, GLSL::GLSLShaderType shaderType1, std::string filename1);
		~ShaderProgram();
		void AddShader(GLSL::GLSLShaderType shaderType, std::string filename);

		void Link(void);
		bool IsLinked(void);
		void Use(void);
		void Unuse(void);
		void ReloadAllShaders(void);
		void ReloadShader(int i);

		std::string Log(void);

		GLint GetHandle(void);

		//! Attribute bindings
		void BindAttributeLocation(GLuint location, std::string name);
		void BindFragDataLocation(GLuint location, std::string* name);

		//! Uniforms bindings
		void SetUniform(const std::string name, const glm::vec3 &vec);
		void SetUniform(const std::string name, const glm::vec4 &vec);
		void SetUniform(const std::string name, const glm::mat4 &mat);
		void SetUniform(const std::string name, float val);
		void SetUniform(const std::string name, int val);

		void SetUniformSampler(const std::string name, GLuint texture, int textureUnit);
		void SetUniformSamplerNoTextureBind(const std::string name, GLuint tetxure, int textureUnit);

		void PrintActiveUniforms(void);
		void PrintActiveAttributes(void);
};

#endif /* SHADERPROGRAM_H_ */
