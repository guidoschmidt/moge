
#include "Node.h"

namespace scenegraph{

	//! Constructor
	Node::Node(){
		childStatus = 0;
		parent = 0;
	}

	//!
	void Node::AddChild(Node* n){
		children.push_back(n);
	}

	//!
	void Node::DeleteChild(Node* n){
		//! TODO Implementation
	}

	void Node::DeleteChild(int n){
		//! TODO
	}

	//!
	inline bool Node::IsChild(void){ return childStatus; }

	//!
	inline bool Node::IsParent(void){ if(parent != 0) return true; else return false;}

}



