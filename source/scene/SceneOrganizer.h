//!
/*!
 * SceneOrganizer.h
 *
 *  Created on: 09.01.2013
 *      Author: guidoschmidt
 */

#ifndef SCENEORGANIZER_H_
#define SCENEORGANIZER_H_

//! C++ includes
#include <vector>
//! Local includes
#include "SceneGraph.h"
#include "MaterialManager.h"
#include "../utilities/Singleton.h"

namespace scene {

	class SceneOrganizer {

		private:
			SceneGraph* scenegraph;
			MaterialManager* materialman;
			std::vector<Node*> oneMaterial;

		protected:

		public:
			SceneOrganizer();
			virtual ~SceneOrganizer();

			std::vector<Node*>* OrganizeScene(void);

		};

} //! namespace scene
#endif //! SCENEORGANIZER_H_
