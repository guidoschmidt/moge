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

} //! namespace scene
