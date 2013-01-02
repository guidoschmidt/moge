//! Mesh.cpp
/*!
 * @date 	02.01.2013
 * @author	Guido Schmidt
 */

#include "Mesh.h"

namespace scene {

	//! Constructor
	Mesh::Mesh(aiMesh* m)
	{
		mesh = m;
		Initialize();
		CreateBuffers();
	}

	Mesh::~Mesh()
	{
		//! TODO Auto-generated destructor stub
	}

	//! Initializes vertex-list, index-list, normal-list and uv-list
	void Mesh::Initialize(void)
	{
		for(unsigned int v=0; v < mesh->mNumVertices; v++){
			//! Write vertices
			vertices.push_back(mesh->mVertices[v].x);
			vertices.push_back(mesh->mVertices[v].y);
			vertices.push_back(mesh->mVertices[v].z);
			//! Write normals
			normals.push_back(mesh->mNormals[v].x);
			normals.push_back(mesh->mNormals[v].y);
			normals.push_back(mesh->mNormals[v].z);
			//! Write uv coordinates/texture coordinates if they exist
			if(mesh->HasTextureCoords(0)){
				uvs.push_back(mesh->mTextureCoords[0][v].x);
				uvs.push_back(mesh->mTextureCoords[0][v].y);
			}
		}
		for(unsigned int f=0; f < mesh->mNumFaces; f++){
			for(unsigned int i=0; i < mesh->mFaces[f].mNumIndices; i++){
				indices.push_back(mesh->mFaces[f].mIndices[i]);
			}
		}
	}

	//! Creates the buffers holding the gemoetry, the normals and the uv coordinates
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
	void Mesh::Draw(void)
	{
		glBindVertexArray(VAO_id);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

} //! namespace scene
