//! Material.cpp
/*!
 * @date	03.01.2013
 * @author	Guido Schmidt
 */

#include "Material.h"

namespace scene {

	//!
	/*!
	 *
	 */
	Material::Material() {
		texture_ptr = 0;
		reflectivity = 0;
		glm::vec4 diffuseColor(0.8f);
		glm::vec4 specularColor(0.8f);
	}


	//!
	/*!
	 *
	 */
	Material::Material(GLuint* texture) {
		texture_ptr = texture;
		reflectivity = 0;
		glm::vec4 diffuseColor(0.8f);
		glm::vec4 specularColor(0.8f);
	}


	//!
	/*!
	 *
	 */
	Material::~Material() {
		// TODO Auto-generated destructor stub
	}


	//! Returns the texture handle
	/*!
	 *
	 * @return
	 */
	GLuint Material::GetTexture(void)
	{
		return *texture_ptr;
	}


	//!
	/*!
	 *
	 */
	inline glm::vec4 Material::GetDiffuseColor(void){ return diffuseColor; }
	inline glm::vec4 Material::GetSpecularColor(void){ return specularColor; }
	inline float Material::GetReflectivity(void){ return reflectivity; }


	//!
	/*!
	 *
	 */
	void Material::SetDiffuseColor(glm::vec4 diffuse){ diffuseColor = diffuse; }
	void Material::SetSpecularColor(glm::vec4 specular){ specularColor = specular; }
	void Material::SetReflectivity(float reflection){ reflectivity= reflection; }


	//!
	/*!
	 *
	 */
	void Material::SetTexturePointer(GLuint* texture)
	{
		texture_ptr = texture;
	}

} //! namespace scene
