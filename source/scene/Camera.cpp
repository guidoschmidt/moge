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
		m_nearPlane = 20.0f;
		m_farPlane = 0.1f;
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
		m_nearPlane = 20.0f;
		m_farPlane = 0.1f;
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
		return glm::lookAt(m_position, m_lookAt, m_up);
	}


	//!
	/*!
	 *
	 * @return
	 */
	glm::mat4 Camera::GetProjectionMatrix(void)
	{
		return glm::perspective(m_fieldOfView, m_aspect, m_farPlane, m_nearPlane);
	}


	//!
	/*!
	 *
	 * @return
	 */
	glm::mat4 Camera::GetViewProjectionMatrix(void)
	{
		m_viewMatrix = glm::lookAt(m_position, m_lookAt, m_up);
		m_projectionMatrix = glm::perspective(m_fieldOfView, m_aspect, m_farPlane, m_nearPlane);
		return m_viewMatrix * m_projectionMatrix;
	}

	//!
	void Camera::TranslateX(float dx)
	{
		m_position.x += dx;
		m_lookAt.x += dx;
	}

	//!
	void Camera::TranslateY(float dy)
	{
		m_position.y += dy;
		m_lookAt.y += dy;
	}

	//!
	void Camera::TranslateZ(float dz)
	{
		m_position.z += dz;
		m_lookAt.z += dz;
	}

	//!
	void Camera::SetCameraPositionX(float x)
	{
		m_position.x = x;
	}

	//!
	void Camera::SetCameraPositionY(float y)
	{
		m_position.y = y;
	}

	//!
	void Camera::SetCameraPositionZ(float z)
	{
		m_position.z = z;
	}

	//!
	void Camera::SetFielOfView(float angle)
	{
		m_fieldOfView = angle;
	}

	//!
	void Camera::SetNearPlane(float near)
	{
		m_nearPlane = near;
	}

	//!
	void Camera::SetFarPlane(float far)
	{
		m_farPlane = far;
	}

} //! namespace scene
