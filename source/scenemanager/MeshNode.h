/*!	Mesh node class definition
 *	@author Guido Schmidt
 *	@date 	26.12.12
 */

#ifndef MESHNODE_H_
#define MESHNODE_H_

//! Local includes
#include "Node.h"
//! C++ includes
#include <vector>
//! OpenGL includes
#include <GL/gl.h>

namespace scenegraph {

	class MeshNode : public Node{
		private:
			//! Topology vectors
			std::vector<GLfloat*> vertices;
			std::vector<GLuint*> indices;
			std::vector<GLfloat*> normals;
			std::vector<GLfloat*> uvcoords;
			//! VBO handlers
			GLuint VAO_ID;
			GLuint VBO_ID, IBO_ID, NBO_ID, UVBO_ID;

		public:
			MeshNode();
			~MeshNode();

			//! Topology methods
			void AddVertex(float*);
			void AddIndex(unsigned int*);
			void AddNormal(float*);
			void AddUVCoord(float*);
	};

} //! namespace scenegraph
#endif //! MESHNODE_H_
