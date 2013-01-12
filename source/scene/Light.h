//! Light.h
/*!
 * @date	11.01.2013
 * @author	Guido Schmidt
 */

#ifndef LIGHT_H_
#define LIGHT_H_

//! GLM includes
#include <glm/glm.hpp>
//! Local includes
#include "Node.h"

namespace scene {

	class Light: public scene::Node {

		private:
			glm::vec3 m_lightColor;
			float m_intensity;

		public:
			Light(glm::vec3 position, glm::vec3 color, float intensity);
			virtual ~Light();

			// Getter
			glm::vec3 GetColor(void);
			float GetIntensity(void);
			// Setter
			void SetColor(glm::vec3 color);
			void SetIntensity(float intensity);
	};

} //! namespace scene */
#endif //! LIGHT_H_ */
