//! Camera.h
/*!
 * Camera class
 *
 * @date 	05.01.2013
 * @author	Guido Schmidt
 */

#ifndef CAMERA_H_
#define CAMERA_H_

//! GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//! Local includes
#include "Node.h"
#include "../utilities/Singleton.h"
#include "../engine/Context.h"
#include <AntTweakBar.h>

namespace scene {

	class Camera: public scene::Node {

		private:
			double m_speed;

			glm::vec3 m_lookAt;
			glm::vec3 m_up;

			float m_fieldOfView;
			float m_aspect;
			float m_farPlane, m_nearPlane;

			glm::mat4 m_viewMatrix, m_projectionMatrix;

		protected:
			float CalculateAspect(void);

		public:
			Camera();
			Camera(glm::vec3 posVec, glm::vec3 lookAtVec, glm::vec3 upVec);
			virtual ~Camera();

			glm::mat4 GetViewMatrix(void);
			glm::mat4 GetProjectionMatrix(void);
			glm::mat4 GetViewProjectionMatrix(void);

			void TranslateX(float dx);
			void TranslateY(float dy);
			void TranslateZ(float dz);

			void SetCameraPositionX(float x);
			void SetCameraPositionY(float y);
			void SetCameraPositionZ(float z);

			//! Setter
			void SetFielOfView(float angle);
			void SetNearPlane(float near);
			void SetFarPlane(float far);
	};

} //! namespace scene
#endif //! CAMERA_H_
