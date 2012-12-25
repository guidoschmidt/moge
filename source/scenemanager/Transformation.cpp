/*!	Scene object class
 *	@author Guido Schmidt
 *	@date 23.12.12
 */

#include "Transformation.h"

namespace scenegraph {

	//! Get translation
	inline glm::vec3 Transformation::GetTranslation(void){ return translation; }
	//! Get rotation
	inline glm::quat Transformation::GetRotation(void){ return rotation; }
	//! Get scale
	inline glm::vec3 Transformation::GetScale(void){ return scale; }

	//! Set translation
	inline void Transformation::Translate(glm::vec3 t){ translation = t; }
	//! Set rotation
	inline void Transformation::Rotate(glm::quat r){ rotation = r; }
	//! set scale
	inline void Transformation::Scale(glm::vec3 s){ scale = s; }

} //! namespace scenegraph
