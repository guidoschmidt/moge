/*
 * Node.cpp
 *
 *  Created on: 02.01.2013
 *      Author: guidoschmidt
 */

#include "Node.h"

namespace scene {

	Node::Node()
	{
		parent = 0;
	}

	Node::~Node()
	{
	}

	//! Set the node's parent node
	void Node::SetParent(Node* p)
	{
		parent = p;
	}

	//! Get the node's parent node
	Node* Node::GetParent()
	{
		return parent;
	}

	//! Add a child node
	void Node::AddChild(Node* c)
	{
		children.push_back(c);
	}

	//! Get a child node by index
	Node* Node::GetChild(int i)
	{
		return children[i];
	}

	//!
	unsigned int Node::ChildrenCount(void)
	{
		return children.size();
	}

	void Node::Draw(void)
	{

	}

} //! namespace scene
