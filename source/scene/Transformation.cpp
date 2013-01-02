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
	//!
	void Transformation::Rotate(glm::quat r){ rotation = r; }
	//!
	void Transformation::Scale(glm::vec3 s){ scale = s; }

} //! namespace scene
