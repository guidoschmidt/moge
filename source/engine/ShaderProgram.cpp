//! ShaderProgram.cpp
/*!
 *	@date 	30.11.2012
 *	@author	Guido Schmidt
 */


#include "ShaderProgram.h"

//! Namespace for global GLSL functions
namespace GLSL{
	//! Reads the shader source code from a given file
	/*!
	 * Reads the shader code from a given file and returns a single string.
	 *
	 * @param filename
	 * @return shader code as a string
	 */
	std::string ReadShaderSource(std::string filename)
	{
		std::string shaderSrc;
		std::string line;

		std::ifstream fileIn(filename.c_str());
		if(fileIn.is_open()){
			while(!fileIn.eof()){
				getline(fileIn, line);
				line += "\n";
				shaderSrc += line;
			}
			fileIn.close();
		}
		else
			std::cout << "ERROR: Unable to read shader source code from " << filename << std::endl;

		//std::cout << shaderSrc << std::endl;
		return shaderSrc;
	}

	//! Prints an info log for a shader
	/*!
	 * Prints the shader's info log to console.
	 * @param shader
	 */
	void PrintShaderInfoLog(GLuint shader)
	{
		GLint infoLogLength = 0;
		GLsizei charsWritten = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* log = (char*)malloc(infoLogLength);
		glGetShaderInfoLog(shader, infoLogLength, &charsWritten, log);
		std::cout << "\n------------------------------------------------------------" << std::endl;
		std::cout << "SHADER INFO LOG" << std::endl << "Shader: " << shader << std::endl << log << std::endl;
		std::cout << "------------------------------------------------------------" << std::endl;
		free(log);
	}

	//! Prints an info log for a shader program
	/*!
	 * Prints the shader's info log to console.
	 * @param shaderProgram
	 */
	void PrintShaderProgramInfoLog(GLuint shaderProgram)
	{
		GLint logLength;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
		if(logLength > 0){
			char* log = (char*) malloc(logLength);
			GLsizei written;
			glGetProgramInfoLog(shaderProgram, logLength, &written, log);
			std::cout << "\n------------------------------------------------------------" << std::endl;
			std::cout << "SHADER PROGRAM INFO LOG" << std::endl << "Shader Program: " << shaderProgram << std::endl << log << std::endl;
			std::cout << "\n------------------------------------------------------------" << std::endl;
			free(log);
		}
	}
}

//! Constructor
ShaderProgram::ShaderProgram()
{
	m_shaderProgram_ID = glCreateProgram();
	m_islinked = false;
	m_errorCheckValue = glGetError();
	m_activeAttributesWritten = false;
	m_activeUniformsWritten = false;
}

//! Constructor
/*!
 * Creates a shader program with two given shaders.
 *
 * @param shaderType0 first shader's shader type
 * @param filename0 first shader's source file
 * @param shaderType1 second shader's shader type
 * @param filename1 second shader's source file
 */
ShaderProgram::ShaderProgram(GLSL::GLSLShaderType shaderType0, std::string filename0, GLSL::GLSLShaderType shaderType1, std::string filename1)
{
	m_shaderProgram_ID = glCreateProgram();
	m_activeAttributesWritten = false;
	m_activeUniformsWritten = false;

	m_shader_sources.push_back(filename0);
	m_shader_sources.push_back(filename1);

	AddShader(shaderType0, m_shader_sources[0]);
	AddShader(shaderType1, m_shader_sources[1]);
	glAttachShader(m_shaderProgram_ID, m_shader_IDs[0]);
	glAttachShader(m_shaderProgram_ID, m_shader_IDs[1]);
	Link();
}

//! Destructor
ShaderProgram::~ShaderProgram()
{

}

//! Adds a shader to the program
/*!
 * Adds a shader to the shader program.
 *
 * @param shaderType
 * @param filename
 */
