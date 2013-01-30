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
		m_texture_id = -1;
		m_normalmap_id = -1;

		//! Material properties
		m_reflectivity = 0.0f;
		glm::vec4 diffuseColor(0.8f);
		glm::vec4 specularColor(0.8f);

		texture_ptr = 0;
		normal_ptr = 0;
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
		m_texture_id = tex_id;
		m_normalmap_id = -1;

		//! Material properties
		m_reflectivity = 0;
		glm::vec4 diffuseColor(0.8f);
		glm::vec4 specularColor(0.8f);

		//! Texture properties
		texture_ptr = texture;
		normal_ptr = NULL;
	}


	//!
	/*!
	 *
	 */
	Material::Material(int mat_id, std::string mat_name, int tex_id, GLuint* texture, int nmap_id, GLuint* normalmap)
	{
		//! Identification
		id = mat_id;
		name = mat_name;
		m_texture_id = tex_id;
		m_normalmap_id = nmap_id;

		//! Material properties
		m_reflectivity = 0;
		glm::vec4 diffuseColor(0.8f);
		glm::vec4 specularColor(0.8f);

		//! Texture properties
		texture_ptr = texture;
		normal_ptr = normalmap;
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
	glm::vec4 Material::GetDiffuseColor(void){ return m_diffuseColor; }
	glm::vec4 Material::GetSpecularColor(void){ return m_specularColor; }
	float Material::GetReflectivity(void){ return m_reflectivity; }


	//!
	/*!
	 *
	 */
	void Material::SetDiffuseColor(glm::vec4 diffuse){ m_diffuseColor = diffuse; }
	void Material::SetSpecularColor(glm::vec4 specular){ m_specularColor = specular; }
	void Material::SetReflectivity(float reflection){ m_reflectivity= reflection; }


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
		return m_texture_id;
	}


	//!
	/*!
	 *
	 */
	void Material::SetTexture(GLuint* texture)
	{
		texture_ptr = texture;
	}


	//! Sets the material's normal map
	/*!
	 *
	 * @param texture
	 */
	void Material::SetNormalMap(GLuint* normaltex)
	{
		normal_ptr = normaltex;
		std::cout << "Normalmap set: " << &normal_ptr << std::endl;
	}


	//!
	/*!
	 *
	 * @param tex_id
	 */
	void Material::SetTextureID(int tex_id)
	{
		m_texture_id = tex_id;
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
