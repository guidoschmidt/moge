//! Mesh.cpp
/*!
 * @date 	02.01.2013
 * @author	Guido Schmidt
 */

#include "Mesh.h"

namespace scene {

	//! Constructor
	/*!
	 *
	 * @param m
	 */
	Mesh::Mesh(aiMesh* m)
	{
		mesh_ptr = m;
		Initialize();
		CreateBuffers();
		Material material();

		m_boundingBox.max = glm::vec3( 0.0f,  0.0f,  0.0f);
		m_boundingBox.min = glm::vec3( 0.0f,  0.0f,  0.0f);
	}


	//!
	/*!
	 *
	 */
	Mesh::~Mesh()
	{
		//! TODO Auto-generated destructor stub
	}


	//! Initializes vertex-list, index-list, normal-list and uv-list
	/*!
	 *
	 */
	void Mesh::Initialize(void)
	{
		//! Topology initialization
		for(unsigned int v=0; v < mesh_ptr->mNumVertices; v++){
			//! Update the bounding box values
			if (mesh_ptr->mVertices[v].x > m_boundingBox.max.x)
				m_boundingBox.max.x = mesh_ptr->mVertices[v].x;
			if (mesh_ptr->mVertices[v].y > m_boundingBox.max.y)
				m_boundingBox.max.y = mesh_ptr->mVertices[v].y;
			if (mesh_ptr->mVertices[v].z > m_boundingBox.max.z)
				m_boundingBox.max.z = mesh_ptr->mVertices[v].z;

			if (mesh_ptr->mVertices[v].x < m_boundingBox.min.x)
				m_boundingBox.min.x = mesh_ptr->mVertices[v].x;
			if (mesh_ptr->mVertices[v].y < m_boundingBox.min.y)
				m_boundingBox.min.y = mesh_ptr->mVertices[v].y;
			if (mesh_ptr->mVertices[v].z < m_boundingBox.min.z)
				m_boundingBox.min.z = mesh_ptr->mVertices[v].z;


			//! Write vertices
			vertices.push_back(mesh_ptr->mVertices[v].x);
			vertices.push_back(mesh_ptr->mVertices[v].y);
			vertices.push_back(mesh_ptr->mVertices[v].z);
			//! Write normals
			normals.push_back(mesh_ptr->mNormals[v].x);
			normals.push_back(mesh_ptr->mNormals[v].y);
			normals.push_back(mesh_ptr->mNormals[v].z);
			//! Write uv coordinates/texture coordinates if they exist
			if(mesh_ptr->HasTextureCoords(0)){
				uvs.push_back(mesh_ptr->mTextureCoords[0][v].x);
				uvs.push_back(mesh_ptr->mTextureCoords[0][v].y);
			}
		}
		for(unsigned int f=0; f < mesh_ptr->mNumFaces; f++){
			for(unsigned int i=0; i < mesh_ptr->mFaces[f].mNumIndices; i++){
				indices.push_back(mesh_ptr->mFaces[f].mIndices[i]);
			}
		}

		//! TODO bounding box stuff
		CreateBoundingBox();
	}