void ShaderProgram::AddShader(GLSL::GLSLShaderType shaderType, std::string filename)
{
	GLint Shader_ID;

	switch (shaderType) {
		case 0:
			Shader_ID = glCreateShader(GL_VERTEX_SHADER);
			break;
		case 1:
			Shader_ID = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
	}

	std::string shaderSource = GLSL::ReadShaderSource(filename);
	const char* shaderSourcePointer = shaderSource.c_str();
	glShaderSource(Shader_ID, 1, &shaderSourcePointer, NULL);
	glCompileShader(Shader_ID);
	GLSL::PrintShaderInfoLog(Shader_ID);

	m_shader_IDs.push_back(Shader_ID);
}


//!
/*!
 *
 */
void ShaderProgram::ReloadAllShaders(void)
{
	for(unsigned int i=0; i < m_shader_sources.size(); i++)
	{
		std::string shaderSource = GLSL::ReadShaderSource(m_shader_sources[i]);
		const char* shaderSourcePointer = shaderSource.c_str();
		glShaderSource(m_shader_IDs[i], 1, &shaderSourcePointer, NULL);
		glCompileShader(m_shader_IDs[i]);
		GLSL::PrintShaderInfoLog(m_shader_IDs[i]);
	}
	Link();
}


//!
/*!
 *
 */
void ShaderProgram::ReloadShader(int i)
{
	std::string shaderSource = GLSL::ReadShaderSource(m_shader_sources[i]);
	const char* shaderSourcePointer = shaderSource.c_str();
	glShaderSource(m_shader_IDs[i], 1, &shaderSourcePointer, NULL);
	glCompileShader(m_shader_IDs[i]);
	GLSL::PrintShaderInfoLog(m_shader_IDs[i]);
	Link();
}


//! Links the shader program
/*!
 * Links the shader program.
 */
void ShaderProgram::Link(void)
{
	glLinkProgram(m_shaderProgram_ID);

	GLint linkStatus;
	glGetProgramiv(m_shaderProgram_ID, GL_LINK_STATUS, &linkStatus);
	if(linkStatus == GL_FALSE){
		std::cout << "ERROR | ShaderProgram: shader program linking failed!" << std::endl;
		//GLSL::PrintShaderProgramInfoLog(m_shaderProgram_ID);
		m_islinked = false;
	}
	else{
		std::cout << "ShaderProgram: shader program linking succeded!" << std::endl;
		m_islinked = true;
	}
}

//! Get shader program's link status
bool ShaderProgram::IsLinked(void)
{
	return m_islinked;
}

//! Activates the shader program
void ShaderProgram::Use(void)
{
	glUseProgram(m_shaderProgram_ID);
}

//! Deactivates the shader program
void ShaderProgram::Unuse(void){
	glUseProgram(0);
}

//! Seta uniform matrix (4x4 components)
/*!
 * Binds a 4x4 matrix to the shader program as uniform.
 *
 * @param name uniform's name
 * @param mat  matrix to bind
 */
void ShaderProgram::SetUniform(std::string name, const glm::mat4 &mat)
{
	GLint glslMatrixID = glGetUniformLocation(m_shaderProgram_ID, name.c_str());
	glUniformMatrix4fv(glslMatrixID, 1, GL_FALSE, &mat[0][0]);
}

//! Set a uniform vector (3 components)
/*!
 * Binds a vector with 3 components to the shader program as uniform.
 *
 * @param name uniform's name
 * @param vec vector to bind
 */
void ShaderProgram::SetUniform(const std::string name, const glm::vec3 &vec)
{
	GLint glslVectorID = glGetUniformLocation(m_shaderProgram_ID, name.c_str());
	glUniform3f(glslVectorID, vec[0], vec[1], vec[2]);
}

//! Set a uniform vector (4 components)
/*!
 * Binds a vector with 4 components to the shader program as uniform.
 *
 * @param name uniform's name
 * @param vec vector to bind
 */
void ShaderProgram::SetUniform(const std::string name, const glm::vec4 &vec)
{
	GLint glslVectorID = glGetUniformLocation(m_shaderProgram_ID, name.c_str());
	glUniform4f(glslVectorID, vec[0], vec[1], vec[2], vec[3]);
}

