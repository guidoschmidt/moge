//! Camera.cpp
/*!
 * @date	05.01.2013
 * @author	Guido Schmidt
 */

#include "Camera.h"

namespace scene {

	//! Empty constructor
	/*!
	 *
	 */
	Camera::Camera()
	{
		m_position = glm::vec3(0.0, 0.0, 1.0);
		m_lookAt = glm::vec3(0.0, 0.0, 0.0);
		m_up = glm::vec3(0.0, 1.0, 0.0);
		m_side = glm::vec3(1.0, 0.0, 0.0);

		SetSide(m_side);
		SetUp(m_up);
		SetLookAt(m_lookAt);
		SetPosition(m_position);

		m_fieldOfView = 70.0f;
		m_width 	  = Singleton<Context>::Instance()->GetWidth();
		m_height 	  = Singleton<Context>::Instance()->GetHeight();
		m_aspect 	  = static_cast<float>(m_width) / static_cast<float>(m_height);
		m_nearPlane   = 0.1f;
		m_farPlane 	  = 200.0f;

		m_viewMatrix	   	= glm::lookAt(m_position, m_lookAt, m_up);
		m_inverseViewMatrix = glm::inverse(m_viewMatrix);
		m_projectionMatrix 	= glm::perspective(m_fieldOfView, m_aspect, m_nearPlane, m_farPlane);
		m_isOrtho 		   	= false;

		m_speed = 0.05;
	}


	//! Initializing Constructor
	/*!
	 *
	 * @param lookAtVec
	 * @param upVec
	 */
	Camera::Camera(glm::vec3 positionVec, glm::vec3 lookAtVec, glm::vec3 upVec)
	{
		m_initPos = positionVec;
		m_position = positionVec;

		m_initLookAt = lookAtVec;
		m_lookAt = lookAtVec;

		m_initUp = upVec;
		m_up = upVec;

		m_lookVec 	= m_initLookAt - m_initPos;
		m_initSide 	= glm::cross(m_lookVec, m_initUp);
		m_side 		= glm::cross(m_lookVec, m_initUp);

		SetSide(m_side);
		SetUp(m_up);
		SetLookAt(m_lookAt);
		SetPosition(m_position);

		m_fieldOfView = 70.0f;
		m_width 	  = Singleton<Context>::Instance()->GetWidth();
		m_height 	  = Singleton<Context>::Instance()->GetHeight();
		m_aspect 	  = static_cast<float>(m_width) / static_cast<float>(m_height);
		m_nearPlane   = 0.1f;
		m_farPlane 	  = 200.f;

		m_viewMatrix	   	= glm::lookAt(m_position, m_lookAt, m_up);
		m_inverseViewMatrix = glm::inverse(m_viewMatrix);
		m_projectionMatrix  = glm::perspective(m_fieldOfView, m_aspect, m_nearPlane, m_farPlane);
		m_isOrtho 		    = false;

		m_speed = 0.05;
	}

