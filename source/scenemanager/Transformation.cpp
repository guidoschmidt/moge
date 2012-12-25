/*!	Scene object class
 *	@author Guido Schmidt
 *	@date 23.12.12
 */

#include "Transformation.h"

//! Get translation
inline glm::vec3 scenegraph::Transformation::GetTranslation(void){ return translation; }
//! Get rotation
inline glm::quat scenegraph::Transformation::GetRotation(void){ return rotation; }
//! Get scale
inline glm::vec3 scenegraph::Transformation::GetScale(void){ return scale; }

//! Set translation
inline void scenegraph::Transformation::Translate(glm::vec3 t){ translation = t; }
//! Set rotation
inline void scenegraph::Transformation::Rotate(glm::quat r){ rotation = r; }
//! set scale
inline void scenegraph::Transformation::Scale(glm::vec3 s){ scale = s; }