//! Set a uniform integer
/*!
 * Binds a vector with 3 components to the shader program as uniform.
 *
 * @param name uniform's name
 * @param val integer to bind
 */
void ShaderProgram::SetUniform(const std::string name, int val)
{
	GLint glslIntID = glGetUniformLocation(m_shaderProgram_ID, name.c_str());
	glUniform1i(glslIntID, val);
}

//! Set a uniform float
/*!
 * Binds a float to the shader program as uniform.
 *
 * @param name uniform's name
 * @param val float to bind
 */
void ShaderProgram::SetUniform(const std::string name, float val)
{
	GLint glslFloatID = glGetUniformLocation(m_shaderProgram_ID, name.c_str());
	glUniform1f(glslFloatID, val);
}


//! Set a uniform texture
/*!
 * Set a texture to the shader program as a uniform.
 *
 * @param name uniform's name
 * @param texture texture to bind
 * @param textureUnit texture unit to read from
 */
void ShaderProgram::SetUniformSampler(std::string name, GLuint texture, GLint textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture);
	int loc = glGetUniformLocation(m_shaderProgram_ID, name.c_str());
	glUniform1i(loc, textureUnit);
}


//! Set a uniform texture
/*!
 * Set a texture to the shader program as a uniform.
 *
 * @param name uniform's name
 * @param texture texture to bind
 * @param textureUnit texture unit to read from
 */
void ShaderProgram::SetUniformCubemap(std::string name, GLuint texture, GLint textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	int loc = glGetUniformLocation(m_shaderProgram_ID, name.c_str());
	glUniform1i(loc, textureUnit);
}

//!
/*!
 *
 * @param location
 * @param name
 */
void ShaderProgram::BindAttributeLocation(GLuint location, std::string name)
{
	glBindAttribLocation(m_shaderProgram_ID, location, name.c_str());
}

//!
/*!
 *
 */
void ShaderProgram::PrintActiveAttributes(void)
{
	if(!m_activeAttributesWritten){
		GLint maxLength, nAttributes;
		glGetProgramiv(m_shaderProgram_ID, GL_ACTIVE_ATTRIBUTES, &nAttributes);
		glGetProgramiv(m_shaderProgram_ID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

		GLchar* name = (GLchar*) malloc(maxLength);
		GLint written, size, location;
		GLenum type;

		std::cout << "-----------------------------------------------------" << std::endl;
		std::cout << "Attribute Index | Name" << std::endl;
		for(int i=0; i < nAttributes; i++){
			glGetActiveAttrib(m_shaderProgram_ID, i, maxLength, &written, &size, &type, name);
			location = glGetAttribLocation(m_shaderProgram_ID, name);
			std::cout << location << " | " << name << std::endl;
		}
		free(name);
		m_activeAttributesWritten = !m_activeAttributesWritten;
	}
}

//!
/*!
 *
 */
void ShaderProgram::PrintActiveUniforms(void)
{
	if(!m_activeUniformsWritten){
		GLint maxLength, nUniforms;
		glGetProgramiv(m_shaderProgram_ID, GL_ACTIVE_UNIFORMS, &nUniforms);
		glGetProgramiv(m_shaderProgram_ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

		GLchar* name = (GLchar*) malloc(maxLength);
		GLint size, location;
		GLsizei written;
		GLenum type;
		std::cout << "-----------------------------------------------------" << std::endl;
		std::cout << "Uniform Index | Name" << std::endl;
		for(int i=0; i < nUniforms; i++){
			glGetActiveUniform(m_shaderProgram_ID, i, maxLength, &written, &size, &type, name);
			location = glGetUniformLocation(m_shaderProgram_ID, name);
			std::cout << "unfiform " << location << " | " << name << std::endl;
		}
		free(name);
		m_activeUniformsWritten = !m_activeUniformsWritten;
	}
}


//!
/*!
 *
 * @return
 */
GLint ShaderProgram::GetHandle(void)
{
	return m_shaderProgram_ID;
}
