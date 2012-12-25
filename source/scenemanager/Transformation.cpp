/*!	Scene object class
 *	@author Guido Schmidt
 *	@date 23.12.12
 */

#include "SceneObject.h"

//!
inline glm::vec3 SceneObject::GetTranslation(void){ return translation; }

//!
inline glm::quat SceneObject::GetRotation(void){ return rotation; }

//!
inline glm::vec3 SceneObject::GetScale(void){ return scale; }


//!
inline void SceneObject::Translate(glm::vec3 t){ translation = t; }
//!
inline void SceneObject::Rotate(glm::quat r){ rotation = r; }
//!
inline void SceneObject::Scale(glm::vec3 s){ scale = s; }
