//! Scenegraph.cpp
/*!
 * TODO Extend logging
 *
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

		activeCamera = 0;

		Initialize();
	}


	//! Destructor
	/*!
	 *
	 */
	SceneGraph::~SceneGraph()
	{
		if(writeLogFile)
			logfile.close();
	}


	//! Initializes the scenegraph
	/*!
	 *	TODO Add MaterialManager
	 */
	void SceneGraph::Initialize(void)
	{
		materialman = Singleton<scene::MaterialManager>::Instance();

		Node root();

		if(writeLogFile)
		{
			logfile.open("logs/scene.txt");

			logfile << "LOGFILE" << std::endl;
			logfile << "SCENE & SCENEGRAPH" << std::endl;
		}
	}


	//! Loads a scene from a file
	/*!
	 *
	 * @param filename
	 */
	void SceneGraph::LoadSceneFromFile(const std::string filename)
	{
		std::ifstream infile(filename.c_str());
		if(!infile.fail())
		{
			scene = aiImporter.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
			if(!scene)
			{
				logfile << "ERROR | assimp: could not import " << filename << std::endl;
			}
			else
			{
				ProcessScene(scene);
			}
		}
		else
		{
			logfile << "ERROR | assimp: file " << filename << " could not be read!" << std::endl;
		}
		infile.close();
	}


	//! Processes an assimp scene
	/*!
	 *
	 * @param scene
	 */
	void SceneGraph::ProcessScene(const aiScene* scene)
	{
		//! Cameras
		logfile << "#Cameras: " << scene->mNumCameras << " camera" << std::endl;
		//! TODO Camera import stuff, blender-collada export doesnt provide correct camera coordinates
		/*!
		 * Incomment code for correct import of camera
		 *
		if(scene->HasCameras())
		{
			for(unsigned int cam = 0; cam < scene->mNumCameras; cam++)
			{

				glm::vec3 position(	scene->mCameras[cam]->mPosition.x,
									scene->mCameras[cam]->mPosition.y,
									scene->mCameras[cam]->mPosition.z);
				glm::vec3 lookAt(	scene->mCameras[cam]->mLookAt.x,
									scene->mCameras[cam]->mLookAt.y,
									scene->mCameras[cam]->mLookAt.y);
				glm::vec3 up(		scene->mCameras[cam]->mUp.x,
									scene->mCameras[cam]->mUp.y,
									scene->mCameras[cam]->mUp.z);
				Camera* camera = new Camera(position, lookAt, up);

				root.AddChild(camera);
				activeCamera = camera;
				logfile << "#Camera(" << cam << ") @ (" << position.x << "|" << position.y << "|" << position.z << ")" << std::endl;
				logfile << "#Camera-LookAt(" << cam << ") @ (" << lookAt.x << "|" << lookAt.y << "|" << lookAt.z << ")" << std::endl;

			}
		}*/
		//! Manually add a a camera
		glm::vec3 position(0.0f, 5.0f, 10.0f);
		glm::vec3 lookAt(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		Camera* camera = new Camera(position, lookAt, up);
		activeCamera = camera;
		root.AddChild(camera);


		//! Materials
		for(unsigned int mat = 0; mat < scene->mNumMaterials; mat++)
		{
			aiMaterial* material = scene->mMaterials[mat];
			//! Get materials name
			aiString mat_name;
			aiGetMaterialString(material, AI_MATKEY_NAME, &mat_name);
			//! Get materils' texture
			aiString ai_tex_path;

			material->GetTexture(aiTextureType_DIFFUSE, 0, &ai_tex_path);
			std::string tex_path = &(ai_tex_path.data[0]);
			size_t found = tex_path.find_last_of("/");
			std::string tex_name = tex_path.substr(found+1);


			if(tex_name == "Marble.jpg")
			{
				materialman->AddMaterial(mat_name.C_Str(), "./assets/texture/jpg/" + tex_name, 0.75);
			}
			else
				materialman->AddMaterial(mat_name.C_Str(), "./assets/texture/jpg/" + tex_name, 0.0);


		}

		//! Meshes
		logfile << "#Meshes: " << scene->mNumMeshes << std::endl;
		for(unsigned int c = 0; c < scene->mRootNode->mNumChildren; c++)
		{
			for(unsigned int m = 0; m < scene->mRootNode->mChildren[c]->mNumMeshes; m++)
			{
				//! Get mesh and create new scenegraph node
				unsigned int meshID = scene->mRootNode->mChildren[c]->mMeshes[m];
				Mesh* mesh = new Mesh(scene->mMeshes[meshID]);
				//! Get material index of mesh
				unsigned int material_index = scene->mMeshes[meshID]->mMaterialIndex;
				mesh->SetMaterial(materialman->GetMaterial(material_index));

				//! Get aiNode's transformation
				aiVector3D aiPosition, aiScale;
				aiQuaternion aiRotation;
				scene->mRootNode->mChildren[c]->mTransformation.Decompose(aiScale, aiRotation, aiPosition);

				//! Convert the aiNode's transformation and store them in mesh
				//! Translation
				glm::vec3 position(aiPosition.x, aiPosition.y, aiPosition.z);
				mesh->Translate(position);
				//! Scale
				glm::vec3 scale(aiScale.x, aiScale.y, aiScale.z);
				mesh->Scale(scale);
				//! Rotation
				glm::quat rotation(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
				mesh->RotateQuat(rotation);

				//! Add mesh to scenegraph
				root.AddChild(mesh);
				logfile << "Mesh #" << m << " was added to the scenegrapg!" << std::endl;
			}
		}
		logfile << "Scene Processing was successfull" << std::endl;
		setupComplete = true;
	}


	//!
	/*!
	 *
	 * @param logging
	 */
	void SceneGraph::Logging(bool logging)
	{
		writeLogFile = logging;
	}


	//! Returns the number of nodes
	/*!
	 *
	 * @return
	 */
	unsigned int SceneGraph::NodeCount(void)
	{
		return root.ChildrenCount();
	}


	//! Returns the node at position i
	/*!
	 *
	 * @param i
	 * @return
	 */
	Node* SceneGraph::GetNode(int i)
	{
		return root.GetChild(i);
	}


	//!
	/*!
	 *
	 * @return
	 */
	Camera* SceneGraph::GetActiveCamera(void)
	{
		if(activeCamera != 0)
			return activeCamera;
		else
			return new Camera();
	}


	//! Draws all drawable nodes
	/*!
	 *
	 */
	glm::mat4 SceneGraph::DrawNodes()
	{
		if(setupComplete)
		{
			for(unsigned int i = 0; i < root.ChildrenCount(); i++)
			{
				root.GetChild(i)->Draw();
				return root.GetChild(i)->GetModelMatrix();
			}
		}
		return glm::mat4(1.0f);
	}


	//! Draws a given node of the scenegraph
	/*!
	 *
	 * @param i
	 * @return
	 */
	glm::mat4 SceneGraph::DrawNode(unsigned int i)
	{
		if((i >= 0) && (i < root.ChildrenCount()))
		{
			//! Check if mesh
			if(dynamic_cast<Mesh*>(root.GetChild(i)) != 0)
			{
				root.GetChild(i)->Draw();
				return root.GetChild(i)->GetModelMatrix();
			}
			//!TODO check if camera or light
		}
		return glm::mat4(1.0f);
	}

} //! namespace scene
