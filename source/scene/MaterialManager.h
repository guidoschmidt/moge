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
#include <GL/gl.h>
//! DevIL includes
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
//! Local includes
#include "../utilities/Singleton.h"
#include "Material.h"

//! TODO Helper class for tupel 'texture' (string filename, GLuint handler)

namespace scene {

	class MaterialManager {

		private:
			//! Material
			std::vector<Material*> materials;
			std::vector<GLuint> textures;
			unsigned int textureCounter;
			unsigned int materialCounter;

			//! Texture
			ILenum ErrorCheckTexture;
			ILuint Image_id;

		protected:
			GLuint LoadTexture(std::string filename);

		public:
			MaterialManager();
			virtual ~MaterialManager();

			//! Material
			void AddMaterial(std::string name, std::string texturefile, float reflectivity);
			void AddMaterial(std::string name);
			Material* GetMaterial(std::string searchname);
			Material* GetMaterial(unsigned int index);
			unsigned int MaterialCount(void);

			//! Texture
			GLuint* GetTexture(int i);
	};


} //! namespace scene
#endif //! MATERIALMANAGER_H_
