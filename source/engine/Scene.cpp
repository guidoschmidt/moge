/*!	Scene class
 *	@author Guido Schmidt
 *	@date 06.12.12
 *
 */

#include "Scene.h"

//! Contructor
Scene::Scene()
{
	scene = 0;
	mesh = 0;
	VAO_ID = 0;
	VBO_ID = 0;
	IndexBuffer_ID = 0;
	NormalBuffer_ID = 0;
	UVBuffer_ID = 0;
	Image_ID = 0;
	ErrorCheckMesh = 0;
	ErrorCheckTexture = 0;
	Texture_ID = 0;

	camera0 = new Camera();
}

//! Destructor
Scene::~Scene()
{
}

//! Import a 3D file
/*!
 * Imports a 3D file using asset importer (assimp).
 */
void Scene::Import3DModel(const std::string filename){
	std::ifstream infile(filename.c_str());
	if(!infile.fail()){
		scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
		infile.close();
	}
	else{
		std::cout << "ERROR | assimp: file " << filename << " was not found or could not be read!" << std::endl;
		infile.close();
	}
	if(!scene){
		std::cout << "ERROR | assimp: could not import " << filename << "!" << std::endl;
	}
	else{


		mesh = scene->mMeshes[0];

		for(unsigned int v=0; v < mesh->mNumVertices; v++){
			//! Write vertices
			vertices.push_back(mesh->mVertices[v].x);
			vertices.push_back(mesh->mVertices[v].y);
			vertices.push_back(mesh->mVertices[v].z);
			//! Write normals
			normals.push_back(mesh->mNormals[v].x);
			normals.push_back(mesh->mNormals[v].y);
			normals.push_back(mesh->mNormals[v].z);
			//! Write uv coordinates/texture coordinates if they exist
			if(mesh->HasTextureCoords(0)){
				uvs.push_back(mesh->mTextureCoords[0][v].x);
				uvs.push_back(mesh->mTextureCoords[0][v].y);
			}
		}
		for(unsigned int f=0; f < mesh->mNumFaces; f++){
			for(unsigned int i=0; i < mesh->mFaces[f].mNumIndices; i++){
				indices.push_back(mesh->mFaces[f].mIndices[i]);
			}
		}
		std::cout << "assimp: Import of " << filename << " was successful!" << std::endl;
	}

	CreateBufferObjects();
}

//! Create OpenGL buffers
/*!
 * Creates the necessary buffers.
 */
void Scene::CreateBufferObjects(void){
	ErrorCheckMesh = glGetError();

	//! Create vertex array object
	glGenVertexArrays(1, &VAO_ID);
	glBindVertexArray(VAO_ID);

	//! Create vertex buffer object
	glGenBuffers(1, &VBO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//! Create index buffer
	glGenBuffers(1, &IndexBuffer_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), &indices[0], GL_STATIC_DRAW);
	//! Create normal buffer
	glGenBuffers(1, &NormalBuffer_ID);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer_ID);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	//! Create uv buffer
	glGenBuffers(1, &UVBuffer_ID);
	glBindBuffer(GL_ARRAY_BUFFER, UVBuffer_ID);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), &uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(2);

	ErrorCheckMesh = glGetError();
	if(ErrorCheckMesh != GL_NO_ERROR){
		std::cout << "ERROR | assimp: buffer objects could not be created!" << std::endl;
	}
}

//! Loads a texture
/*!
 * Loads a texture file from the given filename.
 * @param filename
 */
void Scene::LoadTexture(const std::string filename){
//	ilutRenderer(ILUT_OPENGL);
//	ilutRenderer(ILUT_OPENGL);
//	ilInit();
//	iluInit();
//	ilutInit();
//
//	ILboolean loadSuccess = ilLoadImage(filename.c_str());
//
//	if(loadSuccess){
//		glGenTextures(1, &Texture_ID);
//		glBindTexture(GL_TEXTURE_2D, Texture_ID);
//		//! Set texture's clamping
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//		//! Set texture's filtering
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		//! Load image into texture
//		glTexImage2D(	GL_TEXTURE_2D,
//						0,
//						ilGetInteger(IL_IMAGE_BPP),
//						ilGetInteger(IL_IMAGE_WIDTH),
//						ilGetInteger(IL_IMAGE_HEIGHT),
//						0,
//						ilGetInteger(IL_IMAGE_FORMAT),
//						GL_UNSIGNED_BYTE,
//						ilGetData());
//		//! Delete image
//		ilDeleteImages(1, &Image_ID);
//		std::cout << "DeVIL: texture was generated from " << filename << "!" << std::endl;
//	}
//	else if(!loadSuccess){
//		ErrorCheckTexture = ilGetError();
//		std::cout << "ERROR | DeVIL: Image load error " << iluErrorString(ErrorCheckTexture) << std::endl;
//	}

	textures.push_back(Texture_ID);

//	return Texture_ID;
}

//! Returns the texture
/*!
 *
 * @param index
 * @return texture by index
 */
GLuint Scene::GetTexture(unsigned int index){
	if(textures.size() == 0)
		return 0;
	if(index > (textures.size()-1))
		std::cout << "ERROR | Scene: mesh only has " << textures.size() << " textures!" << std::endl;
	return textures[index];
}

//! Draws the scene
/*!
 * Draws the scene's objects.
 */
void Scene::Draw(void){

	glBindVertexArray(VAO_ID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
