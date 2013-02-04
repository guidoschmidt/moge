//! Node.cpp
/*!
 * @date	02.01.2013
 * @author	Guido Schmidt
 */

#include "Node.h"

namespace scene {

	Node::Node()
	{
		m_parent = 0;
		m_type = "Node";
	}

	Node::~Node()
	{
	}

	//! Set the node's parent node
	void Node::SetParent(Node* p)
	{
		m_parent = p;
	}

	//! Get the node's parent node
	Node* Node::GetParent()
	{
		return m_parent;
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

	//!
	std::string Node::GetType(void)
	{
		return m_type;
	}

} //! namespace scene
