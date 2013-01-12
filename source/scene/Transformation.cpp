//! Transformation.cpp
/*!
 * @date	02.01.2013
 * @author	Guido Schmidt
 */

#include "Transformation.h"

namespace scene {

	Transformation::Transformation()
	{
		// TODO Auto-generated constructor stub
	}

	Transformation::~Transformation()
	{
		// TODO Auto-generated destructor stub
	}

	//!
	void Transformation::Translate(glm::vec3 t){ m_position = t; }
	void Transformation::TranslateX(float dx){ m_position.x += dx; }
	void Transformation::TranslateY(float dy){ m_position.y += dy; }
	void Transformation::TranslateZ(float dz){ m_position.z += dz; }
	//!
	void Transformation::Rotate(glm::quat r){ m_rotation = r; }
	//!
	void Transformation::Scale(glm::vec3 s){ m_scale = s; }

	//!
	/*!
	 *
	 */
	void Transformation::AccumulateModelMatrix(void)
	{
		glm::mat4 TranslationMatrix = glm::translate(m_position);
		glm::mat4 tempRotationMatrix = glm::mat4_cast(m_rotation);
		glm::mat4 ScaleMatrix = glm::scale(m_scale);

//		float tempY0 = tempRotationMatrix[1][0];
//		float tempY1 = tempRotationMatrix[1][1];
//		float tempY2 = tempRotationMatrix[1][2];
//		float tempY3 = tempRotationMatrix[1][3];
//
//		glm::mat4 RotationMatrix;
//		RotationMatrix[][]

		//! TODO Rotationmatrix
		m_modelMatrix = glm::mat4(1.0f) * TranslationMatrix * ScaleMatrix * tempRotationMatrix;
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::mat4 Transformation::GetModelMatrix(void)
	{
		AccumulateModelMatrix();
		return m_modelMatrix;
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::vec3 Transformation::GetPosition(void)
	{
		return m_position;
	}

} //! namespace scene
