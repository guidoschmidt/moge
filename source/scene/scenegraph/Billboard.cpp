//! Billboard.cpp
/*!
 * @date	11.02.2013
 * @author	Guido Schmidt
 */

#include "Billboard.h"

namespace scene {

	//! Constructor
	Billboard::Billboard()
	{
		m_type = "Billboard";

		m_mesh_ptr = 0;

		VAO_id = 0;
		BBVAO_id = 0;
		VBO_id = 0;
		IBO_id = 0;
		NBO_id = 0;
		UVBO_id = 0;
		BBO_id = 0;
		BBIO_id = 0;

		m_position = glm::vec3(0, 0, 0);
		m_rotation = glm::quat(0, 0, 0, 0);
		m_scale = glm::vec3(1, 1, 1);

		ErrorCheckMesh = 0;

		Initialize();
		CreateBuffers();
	}

	//! Constructor
	Billboard::Billboard(aiMesh* mesh)
	{
		m_type = "Billboard";

		m_mesh_ptr = mesh;

		VAO_id = 0;
		BBVAO_id = 0;
		VBO_id = 0;
		IBO_id = 0;
		NBO_id = 0;
		UVBO_id = 0;
		BBO_id = 0;
		BBIO_id = 0;

		m_position = glm::vec3(0, 0, 0);
		m_rotation = glm::quat(0, 0, 0, 0);
		m_scale = glm::vec3(1, 1, 1);

		ErrorCheckMesh = 0;

		Initialize();
		CreateBuffers();
	}

	//! Destructor
	Billboard::~Billboard()
	{
		// TODO Auto-generated destructor stub
	}


	//! Initialize
	void Billboard::Initialize(void)
	{
		if(m_mesh_ptr == 0)
		{
			//! Vertex list
			//! Upper left corner
			m_vertices.push_back( 0.5f);
			m_vertices.push_back( 0.0f);
			m_vertices.push_back( 0.5f);
			//! Lower left corner
			m_vertices.push_back(-0.5f);
			m_vertices.push_back( 0.0f);
			m_vertices.push_back( 0.5f);
			//! Lower right corner
			m_vertices.push_back(-0.5f);
			m_vertices.push_back( 0.0f);
			m_vertices.push_back(-0.5f);
			//! Upper right corner
			m_vertices.push_back( 0.5f);
			m_vertices.push_back( 0.0f);
			m_vertices.push_back(-0.5f);

			//! Index list
			//! First triangle
			m_indices.push_back(0);
			m_indices.push_back(1);
			m_indices.push_back(3);
			//! Second triangle
			m_indices.push_back(1);
			m_indices.push_back(2);
			m_indices.push_back(3);

			//! UV coordinates
			m_uvs.push_back(1.0f);
			m_uvs.push_back(0.0f);

			m_uvs.push_back(0.0f);
			m_uvs.push_back(0.0f);

			m_uvs.push_back(0.0f);
			m_uvs.push_back(1.0f);

			m_uvs.push_back(1.0f);
			m_uvs.push_back(1.0f);
		}
		else
		{
			//! Topology initialization
			std::cout << "Billboard has " << m_mesh_ptr->mNumVertices << " Vertices" << std::endl;
			for(unsigned int v=0; v < m_mesh_ptr->mNumVertices; v++)
			{
				//! Write vertices
				m_vertices.push_back(m_mesh_ptr->mVertices[v].x);
				m_vertices.push_back(m_mesh_ptr->mVertices[v].y);
				m_vertices.push_back(m_mesh_ptr->mVertices[v].z);
				std::cout << m_mesh_ptr->mVertices[v].x << " | " << m_mesh_ptr->mVertices[v].y << " | " << m_mesh_ptr->mVertices[v].z << std::endl;
				//! Write normals
				m_normals.push_back(m_mesh_ptr->mNormals[v].x);
				m_normals.push_back(m_mesh_ptr->mNormals[v].y);
				m_normals.push_back(m_mesh_ptr->mNormals[v].z);
				//! Write uv coordinates/texture coordinates if they exist
				if(m_mesh_ptr->HasTextureCoords(0)){
					m_uvs.push_back(m_mesh_ptr->mTextureCoords[0][v].x);
					m_uvs.push_back(m_mesh_ptr->mTextureCoords[0][v].y);
				}
			}
			for(unsigned int f=0; f < m_mesh_ptr->mNumFaces; f++)
			{
				for(unsigned int i=0; i < m_mesh_ptr->mFaces[f].mNumIndices; i++){
					m_indices.push_back(m_mesh_ptr->mFaces[f].mIndices[i]);
				}
			}
		}


		m_u = glm::vec3(m_vertices[0], m_vertices[1], m_vertices[2]) - glm::vec3(m_vertices[3], m_vertices[4], m_vertices[5]);
		std::cout << "Billboard m_u = " << m_u.x << " | " << m_u.y << " | " << m_u.z << std::endl;
		m_v = glm::vec3(m_vertices[6], m_vertices[7], m_vertices[8]) - glm::vec3(m_vertices[3], m_vertices[4], m_vertices[5]);
		std::cout << "Billboard m_v = " << m_v.x << " | " << m_v.y << " | " << m_v.z << std::endl;
		CalculateNormal();
	}

	//!
	/*!
	 *
	 */
	void Billboard::CreateBuffers(void)
	{
		ErrorCheckMesh = glGetError();

		glGenVertexArrays(1, &VAO_id);
		glBindVertexArray(VAO_id);

		//! Create vertex buffer
		glGenBuffers(1, &VBO_id);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), &m_vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(0);

		//! Create index buffer
		glGenBuffers(1, &IBO_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLint), &m_indices[0], GL_STATIC_DRAW);

		//! Create uv buffer
		glGenBuffers(1, &UVBO_id);
		glBindBuffer(GL_ARRAY_BUFFER, UVBO_id);
		glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(GLfloat), &m_uvs[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(2);
	}

	//!
	void Billboard::CalculateNormal(void)
	{
		m_normal = glm::normalize(glm::cross(m_v, m_u));
		std::cout << "Billboard normal = " << m_normal.x << " | " << m_normal.y << " | " << m_normal.z << std::endl;
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::vec3 Billboard::GetNormal(void)
	{
		return m_normal;
	}

	//!
	/*!
	 *
	 * @param i
	 * @return
	 */
	glm::vec3 Billboard::GetVertex(int i)
	{
		glm::vec3 vertex;
		if(i <= 4)
			vertex = glm::vec3(m_vertices[i], m_vertices[i+1], m_vertices[i+2]);
		else
			vertex = glm::vec3(0.0, 0.0, 0.0);

		return vertex;
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::vec3 Billboard::GetUMax(void)
	{
		return m_u;
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::vec3 Billboard::GetVMax(void)
	{
		return m_v;
	}

	//!
	/*!
	 *
	 * @return
	 */
	Camera* Billboard::GetCamera(void)
	{
		return m_camera_ptr;
	}

	//!
	/*!
	 *
	 * @param camera
	 */
	void Billboard::SetCamera(Camera* camera)
	{
		m_camera_ptr = camera;
	}

	//!
	/*!
	 *
	 */
	void Billboard::Draw(void)
	{
		AccumulateMatrices();
		glBindVertexArray(VAO_id);
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}




} //! namespace scene
