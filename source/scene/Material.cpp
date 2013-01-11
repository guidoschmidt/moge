//! Material.cpp
/*!
 * @date	03.01.2013
 * @author	Guido Schmidt
 */

#include "Material.h"

namespace scene {


	Material::Material(int mat_id, std::string mat_name)
	{
		//! Identification
		id = mat_id;
		name = mat_name;
		texture_id = -1;

		//! Material properties
		reflectivity = 0;
		glm::vec4 diffuseColor(0.8f);
		glm::vec4 specularColor(0.8f);

		texture_ptr = 0;
	}

	//!
	/*!
	 *
	 */
	Material::Material(int mat_id, std::string mat_name, int tex_id, GLuint* texture)
	{
		//! Identification
		id = mat_id;
		name = mat_name;
		texture_id = tex_id;

		//! Material properties
		reflectivity = 0;
		glm::vec4 diffuseColor(0.8f);
		glm::vec4 specularColor(0.8f);

		//! Texture properties
		texture_ptr = texture;
	}


	//!
	/*!
	 *
	 */
	Material::~Material() {
		// TODO Auto-generated destructor stub
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


	//! Returns the texture handle
	/*!
	 *
	 * @return
	 */
	GLuint Material::GetTexture(void)
	{
		return *texture_ptr;
	}


	//! Returns the texture handle
	/*!
	 *
	 * @return
	 */
	int Material::GetTextureID(void)
	{
		return texture_id;
	}


	//!
	/*!
	 *
	 */
	void Material::SetTexturePointer(GLuint* texture)
	{
		texture_ptr = texture;
	}


	//!
	/*!
	 *
	 * @param tex_id
	 */
	void Material::SetTextureID(int tex_id)
	{
		texture_id = tex_id;
	}


	//!
	/*!
	 *
	 * @return
	 */
	int Material::GetID(void)
	{
		return id;
	}


	//!
	/*!
	 *
	 * @return
	 */
	std::string Material::GetName(void)
	{
		return name;
	}


	//!
	/*!
	 *
	 * @return
	 */
	bool Material::HasTexture(void)
	{
		if(texture_ptr != 0)
			return true;
		return false;
	}

} //! namespace scene
