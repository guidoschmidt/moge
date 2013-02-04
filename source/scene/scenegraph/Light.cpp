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
	}

	//!
	Light::Light(glm::vec3 position, float intensity, glm::vec3 color, lighttype type)
	{
		m_type = "Light";
		m_position = position;
		m_intensity = intensity;
		m_color = color;
		m_lightType = type;
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

	lighttype Light::GetLightType(void)
	{
		return m_lightType;
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

} //! namespace scene
