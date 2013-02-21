//! Light.cpp
/*!
 * @date	02.02.2013
 * @author	Guido Schmidt
 */

#include "Light.h"

namespace scene {

	//!
	Light::Light()
	{
		m_type = "Light";
		m_position = glm::vec3(0.0f);
		m_intensity = 1.0f;
		m_color = glm::vec3(1.0f);
		m_lightType = POINT;
		m_material_ptr = 0;
	}

	//!
	Light::Light(std::string name, glm::vec3 position, float intensity, glm::vec3 color, lighttype type)
	{
		m_name = name;
		m_type = "Light";
		m_position = position;
		m_intensity = intensity;
		m_color = color;
		m_lightType = type;
		m_material_ptr = 0;
	}

	//!
	Light::~Light()
	{
		// TODO Auto-generated destructor stub
	}

//! Getter

	//!
	float Light::GetIntensity(void)
	{
		return m_intensity;
	}

	//!
	glm::vec3 Light::GetColor(void)
	{
		return m_color;
	}

	//!
	std::string Light::GetName(void)
	{
		return m_name;
	}

	lighttype Light::GetLightType(void)
	{
		return m_lightType;
	}

	Material* Light::GetMaterial(void)
	{
		if(m_material_ptr != 0)
			return m_material_ptr;
		return 0;
	}

	GLuint* Light::GetTextureHandle()
	{
		return (Singleton<MaterialManager>::Instance()->GetTextureByID(m_material_ptr->GetTextureID(LIGHT)));
	}

//! Setter

	//!
	void Light::SetIntensity(float intensity)
	{
		m_intensity = intensity;
	}

	//!
	void Light::SetColor(glm::vec3 color)
	{
		m_color = color;
	}

	//!
	void Light::SetLightType(lighttype type)
	{
		m_lightType = type;
	}

	//! Set the mesh's material
	/*!
	 *
	 */
	void Light::SetMaterial(Material* material)
	{
		m_material_ptr = material;
	}

} //! namespace scene
