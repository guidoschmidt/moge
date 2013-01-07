//! Camera.h
/*!
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

namespace scene {

	class Camera: public scene::Node {

		private:
			double speed;

			glm::vec3 lookAt;
			glm::vec3 up;

			float fieldOfView;
			float clipPlaneNear, clipPlaneFar;

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
	};

} //! namespace scene
#endif //! CAMERA_H_
