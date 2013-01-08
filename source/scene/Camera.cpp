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
		speed = 0.005;

		glm::vec3 position(0.0f, 0.0f,1.0f);
		glm::vec3 lookAt(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);

		fieldOfView = 60.0f;
		clipPlaneFar = 100.0f;
		clipPlaneNear = 0.1f;
	}


	//!
	/*!
	 *
	 * @param lookAtVec
	 * @param upVec
	 */
	Camera::Camera(glm::vec3 positionVec, glm::vec3 lookAtVec, glm::vec3 upVec)
	{
		speed = 0.005;

		position = positionVec;
		lookAt = lookAtVec;
		up = upVec;

		fieldOfView = 60.0f;
		clipPlaneFar = 100.0f;
		clipPlaneNear = 0.1f;
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
	glm::mat4 Camera::GetViewMatrix(void)
	{
		return glm::lookAt(position, lookAt, up);
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::mat4 Camera::GetProjectionMatrix(void)
	{
		//! TODO Aspect ratio
		return glm::perspective(60.0f, 1.0f, 0.01f, 100.0f);
	}


	//! TODO Overload
	void Camera::TranslateX(float dx)
	{
		position.x += dx;
		lookAt.x += dx;
	}

	//!
	void Camera::TranslateY(float dy)
	{
		position.y += dy;
		lookAt.y += dy;
	}

	//!
	void Camera::TranslateZ(float dz)
	{
		position.z += dz;
		lookAt.z += dz;
	}

	void Camera::SetCameraPositionX(float x)
	{
		position.x = x;
	}
	void Camera::SetCameraPositionY(float y)
	{
		position.y = y;
	}
	void Camera::SetCameraPositionZ(float z)
	{
		position.z = z;
	}

} //! namespace scene