	//! Creates the buffers holding the gemoetry, the normals and the uv coordinates
	/*!
	 *
	 */
	void Mesh::CreateBuffers(void)
	{
		ErrorCheckMesh = glGetError();

		//! Create vertex array object
		glGenVertexArrays(1, &VAO_id);
		glBindVertexArray(VAO_id);

		//! Create vertex buffer object
		glGenBuffers(1, &VBO_id);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//! Create index buffer
		glGenBuffers(1, &IBO_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), &indices[0], GL_STATIC_DRAW);
		//! Create normal buffer
		glGenBuffers(1, &NBO_id);
		glBindBuffer(GL_ARRAY_BUFFER, NBO_id);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		//! Create uv buffer
		glGenBuffers(1, &UVBO_id);
		glBindBuffer(GL_ARRAY_BUFFER, UVBO_id);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), &uvs[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(2);

		ErrorCheckMesh = glGetError();
		if(ErrorCheckMesh != GL_NO_ERROR){
			std::cout << "ERROR | assimp: buffer objects could not be created!" << std::endl;
		}
	}

	//!
	/*!
	 *
	 */
	void Mesh::CreateBoundingBox(void)
	{
		//! Bounding box offset, so cuboids bounding boxes can bee seen
		m_boundingBox.max += 0.1f;
		m_boundingBox.min -= 0.1f;

		//! Create bounding box
		//! Vertex list
		// Bottom
		// A
		m_boundingBox.vertices.push_back(m_boundingBox.min.x);
		m_boundingBox.vertices.push_back(m_boundingBox.min.y);
		m_boundingBox.vertices.push_back(m_boundingBox.min.z);
		// B
		m_boundingBox.vertices.push_back(m_boundingBox.max.x);
		m_boundingBox.vertices.push_back(m_boundingBox.min.y);
		m_boundingBox.vertices.push_back(m_boundingBox.min.z);
		// C
		m_boundingBox.vertices.push_back(m_boundingBox.max.x);
		m_boundingBox.vertices.push_back(m_boundingBox.min.y);
		m_boundingBox.vertices.push_back(m_boundingBox.max.z);
		// D
		m_boundingBox.vertices.push_back(m_boundingBox.min.x);
		m_boundingBox.vertices.push_back(m_boundingBox.min.y);
		m_boundingBox.vertices.push_back(m_boundingBox.max.z);
		// Top
		// E
		m_boundingBox.vertices.push_back(m_boundingBox.min.x);
		m_boundingBox.vertices.push_back(m_boundingBox.max.y);
		m_boundingBox.vertices.push_back(m_boundingBox.min.z);
		// F
		m_boundingBox.vertices.push_back(m_boundingBox.max.x);
		m_boundingBox.vertices.push_back(m_boundingBox.max.y);
		m_boundingBox.vertices.push_back(m_boundingBox.min.z);
		// G
		m_boundingBox.vertices.push_back(m_boundingBox.max.x);
		m_boundingBox.vertices.push_back(m_boundingBox.max.y);
		m_boundingBox.vertices.push_back(m_boundingBox.max.z);
		// H
		m_boundingBox.vertices.push_back(m_boundingBox.min.x);
		m_boundingBox.vertices.push_back(m_boundingBox.max.y);
		m_boundingBox.vertices.push_back(m_boundingBox.max.z);

		//! Index list
		//! Bottom
		m_boundingBox.indices.push_back(0);
		m_boundingBox.indices.push_back(1);
		m_boundingBox.indices.push_back(1);
		m_boundingBox.indices.push_back(2);
		m_boundingBox.indices.push_back(2);
		m_boundingBox.indices.push_back(3);
		m_boundingBox.indices.push_back(3);
		m_boundingBox.indices.push_back(0);
		//! Top
		m_boundingBox.indices.push_back(4);
		m_boundingBox.indices.push_back(5);
		m_boundingBox.indices.push_back(5);
		m_boundingBox.indices.push_back(6);
		m_boundingBox.indices.push_back(6);
		m_boundingBox.indices.push_back(7);
		m_boundingBox.indices.push_back(7);
		m_boundingBox.indices.push_back(4);
		//! Sides
		m_boundingBox.indices.push_back(0);
		m_boundingBox.indices.push_back(4);
		m_boundingBox.indices.push_back(1);
		m_boundingBox.indices.push_back(5);
		m_boundingBox.indices.push_back(2);
		m_boundingBox.indices.push_back(6);
		m_boundingBox.indices.push_back(3);
		m_boundingBox.indices.push_back(7);

		//! Bounding box buffers
		glGenVertexArrays(1, &BBVAO_id);
		glBindVertexArray(BBVAO_id);
		//! Create vertex buffer object for bounding box
		glGenBuffers(1, &BBO_id);
		glBindBuffer(GL_ARRAY_BUFFER, BBO_id);
		glBufferData(GL_ARRAY_BUFFER, m_boundingBox.vertices.size() * sizeof(GLfloat), &(m_boundingBox.vertices[0]), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//! Create index buffer
		glGenBuffers(1, &BBIO_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BBIO_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_boundingBox.indices.size() * sizeof(GLint), &(m_boundingBox.indices[0]), GL_STATIC_DRAW);

	}


	//!
	/*!
	 *
	 */
	void Mesh::Draw(void)
	{
		AccumulateModelMatrix();
		glBindVertexArray(VAO_id);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}


	//! Draws the bounding box
	/*!
	 *
	 */
	void Mesh::DrawBoundingBox(void)
	{
		AccumulateModelMatrix();
		glDisableClientState(GL_LIGHTING);

		glBindVertexArray(BBVAO_id);
		glDrawElements(GL_LINES, m_boundingBox.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glEnableClientState(GL_LIGHTING);
	}


	//! Set the mesh's material
	/*!
	 *
	 */
	void Mesh::SetMaterial(Material* m)
	{
		material_ptr = m;
	}


	//!
	/*!
	 *
	 * @return
	 */
	Material* Mesh::GetMaterial(void)
	{
		if(material_ptr != 0)
			return material_ptr;
		return 0;
	}


	//!
	/*!
	 *
	 * @return
	 */
	bool Mesh::HasMaterial(void)
	{
		if(material_ptr != 0)
			return true;
		return false;
	}


	//!
	/*!
	 *
	 * @return
	 */
	GLuint Mesh::GetTextureHandle(void)
	{
		return material_ptr->GetTexture();
	}


} //! namespace scene
