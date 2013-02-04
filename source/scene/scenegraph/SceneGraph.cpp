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
	SceneGraph::SceneGraph()
	{
		m_setupComplete = false;
		m_writeLogFile = true;

		m_activeCamera_ptr = 0;

		m_lightMatIndex = -1;

		Initialize();
	}


	//! Destructor
	SceneGraph::~SceneGraph()
	{
		if(m_writeLogFile)
			m_logfile.close();
	}


	//! Initializes the scenegraph
	/*!
	 *	TODO Add MaterialManager
	 */
	void SceneGraph::Initialize(void)
	{
		m_materialman_ptr = Singleton<scene::MaterialManager>::Instance();

		Node root();

		if(m_writeLogFile)
		{
			m_logfile.open("logs/scene.txt");

			m_logfile << "LOGFILE" << std::endl;
			m_logfile << "SCENE & SCENEGRAPH" << std::endl;
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
			m_scene_ptr = m_aiImporter.ReadFile(filename, aiProcess_Triangulate | aiProcess_RemoveRedundantMaterials );
			if(!m_scene_ptr)
			{
				m_logfile << "ERROR | assimp: could not import " << filename << std::endl;
			}
			else
			{
				ProcessScene(m_scene_ptr);
			}
		}
		else
		{
			m_logfile << "ERROR | assimp: file " << filename << " could not be read!" << std::endl;
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
		m_logfile << "#Cameras: " << scene->mNumCameras << " camera" << std::endl;
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

		/* CAMERAS ****************************************************************/
		//! Manually add a a camera
		glm::vec3 position(0.0f, 5.0f, 20.0f);
		glm::vec3 lookAt(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		Camera* camera = new Camera(position, lookAt, up);
		m_activeCamera_ptr = camera;
		m_root.AddChild(camera);

		/* MATERIALS ****************************************************************/
		//! Materials
		for(unsigned int mat = 0; mat < scene->mNumMaterials; mat++)
		{
			aiMaterial* material = scene->mMaterials[mat];
			//! Get material's name
			aiString ai_mat_name;
			aiGetMaterialString(material, AI_MATKEY_NAME, &ai_mat_name);
			std::string mat_name = &(ai_mat_name.data[0]);

			//! Check if material is Light
			if(mat_name == "Light")
			{
				m_lightMatIndex = mat;
				std::cout << "matLightIndex = " << m_lightMatIndex << std::endl;
			}

			//! Get material's textures
			aiString ai_tex_path;
			std::vector<texture> textures;
			//! Diffuse
			//! File-extension and folder of textures
			std::string fileextension = "tga";
			material->GetTexture(aiTextureType_DIFFUSE, 0, &ai_tex_path);
			std::string diffuseTex_path = &(ai_tex_path.data[0]);
			size_t foundname = diffuseTex_path.find_last_of("/");
			std::string tex_name_temp = diffuseTex_path.substr(foundname+1);
			unsigned extPos = tex_name_temp.find("." + fileextension);
			std::string tex_name = tex_name_temp.erase(extPos);

			texture diffuse;
			diffuse.m_filename = "./assets/texture/"+ fileextension + "/" + tex_name + "." + fileextension;
			diffuse.m_type = DIFFUSE;
			textures.push_back(diffuse);

			if(mat_name == "Tiles" || mat_name == "Skin")
			{
				texture normal;
				normal.m_filename = "./assets/texture/" + fileextension + "/" + tex_name + "_normal." + fileextension;
				normal.m_type = NORMAL;
				textures.push_back(normal);
			}

			float reflectance = 0.0f;
			if(mat_name == "Cobblestone")
				reflectance = 1.0f;

			m_materialman_ptr->AddMaterial(mat_name, reflectance, textures);
		}

		// Cubemaps
		m_materialman_ptr->AddCubeMap("./assets/texture/cubemaps/stockholm");

		/* MESHES ****************************************************************/
		//! Meshes
		m_logfile << "#Meshes: " << scene->mNumMeshes << std::endl;
		for(unsigned int c = 0; c < scene->mRootNode->mNumChildren; c++)
		{
			for(unsigned int m = 0; m < scene->mRootNode->mChildren[c]->mNumMeshes; m++)
			{
				//! Get mesh and create new scenegraph node
				unsigned int meshID = scene->mRootNode->mChildren[c]->mMeshes[m];
				//! Get material index of mesh
				unsigned int material_index = scene->mMeshes[meshID]->mMaterialIndex;

				//! Get aiNode's transformation
				aiVector3D aiPosition, aiScale;
				aiQuaternion aiRotation;
				scene->mRootNode->mChildren[c]->mTransformation.Decompose(aiScale, aiRotation, aiPosition);

				//! It's light geometry
				if(material_index == m_lightMatIndex)
				{
					std::cout << "index = " << material_index << std::endl;
					m_pointLightMesh = new Mesh(scene->mMeshes[meshID]);
					Light* light = new Light(glm::vec3(0.0f), 1.0f, glm::vec3(1.0f), POINT);
					//! Convert the aiNode's transformation and store them in mesh
					//! Translation
					glm::vec3 position(aiPosition.x, aiPosition.y, aiPosition.z);
					light->Translate(position);
					//! Scale
					glm::vec3 scale(aiScale.x, aiScale.y, aiScale.z);
					light->Scale(scale);
					//! Rotation
					glm::quat rotation(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
					light->RotateQuat(rotation);

					m_root.AddChild(light);
					std::cout << "Light #" << m << " was added to the scenegraph!" << std::endl;
				}
				//! It's mesh geometry
				else
				{
					Mesh* mesh = new Mesh(scene->mMeshes[meshID]);
					mesh->SetMaterial(m_materialman_ptr->GetMaterial(material_index));

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
					m_root.AddChild(mesh);
					m_logfile << "Mesh #" << m << " was added to the scenegraph!" << std::endl;
				}
			}
		}

		m_logfile << "Scene Processing was successfull" << std::endl;
		m_setupComplete = true;
	}


	//!
	/*!
	 *
	 * @param logging
	 */
	void SceneGraph::Logging(bool logging)
	{
		m_writeLogFile = logging;
	}


	//! Returns the number of nodes
	/*!
	 *
	 * @return
	 */
	unsigned int SceneGraph::NodeCount(void)
	{
		return m_root.ChildrenCount();
	}


	//! Returns the node at position i
	/*!
	 *
	 * @param i
	 * @return
	 */
	Node* SceneGraph::GetNode(int i)
	{
		return m_root.GetChild(i);
	}


	//!
	/*!
	 *
	 * @return
	 */
	Camera* SceneGraph::GetActiveCamera(void)
	{
		if(m_activeCamera_ptr != 0)
			return m_activeCamera_ptr;
		else
			return new Camera();
	}


	//! Draws all drawable nodes
	/*!
	 *
	 */
	glm::mat4 SceneGraph::DrawNodes()
	{
		if(m_setupComplete)
		{
			for(unsigned int i = 0; i < m_root.ChildrenCount(); i++)
			{
				m_root.GetChild(i)->Draw();
				return m_root.GetChild(i)->GetModelMatrix();
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
		if((i >= 0) && (i < m_root.ChildrenCount()))
		{
			//! Check if mesh
			if(dynamic_cast<Mesh*>(m_root.GetChild(i)) != 0)
			{
				m_root.GetChild(i)->Draw();
				return m_root.GetChild(i)->GetModelMatrix();
			}
			//!TODO check if camera or light
		}
		return glm::mat4(1.0f);
	}

} //! namespace scene
