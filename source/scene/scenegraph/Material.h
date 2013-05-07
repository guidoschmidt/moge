//! Material.h
/*!
 * @date	03.01.2013
 * @author	Guido Schmidt
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

//! C++ includes
#include <string>
#include <vector>
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
	//! Enumeration for possible texture types
	typedef enum {
		DIFFUSE = 0,
		NORMAL = 1,
		CUBEMAP = 2,
		LIGHT = 3
	} texturetype;

	struct textureRef{
		texturetype m_type;
		int m_id;

		textureRef(int id, texturetype type)
		{
			m_id = id;
			m_type = type;
		}
	};

	class Material {
		private:
			//! Identification
			int m_id;
			std::string m_name;

			//! Texture
			std::vector<textureRef> m_texture_ids;

			//! Material properties
			glm::vec3 m_diffuseColor;
			glm::vec3 m_specularColor;
			float m_reflectance;

		public:
			Material(int mat_id, std::string);
			Material(int mat_id, std::string mat_name, int tex_id, GLuint* texture);
			Material(int mat_id, std::string mat_name, int tex_id, GLuint* texture, int nmap_id, GLuint* normalmap);
			virtual ~Material();

			//! Material
			//! Getter
			int GetMaterialID(void);
			int GetTextureID(texturetype type);
			std::string GetName(void);
			glm::vec3 GetDiffuseColor(void);
			glm::vec3 GetSpecularColor(void);
			float GetReflectivity(void);
			//! Setter
			void SetID(int mat_id);
			void SetName(std::string mat_name);
			void SetDiffuseColor(glm::vec3 diffuse);
			void SetSpecularColor(glm::vec3 specular);
			void SetReflectance(float reflection);
			//! Functions
			void AddTexture(textureRef tex);
			bool HasTexture(void);
	};

} //! namespace scene
#endif //! MATERIAL_H_
