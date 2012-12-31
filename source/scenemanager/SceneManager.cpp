/*!	Scene manager class implementation
 *	@author Guido Schmidt
 *	@date 	25.12.12
 */

#include "SceneManager.h"

namespace scenegraph {

	SceneManager::SceneManager()
	{
		root = 0;
		temp = 0;
		scene = 0;
	}

	//!
	/*!
	 *
	 * @param filename
	 * @return
	 */
	bool SceneManager::LoadScene(const std::string filename)
	{
		std::ifstream infile(filename.c_str());
		if(!infile.fail()){
			scene = aiImporter.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
			infile.close();
			return true;
		}
		else{
			std::cout << "ERROR | assimp: file " << filename << " was not found or could not be read!" << std::endl;
			infile.close();
		}

		ProcessScene(scene);
		return false;
	}

	//!
	/*!
	 *
	 * @param scene
	 */
	void SceneManager::ProcessScene(const aiScene* scene)
	{
		if(!scene){
			std::cout << "ERROR | assimp: not valid scene file!" << std::endl;
		}
		else{
			root = new Node();
			temp = new MeshNode();

			//! Mesh Processing
//			GLuint tempVAO_ID;
//			GLuint tempVBO_ID, tempIBO_ID, tempNBO_ID, tempUVBO_ID;

			for(unsigned int m = 0; m < scene->mNumMeshes; m++)
			{
				aiMesh* currentMesh = scene->mMeshes[m];
				for(unsigned int v = 0; v < currentMesh->mNumVertices; v++)
				{
					//! Write vertices
					temp->AddVertex(&(currentMesh->mVertices[v].x));
					temp->AddVertex(&(currentMesh->mVertices[v].y));
					temp->AddVertex(&(currentMesh->mVertices[v].z));
					//! Write normals
					temp->AddNormal(&(currentMesh->mNormals[v].x));
					temp->AddNormal(&(currentMesh->mNormals[v].y));
					temp->AddNormal(&(currentMesh->mNormals[v].z));
					if(currentMesh->HasTextureCoords(0))
					{
						temp->AddUVCoord(&(currentMesh->mTextureCoords[0][v].x));
						temp->AddUVCoord(&(currentMesh->mTextureCoords[0][v].y));
					}
				}
				for(unsigned int f = 0; f < currentMesh->mNumFaces; f++)
				{
					//! Write indices
					for(unsigned int i = 0; i < currentMesh->mFaces[f].mNumIndices; i++)
					{
						temp->AddIndex(&(currentMesh->mFaces[f].mIndices[i]));
					}
				}
			}

			root->AddChild(temp);
		}
	}

} //! namespace scenegraph
