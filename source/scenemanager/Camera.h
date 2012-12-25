/*! Camera class defintion
 *
 * @author Guido Schmidt
 * @date 07.12.12
 */

#ifndef CAMERA_H_
#define CAMERA_H_

//! GLM includes
#include <glm/glm.hpp>

class Camera
{
	private:
		glm::vec3 position, lookAt, up;

	public:
		Camera();
		virtual ~Camera();

		//! Getter
		glm::vec3 GetPosition(void);
		glm::vec3 GetLookAt(void);
		glm::vec3 GetUp(void);
		//! Setter
		void SetPosition(glm::vec3 position);
		void SetLookAt(glm::vec3 lookAt);
		void SetUp(glm::vec3 up);
};

#endif /* CAMERA_H_ */
