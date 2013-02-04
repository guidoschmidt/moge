//! Material.cpp
/*!
 * @date	03.01.2013
 * @author	Guido Schmidt
 */

#include "Material.h"

namespace scene {

	//! Constructor
	Material::Material(int mat_id, std::string mat_name)
	{
		//! Identification
		m_id = mat_id;
		m_name = mat_name;

		//! Properties
		m_reflectance = 0.0f;
		glm::vec4 diffuseColor(0.8f);
		glm::vec4 specularColor(0.8f);
	}

	//!
	/*!
	 *
	 */
	Material::Material(int mat_id, std::string mat_name, int tex_id, GLuint* texture)
	{
		//! Identification
		m_id = mat_id;
		m_name = mat_name;

		//! Material properties
		m_reflectance = 0;
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


	//!
	/*!
	 *
	 */
	glm::vec4 Material::GetDiffuseColor(void){ return m_diffuseColor; }
	glm::vec4 Material::GetSpecularColor(void){ return m_specularColor; }
	float Material::GetReflectivity(void){ return m_reflectance; }


	//!
	/*!
	 *
	 */
	void Material::SetDiffuseColor(glm::vec4 diffuse){ m_diffuseColor = diffuse; }
	void Material::SetSpecularColor(glm::vec4 specular){ m_specularColor = specular; }
	void Material::SetReflectance(float reflection){ m_reflectance= reflection; }


	//! Returns the texture handle
	/*!
	 *
	 * @return
	 */
	/*GLuint* Material::GetTexture(void)
	{
		return Singleton<scene::MaterialManager>::Instance()->GetTexture(0);
	}*/


	//!
	/*!
	 *
	 * @return
	 */
	int Material::GetMaterialID(void)
	{
		return m_id;
	}


	//!
	/*!
	 *
	 * @return
	 */
	std::string Material::GetName(void)
	{
		return m_name;
	}

	//!
	/*!
	 *
	 * @return
	 */
	bool Material::HasTexture(void)
	{
		return !(m_texture_ids.empty());
	}

	//! Adds a texture to the material
	void Material::AddTexture(textureRef tex)
	{
		m_texture_ids.push_back(tex);
	}

	//! Returns the id of the texture in the material manager
	int Material::GetTextureID(texturetype type)
	{
		int result = 0;
		for(unsigned int i=0; i < m_texture_ids.size(); i++)
		{
			if(m_texture_ids[i].m_type == type)
			{
				result = m_texture_ids[i].m_id;
			}
		}
		return result;
	}

} //! namespace scene
