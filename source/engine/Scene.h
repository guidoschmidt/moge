//!
/*! */

#ifndef SCENE_H_
#define SCENE_H_

//! C++ includes
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
//! OpenGL includes
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
//! Assimp includes
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
//! DevIL includes
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
//! Local includes
#include "Camera.h"

class Scene
{
	private:
		const aiScene* scene;
		const aiMesh* mesh;
		Camera* camera0;

		std::vector<aiMesh> meshes;
		Assimp::Importer importer;

		//! Vectors storing mesh data
		std::vector<GLfloat> vertices;
		std::vector<GLint> indices;
		std::vector<GLfloat> normals;
		std::vector<GLfloat> uvs;
		std::vector<GLuint> textures;

		//! OpenGL handles for mesh
		GLenum ErrorCheckMesh;
		GLuint VAO_ID;
		GLuint VBO_ID, IndexBuffer_ID, NormalBuffer_ID, UVBuffer_ID;

		//! OpenGL handles for texture
		GLuint Texture_ID;
		//! DevIL handles for texture
		ILenum ErrorCheckTexture;
		ILuint Image_ID;

	protected:

	public:
		Scene();
		~Scene();
		GLuint GetTexture(unsigned int index);
		void Import3DModel(const std::string filename);
		void CreateBufferObjects(void);
		void LoadTexture(const std::string filename);
		void Draw(void);

		//! Getter
		Camera GetCamera(int number);
};

#endif //! SCENE_H_
