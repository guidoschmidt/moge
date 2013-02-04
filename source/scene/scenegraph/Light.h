//! Light.h
/*!
 * @date 	02.02.2013
 * @author 	Guido Schmidt
 */

#ifndef LIGHT_H_
#define LIGHT_H_

#include "Mesh.h"
#include "Node.h"

namespace scene {
	//! Enumeration for possible light types

	typedef enum {
		POINT = 0,
		SPOT = 1
	} lighttype;

	class Light: public scene::Node {
		private:
			float m_intensity;
			glm::vec3 m_color;
			lighttype m_lightType;

		public:
			Light();
			Light(glm::vec3 position, float intensty, glm::vec3 color, lighttype type);

			virtual ~Light();

			//! Getter
			float GetIntensity(void);
			glm::vec3 GetColor(void);
			lighttype GetLightType(void);
			//! Setter
			void SetIntensity(float intensity);
			void SetColor(glm::vec3 color);
			void SetLightType(lighttype type);

			void DrawLightGeometry(void);
	};

} //! namespace scene
#endif //! LIGHT_H_
