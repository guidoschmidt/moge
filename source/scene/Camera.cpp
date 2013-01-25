//! Camera.cpp
/*!
 * @date	05.01.2013
 * @author	Guido Schmidt
 */

#include "Camera.h"

namespace scene {

	//!
	/*!
	 *
	 */
	Camera::Camera()
	{
		m_speed = 0.005;

		glm::vec3 position(0.0f, 0.0f,1.0f);
		glm::vec3 lookAt(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);

		m_fieldOfView = 60.0f;
		m_aspect = CalculateAspect();
		m_nearPlane = 0.1f;
		m_farPlane = 100.0f;

		m_translationMatrix = glm::mat4(1.0f);
		m_rotationMatrix = glm::mat4(1.0f);

		m_viewMatrix = glm::lookAt(m_position, m_lookAt, m_up);
		m_projectionMatrix = glm::perspective(m_fieldOfView, m_aspect, m_nearPlane, m_farPlane);
	}

	//!
	/*!
	 *
	 * @param lookAtVec
	 * @param upVec
	 */
	Camera::Camera(glm::vec3 positionVec, glm::vec3 lookAtVec, glm::vec3 upVec)
	{
		m_speed = 0.005;

		m_position = positionVec;
		m_lookAt = lookAtVec;
		m_up = upVec;

		m_fieldOfView = 60.0f;
		m_aspect = CalculateAspect();
		m_nearPlane = 0.1f;
		m_farPlane = 100.f;

		m_translationMatrix = glm::mat4(1.0f);
		m_rotationMatrix = glm::mat4(1.0f);

		m_viewMatrix = glm::lookAt(m_position, m_lookAt, m_up);
		m_projectionMatrix = glm::perspective(m_fieldOfView, m_aspect, m_nearPlane, m_farPlane);
	}


	//!
	/*!
	 *
	 */
	Camera::~Camera() {
		// TODO Auto-generated destructor stub
	}

	//!
	/*!
	 *
	 * @return
	 */
	float Camera::CalculateAspect(void)
	{
		int w = Singleton<Context>::Instance()->GetWidth();
		int h = Singleton<Context>::Instance()->GetHeight();
		return (static_cast<float>(w))/(static_cast<float>(h));
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::mat4 Camera::GetViewMatrix(void)
	{
		return m_viewMatrix;
	}


	//!
	/*!
	 *
	 * @return
	 */
	glm::mat4 Camera::GetProjectionMatrix(void)
	{
		return m_projectionMatrix;
	}

} //! namespace scene
