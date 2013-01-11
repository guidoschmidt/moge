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
	void Transformation::Translate(glm::vec3 t){ position = t; }
	void Transformation::TranslateX(float dx){ position.x += dx; }
	void Transformation::TranslateY(float dy){ position.y += dy; }
	void Transformation::TranslateZ(float dz){ position.z += dz; }
	//!
	void Transformation::Rotate(glm::quat r){ rotation = r; }
	//!
	void Transformation::Scale(glm::vec3 s){ scale = s; }

	//!
	/*!
	 *
	 */
	void Transformation::AccumulateModelMatrix(void)
	{
		glm::mat4 TranslationMatrix = glm::translate(position);
		glm::mat4 tempRotationMatrix = glm::mat4_cast(rotation);
		glm::mat4 ScaleMatrix = glm::scale(scale);

//		float tempY0 = tempRotationMatrix[1][0];
//		float tempY1 = tempRotationMatrix[1][1];
//		float tempY2 = tempRotationMatrix[1][2];
//		float tempY3 = tempRotationMatrix[1][3];
//
//		glm::mat4 RotationMatrix;
//		RotationMatrix[][]

		//! TODO Rotationmatrix
		modelMatrix = glm::mat4(1.0f) * TranslationMatrix * ScaleMatrix * tempRotationMatrix;
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::mat4 Transformation::GetModelMatrix(void)
	{
		AccumulateModelMatrix();
		return modelMatrix;
	}

} //! namespace scene
