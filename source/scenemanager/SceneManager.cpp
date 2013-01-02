/*!	Scene manager class implementation
 *	@author Guido Schmidt
 *	@date 	25.12.12
 */

#include "SceneManager.h"

namespace scenegraph {

	SceneManager::SceneManager()
	{
		scene = 0;
		sceneLoaded = false;
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
			ProcessScene(scene);
			infile.close();
			return true;
		}
		else{
			std::cout << "ERROR | assimp: file " << filename << " was not found or could not be read!" << std::endl;
			infile.close();
		}
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
			for(unsigned int m = 0; m < scene->mNumMeshes; m++)
			{
				aiMesh* currentMesh = scene->mMeshes[0];
				MeshNode* x = new MeshNode();

				for(unsigned int v = 0; v < currentMesh->mNumVertices; v++)
				{
					//! Write vertices
					x->AddVertex(&(currentMesh->mVertices[v].x));
					x->AddVertex(&(currentMesh->mVertices[v].y));
					x->AddVertex(&(currentMesh->mVertices[v].z));
					//! Write normals
					x->AddNormal(&(currentMesh->mNormals[v].x));
					x->AddNormal(&(currentMesh->mNormals[v].y));
					x->AddNormal(&(currentMesh->mNormals[v].z));
					if(currentMesh->HasTextureCoords(0))
					{
						x->AddUVCoord(&(currentMesh->mTextureCoords[0][v].x));
						x->AddUVCoord(&(currentMesh->mTextureCoords[0][v].y));
					}
				}
				for(unsigned int f = 0; f < currentMesh->mNumFaces; f++)
				{
					//! Write indices
					for(unsigned int i = 0; i < currentMesh->mFaces[f].mNumIndices; i++)
					{
						x->AddIndex(&(currentMesh->mFaces[f].mIndices[i]));
					}
				}
				root.AddChild(*x);
				x->CreateBuffers();
			}

			sceneLoaded = true;
			std::cout << "assimp: " << scene << " loaded successfully" << std::endl;
		}
	}


	//!
	/*!
	 *
	 */
	void SceneManager::Draw(void)
	{
		if(sceneLoaded){
			for(unsigned int i = 0; i < root.ChildrenSize(); i++)
			{
				root.GetChild(i).Draw();
			}
		}
		//! TODO SceneManager draw implementation
	}

} //! namespace scenegraph
