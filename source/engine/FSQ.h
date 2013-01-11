//! FSQ.h
/*!
 * @date 	01.12.2012
 * @author 	Guido Schmidt
 */

#ifndef FSQ_H_
#define FSQ_H_

//! OpenGL includes
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

class FSQ
{
	private:
		GLuint m_VAO_ID, m_VBO_ID, m_IBO_ID, m_UVBO_ID;
		void CreateBuffers(void);
		void DeleteBuffers(void);

	public:
		FSQ();
		~FSQ();
		void CreateFSQ(void);
		void Draw(void);
};

#endif /* FSQ_H_ */
