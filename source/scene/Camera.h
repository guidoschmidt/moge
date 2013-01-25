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
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
//! Local includes
#include "Node.h"
#include "../utilities/Singleton.h"
#include "../engine/Context.h"
#include <AntTweakBar.h>

namespace scene {

	class Camera: public scene::Node {

		private:
			double m_speed;
			//! View
			glm::vec3 m_lookAt;
			glm::vec3 m_up;
			//! Perspective
			float m_fieldOfView;
			float m_aspect;
			float m_farPlane, m_nearPlane;
			//! Matrices
			glm::mat4 m_viewMatrix, m_projectionMatrix;
			glm::mat4 m_rotationMatrix, m_translationMatrix;

		protected:
			float CalculateAspect(void);
			void UpdateViewMatrix(void);
			void UpdateProjectionMatrix(void);

		public:
			Camera();
			Camera(glm::vec3 posVec, glm::vec3 lookAtVec, glm::vec3 upVec);
			virtual ~Camera();

			//! Getter
			float GetFieldOfView(void);
			float GetNearPlane(void);
			float GetFarPlane(void);
			glm::vec3 GetPosition(void);
			glm::mat4 GetViewMatrix(void);
			glm::mat4 GetProjectionMatrix(void);

			//! Setter
			void SetFielOfView(float angle);
			void SetNearPlane(float near);
			void SetFarPlane(float far);

			//! Movement
	};

} //! namespace scene
#endif //! CAMERA_H_
