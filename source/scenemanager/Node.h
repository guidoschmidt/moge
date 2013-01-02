/*!	Node class definition
 *	@author Guido Schmidt
 *	@date 	24.12.12
 */

#ifndef NODE_H_
#define NODE_H_

#include <vector>
#include "MeshNode.h"

namespace scenegraph {

class Node {
	private:
		bool childStatus;
		Node* parent;
		std::vector<MeshNode> children;

	public:
		//! constructor and destructor
		Node();
		virtual ~Node();

		//! Methods for children access and graph stuff
		void AddChild(MeshNode n);
		void DeleteChild(Node* n);
		void DeleteChild(int n);
		MeshNode GetChild(int n);
		std::vector<Node*>* GetChildren(void);
		unsigned int ChildrenSize(void);

		bool HasChildren(void);
		bool HasParent(void);

		//! Abstract methods
		virtual void Draw(void){};
};

} /*! namespace scenegraph */
#endif //! NODE_H_
