//! MaterialManager.cpp
/*!
 * @date	05.01.2013
 * @author	Guido Schmidt
 */

#include "MaterialManager.h"

namespace scene {

	//! Constructor
	MaterialManager::MaterialManager()
	{
		m_textureCounter = 0; //0 is black texture
		m_textures.push_back(new texture());
		LoadTexture("./assets/texture/Black.png");
		m_textureCounter = 1;
		m_materialCounter = 1;

		ErrorCheckTexture = 0;
		Image_id = 0;
	}


	//! Destructor
	MaterialManager::~MaterialManager()
	{
		// TODO Auto-generated destructor stub
	}

	//! Adds a new material to the material-stock
	/*!
	 *
	 *	@param name
	 *	@param texturefile
	 */
	void MaterialManager::AddMaterial(std::string name)
	{
		m_materials.push_back(new Material(m_materialCounter, name));
		m_materialCounter++;
	}


	//! Adds a new material to the material-stock
	/*!
	 *
	 *	@param name
	 *	@param texturefile
	 */
	void MaterialManager::AddMaterial(std::string name, glm::vec3 diffuse, float reflectivity, std::vector<texture> textures)
	{
		m_materials.push_back(new Material(m_materialCounter, name));
		for(unsigned int tex = 0; tex < textures.size(); tex++)
		{
			m_textures.push_back(new texture());
			m_textures[m_textureCounter]->m_handle = m_textureCounter;
			m_textures[m_textureCounter]->m_type = textures[tex].m_type;
			m_textures[m_textureCounter]->m_filename = textures[tex].m_filename;
			LoadTexture(m_textures[m_textureCounter]->m_filename);

			textureRef texref(m_textureCounter, m_textures[m_textureCounter]->m_type);
			m_materials[m_materialCounter-1]->AddTexture(texref);
			m_materials[m_materialCounter-1]->SetReflectance(reflectivity);

			m_textureCounter++;
		}
		m_materials[m_materialCounter-1]->SetDiffuseColor(diffuse);

		std::cout << "Material " << name << " has been added:" << std::endl;
		std::cout << "	ID:	" << m_materialCounter << std::endl;
		std::cout << "	Diffuse Color:	" << diffuse.r << ", " << diffuse.g << ", " << diffuse.b << std::endl;
		std::cout << "	Reflectance:	" << reflectivity << std::endl;
		for(int i=0; i < textures.size(); i++)
		{
			std::cout << "	Texture:	" << textures[i].m_type << ": " << textures[i].m_filename << std::endl;
		}

		m_materialCounter++;
	}

	//! Adds a cube map to the material-stack
	/*!
	 *
	 *	@param filename
	 */
	void MaterialManager::AddCubeMap(std::string filename)
	{
		LoadCubeMap(filename);
	}

	//! Loads a texture
	/*!
	 *
	 *	@param filename
	 *	@return
	 */
	void MaterialManager::LoadTexture(std::string filename)
	{
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &(m_textures[m_textureCounter]->m_handle));
		glBindTexture(GL_TEXTURE_2D, m_textures[m_textureCounter]->m_handle);

		glfwLoadTexture2D(&filename[0], GLFW_BUILD_MIPMAPS_BIT);
		//! Set texture's clamping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//! Set texture's filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	//! Loads a cube map
	/*!
	 *	Loads a cube map texture consisting of 6 textures, one for every spatial direction.
	 *	+X, -X, +Y, -Y, +Z, -Z
	 *
	 *	@return
	 */
	void MaterialManager::LoadCubeMap(std::string filename)
	{
		ILboolean loadSuccess;
		GLuint cubemap_id;
		m_cubemaps.push_back(cubemap_id);

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &m_cubemaps[m_cubemapCounter]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemaps[m_cubemapCounter]);

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
			std::string m_filename = filename + "_" + suffixes[i] + ".tga";
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

		m_cubemapCounter++;
	}

	//! Returns a material by it's name
	/*!
	 *
	 *	@param name
	 *	@return
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

	//! Returns a material by it's index
	/*!
	 *
	 *	@param index
	 *	@return
	 */
	Material* MaterialManager::GetMaterial(unsigned int index)
	{
		return m_materials[index];
	}


	//! Returns the count of materials
	unsigned int MaterialManager::MaterialCount(void)
	{
		return m_materials.size();
	}


	//! Returns a texture handle by it's ID
	GLuint* MaterialManager::GetTextureByID(int i)
	{
		return &(m_textures[i]->m_handle);
	}

	//! Returns a cube map texture handle by it's ID
	/*!
	 *
	 *	@param i
	 *	@return
	 */
	GLuint* MaterialManager::GetCubeMapByID(int i)
	{
		return &m_cubemaps[i];
	}

} //! namespace scene
