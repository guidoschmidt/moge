//! SceneGraph.h
/*!
 * @date 	02.01.2013
 * @author 	Guido Schmidt
 */

#ifndef SCENEGRAPH_H_
#define SCENEGRAPH_H_

//! C++ includes
#include <string>
#include <fstream>
#include <iostream>
#include <stack>
//! Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include <assimp/quaternion.h>
//! GLM includes
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
//! Local includes
#include "../utilities/Singleton.h"
#include "Node.h"
#include "Camera.h"
#include "Mesh.h"

namespace scene {

	class SceneGraph {

		private:
			bool setupComplete;
			Camera* activeCamera;

			//! Assimp
			Assimp::Importer aiImporter;
			const aiScene* scene;

			//! Node hierachry
			Node root;

			//! Logfile
			bool writeLogFile;
			std::ofstream logfile;

			//! Matrix-stack
			std::stack<glm::mat4> matrixStack;

		public:
			SceneGraph();
			virtual ~SceneGraph();

			void Initialize(void);

			void LoadSceneFromFile(const std::string filename);
			void ProcessScene(const aiScene* scene);

			void Logging(bool logging);

			unsigned int NodeCount(void);

			Node* GetNode(int i);
			Camera* GetActiveCamera(void);

			//! Drawing
			glm::mat4 DrawNodes(void);
			glm::mat4 DrawNode(unsigned int i);
	};

} //! namespace scene
#endif //! SCENEGRAPH_H_
