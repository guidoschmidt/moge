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
		GLuint* textureID = LoadTexture(texturefile);
		textures.push_back(textureID);
	}


	//! Loads a texture
	/*!
	 *
	 * @param filename
	 * @return
	 */
	GLuint* MaterialManager::LoadTexture(std::string filename)
	{
		GLuint initTextureID;
		//! TODO Load texture (assimp or DevIL)
		return &initTextureID;
	}

} //! namespace scene
