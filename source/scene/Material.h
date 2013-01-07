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
			//! Texture
			GLuint* texture_ptr;
			//! Material properties
			glm::vec4 diffuseColor;
			glm::vec4 specularColor;
			float reflectivity;

		public:
			Material();
			virtual ~Material();

			//! Texture
			void LoadTexture(const std::string filename);
			GLuint GetTexture(void);

			//! Material
			// Getter
			glm::vec4 GetDiffuseColor(void);
			glm::vec4 GetSpecularColor(void);
			float GetReflectivity(void);
			// Setter
			void SetDiffuseColor(glm::vec4 diffuse);
			void SetSpecularColor(glm::vec4 specular);
			void SetReflectivity(float reflection);

			void Use(void);
	};

} //! namespace scene
#endif //! MATERIAL_H_
