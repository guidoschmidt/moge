//! Transformation.cpp
/*!
 * @date	02.01.2013
 * @author	Guido Schmidt
 */

#ifndef TRANSFORMATION_H_
#define TRANSFORMATION_H_

//! GLM includes
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace scene {

	class Transformation {

		protected:
			glm::vec3 m_position;
			glm::quat m_rotation;
			glm::vec3 m_scale;
			glm::mat4 m_modelMatrix;

		public:
			Transformation();
			virtual ~Transformation();

			void Translate(glm::vec3 t);
			void TranslateX(float dx);
			void TranslateY(float dy);
			void TranslateZ(float dz);
			void Rotate(glm::quat r);
			void Scale(glm::vec3 s);

			//! ModelMatrix
			void AccumulateModelMatrix(void);

			//! Setter
			void SetMatrix(glm::mat4);

			//! Getter
			glm::mat4 GetModelMatrix(void);
			glm::vec3 GetPosition(void);
	};

} //! namespace scene
#endif //! TRANSFORMATION_H_
