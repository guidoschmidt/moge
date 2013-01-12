//! Light.cpp
/*!
 * @date	11.01.2013
 * @author	Guido Schmidt
 */

#include "Light.h"

namespace scene {

	//!
	/*!
	 *
	 */
	Light::Light(glm::vec3 position, glm::vec3 color, float intensity)
	{
		//! base class members
		m_position = position;
		//! members
		m_lightColor = color;
		m_intensity = intensity;
	}


	//!
	/*!
	 *
	 */
	Light::~Light() {
		// TODO Auto-generated destructor stub
	}


	//!
	/*!
	 *
	 */
	glm::vec3 Light::GetColor(void)
	{
		return m_lightColor;
	}


	//!
	/*!
	 *
	 */
	float Light::GetIntensity(void)
	{
		return m_intensity;
	}


	//!
	/*!
	 *
	 */
	void Light::SetColor(glm::vec3 color)
	{
		m_lightColor = color;
	}


	//!
	/*!
	 *
	 */
	void Light::SetIntensity(float intensity)
	{
		m_intensity = intensity;
	}

} //! namespace scene */