	//! Destructor
	Camera::~Camera() {
		// TODO Auto-generated destructor stub
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::vec3 Camera::GetSide(void)
	{
		return glm::vec3(m_inverseViewMatrix[0]);
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::vec3 Camera::GetUp(void)
	{
		return glm::vec3(m_inverseViewMatrix[1]);
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::vec3 Camera::GetLookAt(void)
	{
		return glm::vec3(m_inverseViewMatrix[2]);
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::vec3 Camera::GetPosition(void)
	{
		return glm::vec3(m_inverseViewMatrix[3]);
	}

	//!
	/*!
	 *
	 * @return
	 */
	float Camera::GetFOV(void)
	{
		return m_fieldOfView;
	}

	//!
	/*!
	 *
	 * @return
	 */
	float Camera::GetAspectRatio(void)
	{
		return m_aspect;
	}


	//!
	/*!
	 *
	 * @return
	 */
	int Camera::GetWidth(void)
	{
		return m_width;
	}

	//!
	/*!
	 *
	 * @return
	 */
	int Camera::GetHeight(void)
	{
		return m_height;
	}

	//!
	/*!
	 *
	 * @return
	 */
	float Camera::GetNearPlane(void)
	{
		return m_nearPlane;
	}

	//!
	/*!
	 *
	 * @return
	 */
	float Camera::GetFarPlane(void)
	{
		return m_farPlane;
	}

	//!
	/*!
	 *
	 * @return
	 */
	bool Camera::IsOrtho(void)
	{
		return m_isOrtho;
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::mat4 Camera::GetViewMatrix(void)
	{
		return glm::inverse(m_inverseViewMatrix);
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

	//!
	/*!
	 *
	 * @param side
	 */
	void Camera::SetSide(const glm::vec3& side)
	{
		m_inverseViewMatrix[0] = glm::vec4(side, 1.0);
	}

	//!
	/*!
	 *
	 * @param up
	 */
	void Camera::SetUp(const glm::vec3 up)
	{
		m_inverseViewMatrix[1] = glm::vec4(up, 1.0);
	}

	//!
	/*!
	 *
	 * @param lookat
	 */
	void Camera::SetLookAt(const glm::vec3 lookat)
	{
		m_inverseViewMatrix[2] = glm::vec4(lookat, 1.0);
	}

	//!
	/*!
	 *
	 * @param pos
	 */
	void Camera::SetPosition(const glm::vec3 pos)
	{
		m_inverseViewMatrix[3] = glm::vec4(pos, 1.0);
	}

	//!
	/*!
	 *
	 * @param pos
	 * @param lookat
	 * @param up
	 */
	void Camera::SetOrientation(const glm::vec3 side, const glm::vec3 up, const glm::vec3 lookat)
	{
		 m_inverseViewMatrix[0] = glm::vec4(side, 0.0f);
		 m_inverseViewMatrix[1] = glm::vec4(up, 0.0f);
		 m_inverseViewMatrix[2] = glm::vec4(lookat, 0.0f);

		 SetPosition(GetPosition());
	}

	//!
	/*!
	 *
	 * @param fov
	 */
	void Camera::SetFOV(float fov)
	{
		m_fieldOfView = fov;
		if(m_isOrtho)
			m_projectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, m_nearPlane, m_farPlane);
		else
			m_projectionMatrix = glm::perspective(m_fieldOfView, m_aspect, m_nearPlane, m_farPlane);
	}

	//!
	/*!
	 *
	 * @param aspect
	 */
	void Camera::SetAspectRatio(float aspect)
	{
		m_aspect = aspect;
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
	 * @param isOrtho
	 */
	void Camera::SetOrtho(bool isOrtho)
	{
		m_isOrtho = isOrtho;
		if(m_isOrtho)
			m_projectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, m_nearPlane, m_farPlane);
	}

	//!
	/*!
	 *
	 * @param speed
	 */
	void Camera::MoveZ(float speed)
	{
		SetPosition(GetPosition() - GetLookAt() * speed);
	}

	//!
	/*!
	 *
	 * @param speed
	 */
	void Camera::MoveX(float speed)
	{
		SetPosition(GetPosition() + GetSide() * speed);
	}

	//!
	/*!
	 *
	 * @param speed
	 */
	void Camera::MoveY(float speed)
	{
		SetPosition(GetPosition() + GetUp() * speed);
	}

	//!
	/*!
	 *
	 * @param x
	 * @param y
	 */
	void Camera::Rotate(float x, float y)
	{
		RotateByQuat(x, glm::vec3(0.0f, 1.0f, 0.0f));
		RotateByQuat(y, GetSide());
	}

	//!
	/*!
	 *
	 * @param angle
	 * @param axis
	 */
	void Camera::RotateByQuat(const float angle, const glm::vec3 axis)
	{
		glm::vec3 view = GetLookAt();
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		glm::quat quatView(0, view);
		glm::quat quatViewResult = glm::rotate(quatView, angle, axis);
		view.x = quatViewResult.x;
		view.y = quatViewResult.y;
		view.z = quatViewResult.z;
		view    = glm::normalize(view);
		glm::vec3 side = glm::cross(up, view);
		side = glm::normalize(side);
		up = glm::cross(view, side);
		up = glm::normalize(up);
		SetOrientation(side, up, view);
	}

} //! namespace scene
