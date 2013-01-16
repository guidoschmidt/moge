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

		m_viewMatrix = glm::lookAt(m_position, m_lookAt, m_up);
		m_projectionMatrix = glm::perspective(m_fieldOfView, m_aspect, m_farPlane, m_nearPlane);
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

		m_viewMatrix = glm::lookAt(m_position, m_lookAt, m_up);
		m_projectionMatrix = glm::perspective(m_fieldOfView, m_aspect, m_farPlane, m_nearPlane);
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
		UpdateViewMatrix();
		return m_viewMatrix;
	}


	//!
	/*!
	 *
	 * @return
	 */
	glm::mat4 Camera::GetProjectionMatrix(void)
	{
		UpdateProjectionMatrix();
		return m_projectionMatrix;
	}

	//! Updates the camera's view matrix
	/*!
	 *
	 */
	void Camera::UpdateViewMatrix(void)
	{
		m_viewMatrix = rotation * glm::lookAt(m_position, m_lookAt, m_up);
	}

	//!
	void Camera::UpdateProjectionMatrix(void)
	{
		m_projectionMatrix = glm::perspective(m_fieldOfView, m_aspect, m_nearPlane, m_farPlane);
	}


	//!
	/*!
	 *
	 * @param dz
	 */
	void Camera::Translate(float dx, float dy, float dz)
	{
		m_position.x += dx;
		m_position.y += dy;
		m_position.z += dz;
		m_lookAt.x += dx;
		m_lookAt.y += dy;
		m_lookAt.z += dz;
	}

	//!
	glm::mat4 Camera::GetCameraToClipMatrix(void)
	{
		//! TODO return Cmaera to clip matrix
		return glm::mat4(1.0f);
	}


	//!
	/*!
	 *
	 * @param angle
	 */
	void Camera::SetFielOfView(float angle)
	{
		m_fieldOfView = angle;
	}

	//!
	/*!
	 *
	 * @param near
	 */
	void Camera::SetNearPlane(float near)
	{
		m_nearPlane = near;
	}

	//!
	/*!
	 *
	 * @param far
	 */
	void Camera::SetFarPlane(float far)
	{
		m_farPlane = far;
	}

	//!
	/*!
	 *
	 * @return
	 */
	float Camera::GetFieldOfView(void){ return m_fieldOfView; }

	//!
	/*!
	 *
	 * @return
	 */
	float Camera::GetFarPlane(void){ return m_farPlane; }

	//!
	/*!
	 *
	 * @return
	 */
	float Camera::GetNearPlane(void){ return m_nearPlane; }


	//!
	void Camera::Yaw(float angle)
	{
	}
	//!
	void Camera::Roll(float angle)
	{
	}

	//!
	void Camera::Pitch(float angle)
	{
	}


} //! namespace scene
