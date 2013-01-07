//! MaterialManager.cpp
/*!
 * @date	05.01.2013
 * @author	Guido Schmidt
 */

#include "MaterialManager.h"

namespace scene {

	//! Constructor
	/*!
	 *
	 */
	MaterialManager::MaterialManager()
	{
		textureCounter = 0;

		ErrorCheckTexture = 0;
		Image_id = 0;
	}


	//! Destructor
	/*!
	 *
	 */
	MaterialManager::~MaterialManager()
	{
		// TODO Auto-generated destructor stub
	}


	//! Adds a new material to the material-stock
	/*!
	 *
	 * @param material
	 * @param texturefile
	 */
	void MaterialManager::AddMaterial(Material* material, std::string texturefile)
	{
		materials.push_back(material);
		GLuint textureID = LoadTexture(texturefile);
		textures.push_back(textureID);
	}


	//! Loads a texture
	/*!
	 *
	 * @param filename
	 * @return
	 */
	GLuint MaterialManager::LoadTexture(std::string filename)
	{
		GLuint texture_id;
		textures.push_back(texture_id);
		//! TODO Load texture (assimp or DevIL)
		ILboolean loadSuccess = ilLoadImage(filename.c_str());

		if(loadSuccess){
			glActiveTexture(GL_TEXTURE5);
			glGenTextures(1, &textures[textureCounter]);
			glBindTexture(GL_TEXTURE_2D, textures[textureCounter]);
			//! Set texture's clamping
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			//! Set texture's filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//! Load image into texture
			glTexImage2D(	GL_TEXTURE_2D,
							0,
							ilGetInteger(IL_IMAGE_BPP),
							ilGetInteger(IL_IMAGE_WIDTH),
							ilGetInteger(IL_IMAGE_HEIGHT),
							0,
							ilGetInteger(IL_IMAGE_FORMAT),
							GL_UNSIGNED_BYTE,
							ilGetData());
			//! Delete image
			ilDeleteImages(1, &Image_id);
			std::cout << "DeVIL: texture was generated from " << filename << "!" << std::endl;
		}
		else if(!loadSuccess){
			ErrorCheckTexture = ilGetError();
			std::cout << "ERROR | DeVIL: Image load error " << iluErrorString(ErrorCheckTexture) << std::endl;
		}

		textureCounter++;
		return textures[textureCounter-1];
	}


	//!
	/*!
	 *
	 */
	GLuint* MaterialManager::GetTexture(int i)
	{
		return &textures[i];
	}

} //! namespace scene
