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
		m_activeLight_ptr = 0;

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
		//! Set the scene's name
		m_scene_name = "museum";

		std::ifstream infile(filename.c_str());
		if(!infile.fail())
		{
			m_scene_ptr = m_aiImporter.ReadFile(filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials );
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
		/* CAMERAS ****************************************************************/
		//! Manually add a a camera
		//! Standard Camera
		glm::vec3 position(0.0f, 1.8f, 5.0f);
		glm::vec3 lookAt(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);

		Camera* camera = new Camera(position, lookAt, up);
		m_activeCamera_ptr = camera;
		m_root.AddChild(camera);

		/* MATERIALS ****************************************************************/
		//! Materials
		for(unsigned int mat = 0; mat < scene->mNumMaterials; mat++)
		{
			//! Get current material
			aiMaterial* material = scene->mMaterials[mat];
			//! Get material's name
			aiString ai_mat_name;
			aiGetMaterialString(material, AI_MATKEY_NAME, &ai_mat_name);
			std::string mat_name = &(ai_mat_name.data[0]);

			//! Check if material is Light
			std::string contains_light("Light");
			std::string isLight;
			if(mat_name.find(contains_light) != std::string::npos)
			{
				//! Light material
				m_lightMatIndex = mat;
				size_t foundLight = mat_name.find_first_of("_");
				isLight = mat_name.substr(0, foundLight);
			}

			//! Get material's diffuse color
			aiColor3D aiDiffuse;
			material->Get(AI_MATKEY_COLOR_SPECULAR, aiDiffuse);
			glm::vec3 difColor = glm::vec3(aiDiffuse.r, aiDiffuse.g, aiDiffuse.b);

			//! Get material's textures
			//! File-extension and folder of textures
			std::string fileextension = "tga";
			aiString ai_tex_path;
			std::vector<texture> textures;
			std::string tex_name;
			float reflectance = 0.0f;

			//! Diffuse texture
			if(AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &ai_tex_path))
			{
				//! extract texture name from path
				std::string diffuseTex_path = &(ai_tex_path.data[0]);
				size_t foundname = diffuseTex_path.find_last_of("/");
				std::string tex_name_temp = diffuseTex_path.substr(foundname+1);
				unsigned extPos = tex_name_temp.find("." + fileextension);

				tex_name = tex_name_temp.erase(extPos);
			}
			//! Handle texture errors
			else
			{
				tex_name = "common/none";
			}
			//! Texture if material is a light
			if(isLight == "Light")
			{
				texture light;
				light.m_filename = "./assets/texture/common/Light_White.tga";
				light.m_type = LIGHT;
				textures.push_back(light);
			}
			//! Textures for other materials
			else
			{
				//! Diffuse maps
				texture diffuse;
				diffuse.m_filename = "./assets/texture/scene-" + m_scene_name + "/" + fileextension + "/" + tex_name + "." + fileextension;
				diffuse.m_type = DIFFUSE;
				textures.push_back(diffuse);

				//! Normal maps
				if( mat_name == "Street" 		||
					mat_name == "Skin" 			||
					mat_name == "Tiles"			||
					mat_name == "Cobblestone" 	||
					mat_name == "Museum"		||
					mat_name == "Rusty_Metall"
				   )
				{
					texture normal;
					normal.m_filename = "./assets/texture/scene-" + m_scene_name + "/" + fileextension + "/" + tex_name + "_normal." + fileextension;
					normal.m_type = NORMAL;
					textures.push_back(normal);
					std::cout << "	" << normal.m_type << ": " << normal.m_filename << std::endl;
				}

			//! Console output
			std::cout << "\nMaterial " << mat << ":" << mat_name << std::endl;
			std::cout << "	Diffuse Color: (" << difColor.r << ", " << difColor.g << ", " << difColor.b << ")" << std::endl;
			std::cout << "	Textures: " << std::endl;
			std::cout << "	" << diffuse.m_type << ": " << diffuse.m_filename << std::endl;
			}

			//! Add material to material manager
			m_materialman_ptr->AddMaterial(mat_name, difColor, reflectance, textures);
		}

		/* CUBEMAPS ****************************************************************/
		m_materialman_ptr->AddCubeMap("./assets/texture/cubemaps/museum/museum");

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

				//! Get node's name and type
				aiString ai_node_name = scene->mRootNode->mChildren[c]->mName;
				std::string node_name = &(ai_node_name.data[0]);
				size_t foundname = node_name.find_first_of("_");
				std::string node_type = node_name.substr(0, foundname);
				node_name = node_name.substr(foundname+1);

				//! Importing Nodes
				/******************************************/
				//! Light geometry
				if(node_type == "LightGeometry")
				{
					if(node_name == "Sphere")
						m_pointLightMesh = new Mesh(scene->mMeshes[meshID]);
					else if(node_name == "Cone")
						m_spotLightMesh = new Mesh(scene->mMeshes[meshID]);
				}
				//! Lights
				if(node_type == "Light")
				{
					Light* light = new Light(node_name, glm::vec3(0.0f), 1.0f, glm::vec3(1.0f), POINT);
					m_materialman_ptr->GetMaterial(material_index)->GetDiffuseColor();

					//! Convert the aiNode's transformation and store them in mesh
					//! Translation
					glm::vec3 position(aiPosition.x, aiPosition.y, aiPosition.z);
					light->Translate(position);
					std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
					//! Scale
					glm::vec3 scale(aiScale.x, aiScale.y, aiScale.z);
					light->Scale(scale);
					//! Rotation
					glm::quat rotation(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
					light->RotateQuat(rotation);

					light->SetMaterial(m_materialman_ptr->GetMaterial(material_index));
					light->SetColor(light->GetMaterial()->GetDiffuseColor());

					m_root.AddChild(light);
					m_lights.push_back(light);
					m_activeLight_ptr = m_lights.size()-1;

					std::cout << "Node #" << c << " is a Light!";
					std::cout << " Color: "<< light->GetColor().r << ", " << light->GetColor().r << ", " << light->GetColor().r << std::endl;
				}
				//! Mesh geometry
				else if(node_type == "Mesh")
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
					std::cout << "Mesh #" << m << " was added to the scenegraph!" << std::endl;
				}
				else if(node_type == "Billboard")
				{
					//Billboard* billboard = new Billboard();
					Billboard* billboard = new Billboard(scene->mMeshes[meshID]);
					billboard->SetMaterial(m_materialman_ptr->GetMaterial(material_index));

					//! Convert the aiNode's transformation and store them in mesh
					//! Translation
					glm::vec3 position(aiPosition.x, aiPosition.y, aiPosition.z);
					billboard->Translate(position);
					//! Scale
					glm::vec3 scale(aiScale.x, aiScale.y, aiScale.z);
					billboard->Scale(scale);
					//! Rotation
					glm::quat rotation(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
					billboard->RotateQuat(rotation);
					m_impostors.push_back(billboard);
					std::cout << "Billboard #" << m << " was added to the scenegraph!" << std::endl;
				}
			}
		}

		m_logfile << "Scene Processing was successfull:" << std::endl;
		std::cout << "Scene # Lights:" << m_lights.size() << std::endl;

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

	//! Returns the billboard at position i
	/*!
	 *
	 * @param i
	 * @return
	 */
	Billboard* SceneGraph::GetImpostor(int i)
	{
		return m_impostors[i];
	}

	//!
	/*!
	 *
	 * @param type
	 * @return
	 */
	Mesh* SceneGraph::GetLightMesh(lighttype type)
	{
		Mesh* mesh;
		switch(type)
		{
			case POINT:
				mesh = m_pointLightMesh;
				break;
			case SPOT:
				mesh = m_spotLightMesh;
				break;
		}
		return mesh;
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


	//!
	/*!
	 *
	 * @return
	 */
	Light* SceneGraph::GetActiveLight(void)
	{
		if(m_activeLight_ptr >= 0 && m_activeLight_ptr < m_lights.size())
			return m_lights[m_activeLight_ptr];
		else
			return new Light();
	}

	//!
	/*!
	 *
	 */
	int SceneGraph::GetLightCount(void)
	{
		return m_lights.size();
	}


	//!
	/*!
	 *
	 * @param i
	 * @return
	 */
	Light* SceneGraph::GetLight(int i)
	{
		return m_lights[i];
	}

	//!
	/*!
	 *
	 * @param i
	 */
	void SceneGraph::SetActiveLight(int i)
	{
		m_activeLight_ptr = i;
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


	//! Draws a single node of the scenegraph
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
