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
	MeshNode::MeshNode() {
		Node();
		VAO_ID 	= 0;
		VBO_ID 	= 0;
		IBO_ID 	= 0;
		NBO_ID 	= 0;
		UVBO_ID = 0;
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

} //! namespace scenegraph
