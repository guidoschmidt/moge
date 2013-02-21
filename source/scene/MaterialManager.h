//! MaterialManagher.h
/*!
 * Manages all used materials and loads the
 * corresponding textures.
 *
 * @date	05.01.2013
 * @author	Guido Schmidt
 */

#ifndef MATERIALMANAGER_H_
#define MATERIALMANAGER_H_

//! C++ includes
#include <vector>
#include <string>
//! OpenGL includes
#include <GL/glew.h>
#include <GL/glfw.h>
//! DevIL includes
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
//! Local includes
#include "../utilities/Singleton.h"
#include "scenegraph/Material.h"

//! TODO Helper class for tupel 'texture' (string filename, GLuint handler)

namespace scene {

	struct texture{
		std::string m_filename;
		texturetype m_type;
		GLuint m_handle;
	};

	class MaterialManager {

		private:
			//! Materials
			std::vector<Material*> m_materials;
			unsigned int m_materialCounter;

			//! Textures
			ILenum ErrorCheckTexture;
			ILuint Image_id;
			std::vector<texture*> m_textures;
			unsigned int m_textureCounter;

			//! Cubemaps
			std::vector<GLuint> m_cubemaps;
			unsigned int m_cubemapCounter;

		protected:
			void LoadTexture(std::string filename);
			void LoadCubeMap(std::string filename);

		public:
			MaterialManager();
			virtual ~MaterialManager();

			//! Material
			void AddMaterial(std::string name);
			void AddMaterial(std::string name, glm::vec3 diffuse, float reflectivity, std::vector<texture> textures);
			void AddCubeMap(std::string filename);

			//! Getter
			Material* GetMaterial(std::string searchname);
			Material* GetMaterial(unsigned int index);
			unsigned int MaterialCount(void);
			GLuint* GetTextureByID(int i);
			GLuint* GetCubeMapByID(int i);

	};


} //! namespace scene
#endif //! MATERIALMANAGER_H_
