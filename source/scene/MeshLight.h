//! MeshLight.h
/*!
 * @date	11.01.2013
 * @author	Guido Shcmidt
 */

#ifndef MESHLIGHT_H_
#define MESHLIGHT_H_

#include "Mesh.h"
#include "Light.h"

namespace scene {

	class MeshLight: public Mesh, public Light {

		private:
        	Mesh* mesh_ptr;
        	Light* light_ptr;

		public:
			MeshLight();
			virtual ~MeshLight();
	};

} //! namespace scene
#endif //! MESHLIGHT_H_
