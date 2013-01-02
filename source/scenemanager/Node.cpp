
#include "Node.h"

namespace scenegraph{

	//! Constructor
	Node::Node(){
		childStatus = 0;
		parent = 0;
	}

	//!
	Node::~Node(){

	}

	//!
	void Node::AddChild(MeshNode n){
		children.push_back(n);
	}

	//!
	void Node::DeleteChild(Node* n){
		//! TODO Implementation
	}

	//!
	void Node::DeleteChild(int n){
		//! TODO
	}

	//! Returns a point to the i-th child
	MeshNode Node::GetChild(int i)
	{
		return children[i];
	}

	//! Returns a pointer to the children
	std::vector<Node*>* Node::GetChildren(void)
	{
		//return &children;
	}

	//! Returns the number of children
	unsigned int Node::ChildrenSize(void)
	{
		return children.size();
	}

	//!	Check if the Node is an endchild
	inline bool Node::HasChildren(void){ if(children.empty()) return true; else return false; }

	//! Check if the Node has a parent node
	inline bool Node::HasParent(void){ if(parent != 0) return true; else return false;}

}



