/*
 * MeshNode.cpp
 *
 *  Created on: 26.12.2012
 *      Author: guidoschmidt
 */

#include "MeshNode.h"

namespace scenegraph {

	//! Constructor
	/*!
	 *
	 */
	MeshNode::MeshNode()
	{
		VAO_ID 	= 0;
		VBO_ID 	= 0;
		IBO_ID 	= 0;
		NBO_ID 	= 0;
		UVBO_ID = 0;
	}

	MeshNode::~MeshNode(){

	}

	//!
	void MeshNode::AddVertex(float* vertex)
	{
		vertices.push_back(vertex);
	}

	//!
	void MeshNode::AddIndex(unsigned int* index)
	{
		indices.push_back(index);
	}

	//!
	void MeshNode::AddNormal(float* normal)
	{
		normals.push_back(normal);
	}

	//!
	void MeshNode::AddUVCoord(float* uvcoord)
	{
		uvcoords.push_back(uvcoord);
	}

	//!
	/*!
	 *
	 */
	void MeshNode::CreateBuffers(void)
	{
		static GLenum ErrorCheckMesh = glGetError();

			//! Create vertex array object
			glGenVertexArrays(1, &VAO_ID);
			glBindVertexArray(VAO_ID);

			//! Create vertex buffer object
			glGenBuffers(1, &VBO_ID);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			glEnableVertexAttribArray(0);
			//! Create index buffer
			glGenBuffers(1, &IBO_ID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_ID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), &indices[0], GL_STATIC_DRAW);
			//! Create normal buffer
			glGenBuffers(1, &NBO_ID);
			glBindBuffer(GL_ARRAY_BUFFER, NBO_ID);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			glEnableVertexAttribArray(1);
			//! Create uv buffer
			glGenBuffers(1, &UVBO_ID);
			glBindBuffer(GL_ARRAY_BUFFER, UVBO_ID);
			glBufferData(GL_ARRAY_BUFFER, uvcoords.size() * sizeof(GLfloat), &uvcoords[0], GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			glEnableVertexAttribArray(2);

			ErrorCheckMesh = glGetError();
			if(ErrorCheckMesh != GL_NO_ERROR)
			{
				std::cout << "ERROR | assimp: buffer objects could not be created!" << std::endl;
			}
			else
				std::cout << "Buffers created" << std::endl;

	}

	//!
	/*!
	 *
	 */
	void MeshNode::Draw(void)
	{
//		CreateBuffers();

//		glBindVertexArray(VAO_ID);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}
} //! namespace scenegraph
