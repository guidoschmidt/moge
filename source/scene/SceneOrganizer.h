//! SceneOrganizer.h
/*!
 * @date	09.01.2013
 * @author	Guido Schmidt
 */

#ifndef SCENEORGANIZER_H_
#define SCENEORGANIZER_H_

//! C++ includes
#include <vector>
//! Local includes
#include "./scenegraph/SceneGraph.h"
#include "MaterialManager.h"
#include "../utilities/Singleton.h"

namespace scene {

	class SceneOrganizer {

		private:
			SceneGraph* m_scenegraph_ptr;
			MaterialManager* m_materialman_ptr;
			std::vector<Node*> m_renderQ;

		protected:

		public:
			SceneOrganizer();
			virtual ~SceneOrganizer();

			std::vector<Node*>* OrganizeByMaterial(void);

		};

} //! namespace scene
#endif //! SCENEORGANIZER_H_
