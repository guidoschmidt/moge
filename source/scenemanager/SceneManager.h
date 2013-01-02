/*!	Scene manager class definition
 *	@author Guido Schmidt
 *	@date 	25.12.12
 */

#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

//! C++ includes
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
//! Local includes
#include "Node.h"
#include "MeshNode.h"
//! OpenGL includes
#include <GL/gl.h>
//! Assimp includes
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace scenegraph {

	class SceneManager {

		private:
			//! TODO Make class a singleton
			static SceneManager instance;

			Node root;
			//! Assimp
			Assimp::Importer aiImporter;
			const aiScene *scene;

			bool sceneLoaded;

		protected:
			void ProcessScene(const aiScene* scene);

		public:
			SceneManager();
			~SceneManager();
			static SceneManager& Instance();
			bool LoadScene(std::string filename);
			void Draw();
	};

} //! namespace scenegraph
#endif //! SCENEMANAGER_H_
