/*
 * SceneObject.h
 *
 *  Created on: 06.12.2012
 *      Author: guidoschmidt
 */

#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

//! C++ includes
#include <vector>
//! OpenGL includes
#include <GL/gl.h>
//! assimp includes
#include <assimp/mesh.h>
//! GLM includes
#include <glm/glm.hpp>

class SceneObject
{
	private:
		GLuint VAO_ID;
		GLuint VBO_ID, IndexBuffer_ID, NormalBuffer_ID, UVBuffer_ID;
		std::vector<GLfloat> vertices, normals, uvs;
		std::vector<GLint> indices;

		aiMesh* mesh;

		glm::vec3 position;

	public:
		SceneObject();
		~SceneObject();
		void CreateBuffers();
		void Draw();
};

#endif /* SCENEOBJECT_H_ */
