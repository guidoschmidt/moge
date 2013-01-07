//! Transformation.cpp
/*!
 * @date	02.01.2013
 * @author	Guido Schmidt
 */

#include "Transformation.h"

namespace scene {

	Transformation::Transformation() {
		// TODO Auto-generated constructor stub

	}

	Transformation::~Transformation() {
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
	void Transformation::AccumulateModelMatrix(void)
	{
		glm::mat4 TranslationMatrix = glm::translate(position);
		// glm::mat4 ScaleMatrix = glm::scale(scale);
		modelMatrix = glm::mat4(1.0f) * TranslationMatrix;
	}

	//!
	glm::mat4 Transformation::GetModelMatrix(void)
	{
		return modelMatrix;
	}

} //! namespace scene
