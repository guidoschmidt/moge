/*
 * Transformation.h
 *
 *  Created on: 02.01.2013
 *      Author: guidoschmidt
 */

#ifndef TRANSFORMATION_H_
#define TRANSFORMATION_H_

//! GLM includes
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace scene {

	class Transformation {

		protected:
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
			glm::mat4 modelMatrix;

		public:
			Transformation();
			virtual ~Transformation();

			void Translate(glm::vec3 t);
			void Rotate(glm::quat r);
			void Scale(glm::vec3 s);

			//! Setter
			void SetMatrix(glm::mat4);

			//! Getter
			glm::mat4 GetMatrix(void);
	};

} //! namespace scene
#endif //! TRANSFORMATION_H_
