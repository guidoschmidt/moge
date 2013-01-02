//! Scenegraph.cpp
/*!
 * @date	02.01.2013
 * @author	Guido Schmidt
 */

#include "SceneGraph.h"

namespace scene {

	//! Constructor
	/*!
	 *
	 * @param logging
	 */
	SceneGraph::SceneGraph()
	{
		setupComplete = false;
		writeLogFile = true;
		Initialize();
	}

	//! Destructor
	SceneGraph::~SceneGraph()
	{
		if(writeLogFile)
			logFile.close();
	}

	//! Initializes the scenegraph
	void SceneGraph::Initialize(void)
	{
		Node root();

		if(writeLogFile)
		{
			logFile.open("logs/scene.txt");

			logFile << "LOGFILE" << std::endl;
			logFile << "SCENE & SCENEGRAPH" << std::endl;
		}
	}


	//! Loads a scene from a file
	void SceneGraph::LoadSceneFromFile(const std::string filename)
	{
		std::ifstream infile(filename.c_str());
		if(!infile.fail())
		{
			scene = aiImporter.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
			if(!scene)
			{
				logFile << "ERROR | assimp: could not import " << filename << std::endl;
			}
			else
			{
				ProcessScene(scene);
			}
		}
		else
		{
			logFile << "ERROR | assimp: file " << filename << " could not be read!" << std::endl;
		}
		infile.close();
	}

	//! Process an assimp scene
	void SceneGraph::ProcessScene(const aiScene* scene)
	{

		std::cout << scene->mRootNode->mNumChildren << std::endl;

		for(unsigned int c = 0; c < scene->mRootNode->mNumChildren; c++)
		{
			for(unsigned int m = 0; m < scene->mRootNode->mChildren[c]->mNumMeshes; m++)
			{
				//! Get mesh and create new scenegraph node
				unsigned int meshID = scene->mRootNode->mChildren[c]->mMeshes[m];
				Mesh* mesh = new Mesh(scene->mMeshes[meshID]);

				//! Get aiNode's transformation
				aiVector3D aiPosition, aiScale;
				aiQuaternion aiRotation;
				scene->mRootNode->mChildren[c]->mTransformation.Decompose(aiScale, aiRotation, aiPosition);

				//! Convert the aiNode's transformation and store them in mesh
				glm::vec3 position(aiPosition.x, aiPosition.y, aiPosition.z);
				mesh->Translate(position);
				glm::vec3 scale(aiScale.x, aiScale.y, aiScale.z);
				mesh->Scale(scale);
				glm::quat rotation(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
				mesh->Rotate(rotation);

				//! Add mesh to scenegraph
				root.AddChild(mesh);
			}
		}
		setupComplete = true;
	}

	//!
	void SceneGraph::Logging(bool logging)
	{
		writeLogFile = logging;
	}

	//! Draws all drawable nodes
	/*!
	 *
	 */
	void SceneGraph::DrawNodes(void)
	{
		if(setupComplete)
		{
			for(unsigned int i = 0; i < root.ChildrenCount(); i++)
			{
				root.GetChild(i)->Draw();
			}
		}
	}

} //! namespace scene
