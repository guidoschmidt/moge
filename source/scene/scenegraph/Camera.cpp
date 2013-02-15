//! Camera.cpp
/*!
 * @date	05.01.2013
 * @author	Guido Schmidt
 */

#include "Camera.h"

namespace scene {

	bool written = false;

	//! Empty constructor
	/*!
	 *
	 */
	Camera::Camera()
	{
		m_position = glm::vec3(0.0f, 0.0f,1.0f);
		m_lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
		m_up = glm::vec3(0.0f, 1.0f, 0.0f);
		m_lookVec = m_lookAt - m_position;
		m_side = glm::cross(m_lookVec, m_up);

		m_fieldOfView = 60.0f;
		m_aspect = CalculateAspect();
		m_nearPlane = 0.1f;
		m_farPlane = 500.0f;

		m_translationMatrix = glm::mat4(1.0f);
		m_rotationMatrix = glm::mat4(1.0f);

		m_viewMatrix = glm::lookAt(m_position, m_lookAt, m_up);
		m_projectionMatrix = glm::perspective(m_fieldOfView, m_aspect, m_nearPlane, m_farPlane);
	}

	//! Initializing Constructor
	/*!
	 *
	 * @param lookAtVec
	 * @param upVec
	 */
	Camera::Camera(glm::vec3 positionVec, glm::vec3 lookAtVec, glm::vec3 upVec)
	{
		m_position = positionVec;
		m_lookAt = lookAtVec;
		m_up = upVec;
		glm::vec3 lookVec = m_lookAt - m_position;
		m_side = glm::cross(lookVec, m_up);

		m_fieldOfView = 60.0f;
		m_aspect = CalculateAspect();
		m_nearPlane = 0.1f;
		m_farPlane = 500.f;

		m_translationMatrix = glm::mat4(1.0f);
		m_qOrientation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
		m_rotationMatrix = glm::toMat4(m_qOrientation);

		m_viewMatrix = glm::lookAt(m_position, m_lookAt, m_up);
		m_projectionMatrix = glm::perspective(m_fieldOfView, m_aspect, m_nearPlane, m_farPlane);
	}


	//! Destructor
	Camera::~Camera() {
		// TODO Auto-generated destructor stub
	}

	//! Returns the aspect ratio of the camera
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
	 */
	void Camera::UpdateViewMatrix(void)
	{
	}

	//void UpdateProjectionMatrix(void);

/* GETTER ***********************************************************/
	//! Returns the field of view
	/*!
	 * @return
	 */
	float Camera::GetFieldOfView(void)
	{
		return m_fieldOfView;
	}

	//! Returns the near plane
	float Camera::GetNearPlane(void)
	{
		return m_nearPlane;
	}

	//! Returns the far plane
	float Camera::GetFarPlane(void)
	{
		return m_farPlane;
	}

	//!
	glm::vec3 Camera::GetPosition(void)
	{
		return m_position;
	}

	//! Returns the view matrix
	/*!
	 * @return
	 */
	glm::mat4 Camera::GetViewMatrix(void)
	{
		return m_viewMatrix;
	}


	//! Returns the projection matrix
	/*!
	 * @return
	 */
	glm::mat4 Camera::GetProjectionMatrix(void)
	{
		return m_projectionMatrix;
	}

/* SETTER ***********************************************************/
	//! Sets the field of view
	/*!
	 * @param angle
	 */
	void Camera::SetFielOfView(float angle)
	{
		m_fieldOfView = angle;
		m_projectionMatrix = glm::perspective(m_fieldOfView, m_aspect, m_nearPlane, m_farPlane);
	}

	//! Sets the field of view
	/*!
	 * @param near
	 */
	void Camera::SetNearPlane(float near)
	{
		m_nearPlane = near;
	}

	//! Sets the field of view
	/*!
	 * @param near
	 */
	void Camera::SetFarPlane(float far)
	{
		m_farPlane = far;
	}

/* MOVEMENT ***********************************************************/
	//!
	void Camera::Move(float x, float y, float z)
	{
		m_position += glm::vec3(x, y, z);
		//m_lookAt += glm::vec3(x, y, z);

		//! TODO movement of camera
		//m_translationMatrix = glm::translate(m_position);
		m_viewMatrix = glm::lookAt(m_position, m_lookAt, m_up);
	}

	//! Yaw is rotation around up vector
	void Camera::Yaw(float angle)
	{
		if(angle <= 180.0f && angle >= -180.0f)
		{
			float radAngle = cosf(angle/2.0f);

			glm::vec3 qAxis = glm::normalize(m_up);
			qAxis = qAxis * (sinf(angle/2.0f));

			m_qOrientation = glm::quat(radAngle, qAxis);
			m_rotationMatrix = glm::toMat4(m_qOrientation);

			//m_viewMatrix = m_viewMatrix * m_rotationMatrix;
		}
	}

	//! Pitch is rotation around side vector
	void Camera::Pitch(float angle)
	{
		if(angle <= 180.0f && angle >= -180.0f)
		{
			float radAngle = cosf(angle/2.0f);

			glm::vec3 qAxis = glm::normalize(m_side);
			qAxis = qAxis * (sinf(angle/2.0f));

			m_qOrientation = glm::quat(radAngle, qAxis);
			m_rotationMatrix = glm::toMat4(m_qOrientation);

			//m_viewMatrix = m_viewMatrix * m_rotationMatrix;
		}
	}

	//! Resets the camera to an inital position
	void Camera::Reset(void)
	{
		m_position = glm::vec3(0.0f, 25.0f, 50.0f);
		m_lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
		m_up = glm::vec3(0.0f, 1.0f, 0.0f);

		m_viewMatrix = glm::lookAt(m_position, m_lookAt, m_up);
	}

} //! namespace scene
