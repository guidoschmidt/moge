//! Node.h
/*!
 * @date	02.01.2013
 * @author 	Guido Schmidt
 */

#ifndef NODE_H_
#define NODE_H_

//! C++ includes
#include <vector>
#include <string>
//! Local includes
#include "Transformation.h"

namespace scene {

	class Node : public Transformation
	{
		private:

		protected:
			//! Hierarchy
			Node* m_parent;
			std::vector<Node*> children;
			std::string m_type;

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

			std::string GetType(void);
		};

} //! namespace scene
#endif //! NODE_H_
