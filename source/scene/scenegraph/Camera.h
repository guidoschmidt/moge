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
#include <glm/core/type_gentype.hpp>
#include <glm/gtx/quaternion.hpp>
//! Local includes
#include "../../utilities/Singleton.h"
#include "../../engine/Context.h"
#include "Node.h"

namespace scene {

	class Camera: public scene::Node {

		private:
			//! View
			glm::vec3 m_lookVec;
			glm::vec3 m_lookAt;
			glm::vec3 m_up;
			glm::vec3 m_side;
			//! Perspective
			float m_fieldOfView;
			float m_aspect;
			float m_farPlane, m_nearPlane;
			//! Matrices
			glm::mat4 m_viewMatrix, m_projectionMatrix;
			glm::quat m_qOrientation;
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
			glm::vec3 GetLookAt(void);
			float GetFieldOfView(void);
			float GetNearPlane(void);
			float GetFarPlane(void);
			glm::vec3 GetPosition(void);
			glm::mat4 GetViewMatrix(void);
			glm::mat4 GetProjectionMatrix(void);

			//! Setter
			void SetLookAt(glm::vec3 lookAt);
			void SetFielOfView(float angle);
			void SetNearPlane(float near);
			void SetFarPlane(float far);

			//! Movement
			void Move(float x, float y, float z);
			void Pan(float x, float y, float z);
			void Yaw(float angle);
			void Pitch(float angle);
			void Reset(void);
	};

} //! namespace scene
#endif //! CAMERA_H_
