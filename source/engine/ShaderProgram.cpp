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
	ShaderProgram_ID = glCreateProgram();
	linked = false;
	ErrorCheckValue = glGetError();
	activeAttributesWritten = false;
	activeUniformsWritten = false;
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
	ShaderProgram_ID = glCreateProgram();
	activeAttributesWritten = false;
	activeUniformsWritten = false;

	AddShader(shaderType0, filename0);
	AddShader(shaderType1, filename1);
	glAttachShader(ShaderProgram_ID, Shader_IDS[0]);
	glAttachShader(ShaderProgram_ID, Shader_IDS[1]);
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

	Shader_IDS.push_back(Shader_ID);
}


//! Links the shader program
/*!
 * Links the shader program.
 */
void ShaderProgram::Link(void)
{
	glLinkProgram(ShaderProgram_ID);

	GLint linkStatus;
	glGetProgramiv(ShaderProgram_ID, GL_LINK_STATUS, &linkStatus);
	if(linkStatus == GL_FALSE){
		std::cout << "ERROR | ShaderProgram: shader program linking failed!" << std::endl;
		GLSL::PrintShaderProgramInfoLog(ShaderProgram_ID);
		linked = false;
	}
	else{
		std::cout << "ShaderProgram: shader program linking succeded!" << std::endl;
		linked = true;
	}
}

//! Get shader program's link status
bool ShaderProgram::IsLinked(void)
{
	return linked;
}

//! Activates the shader program
void ShaderProgram::Use(void)
{
	glUseProgram(ShaderProgram_ID);
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
	GLint glslMatrixID = glGetUniformLocation(ShaderProgram_ID, name.c_str());
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
	GLint glslVectorID = glGetUniformLocation(ShaderProgram_ID, name.c_str());
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
	GLint glslVectorID = glGetUniformLocation(ShaderProgram_ID, name.c_str());
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
	GLint glslIntID = glGetUniformLocation(ShaderProgram_ID, name.c_str());
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
	GLint glslFloatID = glGetUniformLocation(ShaderProgram_ID, name.c_str());
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
	int loc = glGetUniformLocation(ShaderProgram_ID, name.c_str());
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
	glBindAttribLocation(ShaderProgram_ID, location, name.c_str());
}

//!
/*!
 *
 */
void ShaderProgram::PrintActiveAttributes(void)
{
	if(!activeAttributesWritten){
		GLint maxLength, nAttributes;
		glGetProgramiv(ShaderProgram_ID, GL_ACTIVE_ATTRIBUTES, &nAttributes);
		glGetProgramiv(ShaderProgram_ID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

		GLchar* name = (GLchar*) malloc(maxLength);
		GLint written, size, location;
		GLenum type;

		std::cout << "-----------------------------------------------------" << std::endl;
		std::cout << "Attribute Index | Name" << std::endl;
		for(int i=0; i < nAttributes; i++){
			glGetActiveAttrib(ShaderProgram_ID, i, maxLength, &written, &size, &type, name);
			location = glGetAttribLocation(ShaderProgram_ID, name);
			std::cout << location << " | " << name << std::endl;
		}
		free(name);
		activeAttributesWritten = !activeAttributesWritten;
	}
}

//!
/*!
 *
 */
void ShaderProgram::PrintActiveUniforms(void)
{
	if(!activeUniformsWritten){
		GLint maxLength, nUniforms;
		glGetProgramiv(ShaderProgram_ID, GL_ACTIVE_UNIFORMS, &nUniforms);
		glGetProgramiv(ShaderProgram_ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

		GLchar* name = (GLchar*) malloc(maxLength);
		GLint size, location;
		GLsizei written;
		GLenum type;
		std::cout << "-----------------------------------------------------" << std::endl;
		std::cout << "Uniform Index | Name" << std::endl;
		for(int i=0; i < nUniforms; i++){
			glGetActiveUniform(ShaderProgram_ID, i, maxLength, &written, &size, &type, name);
			location = glGetUniformLocation(ShaderProgram_ID, name);
			std::cout << "unfiform " << location << " | " << name << std::endl;
		}
		free(name);
		activeUniformsWritten = !activeUniformsWritten;
	}
}


//!
/*!
 *
 * @return
 */
GLint ShaderProgram::GetHandle(void)
{
	return ShaderProgram_ID;
}
