/*
 * FSQ.h
 *
 *  Created on: 01.12.2012
 *      Author: guidoschmidt
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
		GLuint FSQVAO_ID, FSQVBO_ID, FSQIndexBuffer_ID, FSQUVBuffer_ID;
		void CreateBuffers(void);
		void DeleteBuffers(void);

	public:
		FSQ();
		~FSQ();
		void CreateFSQ(void);
		void Draw(void);
};

#endif /* FSQ_H_ */
