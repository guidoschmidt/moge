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
			//! Inital Values
			glm::vec3 m_initPos;
			glm::vec3 m_initLookAt;
			glm::vec3 m_initUp;
			glm::vec3 m_initSide;

			//! View
			glm::vec3 m_lookVec;
			glm::vec3 m_lookAt;
			glm::vec3 m_up;
			glm::vec3 m_side;

			//! Perspective
			float m_fieldOfView;
			float m_aspect;
			int m_width, m_height;
			float m_farPlane, m_nearPlane;
			bool m_isOrtho;

			//! Matrices
			glm::mat4 m_viewMatrix, m_inverseViewMatrix, m_projectionMatrix;

			//! Movement
			float m_speed;

		protected:
			void Changed(void);
			void RotateByQuat(const float angle, const glm::vec3 axis);

		public:
			Camera();
			Camera(glm::vec3 posVec, glm::vec3 lookAtVec, glm::vec3 upVec);
			virtual ~Camera();

			//! Getter
			glm::vec3 GetSide(void);
			glm::vec3 GetUp(void);
			glm::vec3 GetLookAt(void);
			glm::vec3 GetPosition(void);

			float GetFOV(void);
			float GetAspectRatio(void);
			int GetWidth(void);
			int GetHeight(void);
			float GetNearPlane(void);
			float GetFarPlane(void);

			bool IsOrtho(void);

			glm::mat4 GetViewMatrix(void);
			glm::mat4 GetProjectionMatrix(void);

			//! Setter
			void SetSide(const glm::vec3& side);
			void SetUp(const glm::vec3 up);
			void SetLookAt(const glm::vec3 lookat);
			void SetPosition(const glm::vec3 pos);
			void SetOrientation(const glm::vec3 side, const glm::vec3 up, const glm::vec3 lookat);

			void SetFOV(float fov);
			void SetAspectRatio(float aspect);
			void SetNearPlane(float near);
			void SetFarPlane(float far);

			void SetOrtho(bool isOrtho);

			//! Movement
			void MoveZ(float speed);
			void MoveX(float speed);
			void MoveY(float speed);
			void Rotate(float x, float y);
	};

} //! namespace scene
#endif //! CAMERA_H_
