//! Material.h
/*!
 * @date	03.01.2013
 * @author	Guido Schmidt
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

//! C++ includes
#include <string>
#include <iostream>
//! OpenGL includes
#include <GL/glew.h>
#include <GL/gl.h>
//! DevIL includes
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
//! GLM includes
#include <glm/glm.hpp>

namespace scene {

	class Material {
		private:
			//! Identification
			int id;
			std::string name;

			//! Texture
			GLuint* texture_ptr;
			int texture_id;

			//! Material properties
			glm::vec4 diffuseColor;
			glm::vec4 specularColor;
			float reflectivity;

		public:
			Material(int mat_id, std::string);
			Material(int mat_id, std::string mat_name, int tex_id, GLuint* texture);
			virtual ~Material();

			//! Material
			// Getter
			int GetID(void);
			std::string GetName(void);
			glm::vec4 GetDiffuseColor(void);
			glm::vec4 GetSpecularColor(void);
			float GetReflectivity(void);
			// Setter
			void SetID(int mat_id);
			void SetName(std::string mat_name);
			void SetDiffuseColor(glm::vec4 diffuse);
			void SetSpecularColor(glm::vec4 specular);
			void SetReflectivity(float reflection);

			//! Texture
			// Getter
			GLuint GetTexture(void);
			int GetTextureID(void);

			// Setter
			void SetTexturePointer(GLuint* texture);
			void SetTextureID(int tex_id);
			// Check texture
			bool HasTexture(void);
	};

} //! namespace scene
#endif //! MATERIAL_H_
