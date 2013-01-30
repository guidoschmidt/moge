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
		textureCounter = 0; //0 is black texture
		m_textures.push_back(new texture());
		LoadTexture("./assets/texture/Black.png");
		textureCounter = 1;
		materialCounter = 0;

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
	void MaterialManager::AddMaterial(std::string name)
	{
		m_materials.push_back(new Material(materialCounter, name));
		materialCounter++;
	}


	//! Adds a new material to the material-stock
	/*!
	 *
	 * @param name
	 * @param texturefile
	 */
	void MaterialManager::AddMaterial(std::string name, float reflectivity, std::vector<texture> textures)
	{
		m_materials.push_back(new Material(materialCounter, name));
		for(unsigned int tex = 0; tex < textures.size(); tex++)
		{
			m_textures.push_back(new texture());
			m_textures[textureCounter]->m_handle = textureCounter;
			m_textures[textureCounter]->m_type = textures[tex].m_type;
			m_textures[textureCounter]->m_filename = textures[tex].m_filename;
			LoadTexture(m_textures[textureCounter]->m_filename);

			textureRef texref(textureCounter, m_textures[textureCounter]->m_type);
			m_materials[materialCounter]->AddTexture(texref);
			m_materials[materialCounter]->SetReflectance(reflectivity);

			textureCounter++;
		}
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
	void MaterialManager::LoadTexture(std::string filename)
	{
		ILboolean loadSuccess = ilLoadImage(filename.c_str());

		if(loadSuccess)
		{
			glActiveTexture(GL_TEXTURE0);
			glGenTextures(1, &(m_textures[textureCounter]->m_handle));
			glBindTexture(GL_TEXTURE_2D, m_textures[textureCounter]->m_handle);
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
	}


	//!
	/*!
	 *
	 * @return
	 */
	void MaterialManager::LoadCubeMap(std::string filename)
	{
		ILboolean loadSuccess;
		//cubemaps.push_back(cubemap_id);

		glActiveTexture(GL_TEXTURE0);
		//glGenTextures(1, &cubemaps[cubemapCounter]);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaps[cubemapCounter]);

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
		for(unsigned int i = 0; i < m_materials.size(); i++)
		{
			if(m_materials[i]->GetName() == searchname)
				return m_materials[i];
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
		return m_materials[index];
	}


	//!
	unsigned int MaterialManager::MaterialCount(void)
	{
		return m_materials.size();
	}


	//!
	/*!
	 *
	 */
	GLuint* MaterialManager::GetTextureByID(int i)
	{
		return &(m_textures[i]->m_handle);
	}

} //! namespace scene
