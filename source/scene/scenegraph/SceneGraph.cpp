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
	void SceneGraph::Initialize(void)
	{
		m_materialman_ptr = Singleton<scene::MaterialManager>::Instance();

		Node root();

		if(m_writeLogFile)
		{
			m_logfile.open("logs/scene.txt");

			m_logfile << "SCENE LOGFILE" << "*******************************************" << std::endl;
			m_logfile << "SCENE & SCENEGRAPH" << std::endl;
		}
	}

	//! Loads a scene from the moge assets directory
	/*!
	 *
	 * @param scenename
	 */
	void SceneGraph::LoadScene(const std::string scenename)
	{
		m_scene_name = scenename;
		std::string loadstring = "./assets/scenes/collada/" + scenename + "/" + scenename + ".dae";
		LoadSceneFromFile(loadstring);
	}

	//! Loads a scene from a file
	/*!
	 *
	 *	@param filename
	 */
	void SceneGraph::LoadSceneFromFile(const std::string filename)
	{
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
	 *	@param scene
	 */
	void SceneGraph::ProcessScene(const aiScene* scene)
	{
		/* CAMERAS ****************************************************************/
		//! Manually add a a camera
		//! Standard Camera
		/*
		glm::vec3 position(0.0f, 1.8f, 5.0f);
		glm::vec3 lookAt(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		*/

		glm::vec3 position, lookAt, up;
		if(m_scene_name == "street")
		{
			position = glm::vec3(30.0f, 15.0f, 30.0f);
			lookAt = glm::vec3(0.0f, 5.0f, 0.0f);
			up = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		if(m_scene_name == "church")
		{
			position = glm::vec3(-15.830f, 11.921f, 2.367f);
			lookAt = glm::vec3(21.529f, -1.912f, -1.679f);
			up = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		if(m_scene_name == "museum")
		{
			position = glm::vec3(-15.0f, 15.0f, 4.0f);
			lookAt = glm::vec3(10.0f, 3.0f, -2.0f);
			up = glm::vec3(0.0f, 1.0f, 0.0f);
		}

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
				texture normal;
				normal.m_filename = "./assets/texture/scene-" + m_scene_name + "/" + fileextension + "/" + tex_name + "_normal." + fileextension;
				normal.m_type = NORMAL;
				textures.push_back(normal);
				std::cout << "	" << normal.m_type << ": " << normal.m_filename << std::endl;

			}

			//! Add material to material manager
			m_materialman_ptr->AddMaterial(mat_name, difColor, reflectance, textures);
		}

		/* CUBEMAPS ****************************************************************/
		m_materialman_ptr->AddCubeMap("./assets/texture/cubemaps/" + m_scene_name + "/" + m_scene_name);

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
				//! Billboards
				else if(node_type == "Billboard")
				{
					Billboard* billboard = new Billboard(scene->mMeshes[meshID]);
					Mesh* billboardMesh = new Mesh(scene->mMeshes[meshID]);
					billboard->SetMaterial(m_materialman_ptr->GetMaterial(material_index));
					billboardMesh ->SetMaterial(m_materialman_ptr->GetMaterial(material_index));

					//! Convert the aiNode's transformation and store them in mesh
					//! Translation
					glm::vec3 position(aiPosition.x, aiPosition.y, aiPosition.z);
					billboard->Translate(position);
					billboardMesh ->Translate(position);
					//! Scale
					glm::vec3 scale(aiScale.x, aiScale.y, aiScale.z);
					billboard->Scale(scale);
					billboardMesh ->Scale(scale);
					//! Rotation
					glm::quat rotation(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
					billboard->RotateQuat(rotation);
					billboardMesh->RotateQuat(rotation);

					m_impostors.push_back(billboard);
					std::cout << "Billboard #" << m << " was added to the imposters list!" << std::endl;
					m_root.AddChild(billboardMesh);
					std::cout << "Billboard #" << m << " was added to the scenegraph!" << std::endl;
				}
				else if(node_type == "Approx")
				{
					m_approx = new BoundingBox();
					m_approx->max = glm::vec3(0, 0, 0);
					m_approx->min = glm::vec3(0, 0, 0);
					for(unsigned int v=0; v < scene->mMeshes[meshID]->mNumVertices; v++)
					{
						glm::vec3 vertex = glm::vec3(scene->mMeshes[meshID]->mVertices[v].x,
													 scene->mMeshes[meshID]->mVertices[v].y,
													 scene->mMeshes[meshID]->mVertices[v].z);
						//! AABB maximum
						if(vertex.x > m_approx->max.x)
							m_approx->max.x = scene->mMeshes[meshID]->mVertices[v].x;
						if(vertex.y > m_approx->max.y)
							m_approx->max.y = scene->mMeshes[meshID]->mVertices[v].y;
						if(vertex.z > m_approx->max.z)
							m_approx->max.z = scene->mMeshes[meshID]->mVertices[v].z;

						//! AABB minimum
						if(vertex.x < m_approx->min.x)
							m_approx->min.x = scene->mMeshes[meshID]->mVertices[v].x;
						if(vertex.y < m_approx->min.y)
							m_approx->min.y = scene->mMeshes[meshID]->mVertices[v].y;
						if(vertex.z < m_approx->min.z)
							m_approx->min.z = scene->mMeshes[meshID]->mVertices[v].z;

						m_approx->vertices.push_back(scene->mMeshes[meshID]->mVertices[v].x);
						m_approx->vertices.push_back(scene->mMeshes[meshID]->mVertices[v].y);
						m_approx->vertices.push_back(scene->mMeshes[meshID]->mVertices[v].z);
					}

					std::cout << "Scene approximation: " << std::endl;
					std::cout << "	max: " << m_approx->max.x << ", " << m_approx->max.y << ", " << m_approx->max.z << std::endl;
					std::cout << "	min: " << m_approx->min.x << ", " << m_approx->min.y << ", " << m_approx->min.z << std::endl;
				}
			}
		}

		m_logfile << "Scene Processing was successfull:" << std::endl;
		std::cout << "Scene # Lights:" << m_lights.size() << std::endl;

		m_setupComplete = true;
	}

	//! Sets logging to console or to file
	/*!
	 *
	 *	@param logging
	 */
	void SceneGraph::Logging(bool logging)
	{
		m_writeLogFile = logging;
	}

	//! Returns the number of nodes
	/*!
	 *
	 *	@return
	 */
	unsigned int SceneGraph::NodeCount(void)
	{
		return m_root.ChildrenCount();
	}

	//! Returns the node at position i
	/*!
	 *
	 *	@param i
	 *	@return
	 */
	Node* SceneGraph::GetNode(int i)
	{
		return m_root.GetChild(i);
	}

	//! Returns the billboard at position i
	/*!
	 *
	 *	@param i
	 *	@return
	 */
	Billboard* SceneGraph::GetImpostor(int i)
	{
		return m_impostors[i];
	}

	//!
	/*!
	 *
	 * @return
	 */
	int SceneGraph::GetImpostorCount(void)
	{
		return m_impostors.size();
	}

	//!
	/*!
	 *
	 *	@param type
	 *	@return
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

	//! Returns the active camera
	/*!
	 *
	 * @return
	 */
	Camera* SceneGraph::GetActiveCamera(void)
	{
			return m_activeCamera_ptr;
	}


	//! Returns the active light
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

	//! Returns the count of lights
	/*!
	 *
	 */
	int SceneGraph::GetLightCount(void)
	{
		return m_lights.size();
	}


	//! Returns a light by it's index i
	/*!
	 *
	 * @param i
	 * @return
	 */
	Light* SceneGraph::GetLight(int i)
	{
		return m_lights[i];
	}

	//! Returns a AABB proxy geometry of the scene
	/*!
	 *	Used for parallax-corrected cube mapping
	 * @return
	 */
	BoundingBox* SceneGraph::GetSceneApproximation(void)
	{
		return m_approx;
	}

	//! Sets light at index i as the active light
	/*!
	 *	Active lights can be changed (color, position)
	 * 
	 * @param i
	 */
	void SceneGraph::SetActiveLight(int i)
	{
		m_activeLight_ptr = i;
	}

	//! Sets the active camera
	/*!
	 *
	 * @param camera
	 */
	void SceneGraph::SetActiveCamera(Camera* camera)
	{
		m_activeCamera_ptr = camera;
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
