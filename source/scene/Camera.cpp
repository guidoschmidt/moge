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
	void Camera::RotateX(float angle){ m_viewMatrix = glm::rotate(angle, 1.0f, 0.0f, 0.0f); }
	void Camera::RotateY(float angle){}
	void Camera::RotateZ(float angle){}

	//!
	glm::mat4 Camera::GetCameraToClipMatrix(void)
	{

		glm::mat4 cameraToClipMatrix = glm::mat4(0.0f);

		const float degToRad = 3.14159f / 180.0f;
		float viewAngleRad = m_fieldOfView * degToRad;
		float frustumScale = 1.0f / tan(viewAngleRad / 2.0f);

		cameraToClipMatrix[0].x = frustumScale;
		cameraToClipMatrix[1].y = frustumScale;
		cameraToClipMatrix[2].z = (m_farPlane + m_nearPlane)/(m_nearPlane - m_farPlane);
		cameraToClipMatrix[2].w = -1.0f;
		cameraToClipMatrix[3].z = (2.0f * m_farPlane * m_nearPlane)/(m_nearPlane - m_farPlane);

		return cameraToClipMatrix;
	}


	//!
	/*!
	 *
	 * @param x
	 */
	void Camera::SetCameraPositionX(float x)
	{
		m_position.x = x;
	}

	//!
	/*!
	 *
	 * @param y
	 */
	void Camera::SetCameraPositionY(float y)
	{
		m_position.y = y;
	}

	//!
	/*!
	 *
	 * @param z
	 */
	void Camera::SetCameraPositionZ(float z)
	{
		m_position.z = z;
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

} //! namespace scene
