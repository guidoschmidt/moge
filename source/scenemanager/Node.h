/*!	Node class definition
 *	@author Guido Schmidt
 *	@date 	24.12.12
 */

#ifndef NODE_H_
#define NODE_H_

#include <vector>

namespace scenegraph {

class Node {
	private:
		bool childStatus;
		Node* parent;
		std::vector<Node*> childs;

	public:
		Node();
		virtual ~Node();

		bool AddChild(Node n);
		bool DeleteChild(Node n);
		bool DeleteChild(int n);
		Node* GetChild(int n);

		bool IsChild(void);
		bool IsParent(void);
};

} /*! namespace scenegraph */
#endif //! NODE_H_
