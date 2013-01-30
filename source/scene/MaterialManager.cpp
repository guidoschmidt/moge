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
		materialCounter = 0;
		cubemapCounter = 0;

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
	 * @param name
	 * @param texturefile
	 */
	void MaterialManager::AddMaterial(std::string name, std::string texturefile, float reflectivity)
	{
		GLuint textureID = LoadTexture(texturefile);
		materials.push_back(new Material(materialCounter, name, textureCounter, &textureID));
		materials[materialCounter]->SetReflectivity(reflectivity);
		materialCounter++;
		textureCounter++;
	}

	//! Adds a new material to the material-stock
	/*!
	 *
	 * @param name
	 * @param texturefile
	 */
	void MaterialManager::AddMaterial(std::string name, std::string texturefile, std::string normalmap, float reflectivity)
	{
		GLuint textureID = LoadTexture(texturefile);
		GLuint normalmapID = LoadTexture(normalmap);
		materials.push_back(new Material(materialCounter, name, textureCounter, &textureID, textureCounter++, &normalmapID));
		materials[materialCounter]->SetReflectivity(reflectivity);
		materialCounter++;
	}


	//! Adds a new material to the material-stock
	/*!
	 *
	 * @param name
	 * @param texturefile
	 */
	void MaterialManager::AddMaterial(std::string name)
	{
		materials.push_back(new Material(materialCounter, name));
		materialCounter++;
	}

	//!
	/*!
	 *
	 * @param filename
	 */
	void MaterialManager::AddCubeMap(std::string filename)
	{
		LoadCubeMap(filename);
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

		ILboolean loadSuccess = ilLoadImage(filename.c_str());

		if(loadSuccess)
		{
			glActiveTexture(GL_TEXTURE0);
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
		else if(!loadSuccess)
		{
			ErrorCheckTexture = ilGetError();
			std::cout << "ERROR | DeVIL: Image load error " << iluErrorString(ErrorCheckTexture) << std::endl;
		}

		return textures[textureCounter];
	}

	//!
	/*!
	 *
	 * @return
	 */
	void MaterialManager::LoadCubeMap(std::string filename)
	{
		GLuint cubemap_id;
		ILboolean loadSuccess;
		cubemaps.push_back(cubemap_id);

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &cubemaps[cubemapCounter]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaps[cubemapCounter]);

		std::string suffixes[] = {"posx", "negx", "posy", "negy", "posz", "negz"};

		GLuint cubemap_targets[] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		};

		//! Load cubemap images
		for(int i=0; i < 6; i++)
		{
			std::string m_filename = filename + "_" + suffixes[i] + ".png";
			loadSuccess = ilLoadImage(m_filename .c_str());
			glTexImage2D(	cubemap_targets[i],
							0,
							ilGetInteger(IL_IMAGE_BPP),
							ilGetInteger(IL_IMAGE_WIDTH),
							ilGetInteger(IL_IMAGE_HEIGHT),
							0,
							ilGetInteger(IL_IMAGE_FORMAT),
							GL_UNSIGNED_BYTE,
							ilGetData());
			if(!loadSuccess)
			{
				ErrorCheckTexture = ilGetError();
				std::cout << "ERROR | DeVIL: Image load error " << iluErrorString(ErrorCheckTexture) << std::endl;
			}
			else
				std::cout << "DeVIL: cubemap was generated from " << m_filename << "!" << std::endl;
		}
		//! Typical cube map settings
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}


	//!
	/*!
	 *
	 * @param name
	 * @return
	 */
	Material* MaterialManager::GetMaterial(std::string searchname)
	{
		for(unsigned int i = 0; i < materials.size(); i++)
		{
			if(materials[i]->GetName() == searchname)
				return materials[i];
		}
		return 0;
	}

	//!
	/*!
	 *
	 * @param index
	 * @return
	 */
	Material* MaterialManager::GetMaterial(unsigned int index)
	{
		//std::cout << "GetMaterial counter = " << materialCounter << std::endl;
		return materials[index];
	}


	//!
	unsigned int MaterialManager::MaterialCount(void)
	{
		return materials.size();
	}


	//!
	/*!
	 *
	 */
	GLuint* MaterialManager::GetTexture(int i)
	{
		return &textures[i];
	}


	//!
	/*!
	 *
	 */
	GLuint* MaterialManager::GetCubeMap(int i)
	{
		return &cubemaps[i];
	}

} //! namespace scene
