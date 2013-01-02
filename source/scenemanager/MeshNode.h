/*!	Mesh node class definition
 *	@author Guido Schmidt
 *	@date 	26.12.12
 */

#ifndef MESHNODE_H_
#define MESHNODE_H_

//! C++ includes
#include <vector>
#include <iostream>
//! OpenGL includes
#include <GL/glew.h>
#include <GL/gl.h>

namespace scenegraph {

	class MeshNode{
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
			//! Buffer objects
			void CreateBuffers(void);

			void Draw(void);
	};

} //! namespace scenegraph
#endif //! MESHNODE_H_
