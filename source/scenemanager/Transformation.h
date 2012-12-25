/*!	Scene object header
 *	@author Guido Schmidt
 *	@date 23.12.12
 */

#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

//! OpenGL includes
#include <GL/gl.h>
//! GLM includes
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class SceneObject
{
	private:
		glm::vec3 position;
		glm::vec3 translation;
		glm::quat rotation;
		glm::vec3 scale;

	public:
		SceneObject();
		virtual ~SceneObject();

		//! Getter
		glm::vec3 GetTranslation(void);
		glm::quat GetRotation(void);
		glm::vec3 GetScale(void);
		//! Setter
		void Translate(glm::vec3 translation);
		void Rotate(glm::quat rotation);
		void Scale(glm::vec3 scale);

		virtual void Draw(void);
};

#endif /* SCENEOBJECT_H_ */
