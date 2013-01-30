//! Node.h
/*!
 * @date	02.01.2013
 * @author 	Guido Schmidt
 */

#ifndef NODE_H_
#define NODE_H_

#include <vector>
#include "Transformation.h"

namespace scene {

	class Node : public Transformation
	{
		private:

		protected:
			//! Hierarchy
			Node* parent;
			std::vector<Node*> children;

		public:
			Node();
			virtual ~Node();

			//! Parent
			void SetParent(Node* p);
			Node* GetParent();

			//! Children
			void AddChild(Node* c);
			Node* GetChild(int i);
			unsigned int ChildrenCount(void);

			virtual void Draw(void);
		};

} //! namespace scene
#endif //! NODE_H_
