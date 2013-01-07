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
			std::vector<Material*> materials;
			std::vector<GLuint*> textures;
			int textureCounter;

			//!
			ILenum ErrorCheckTexture;
			ILuint Image_id;

		public:
			MaterialManager();
			virtual ~MaterialManager();

			void AddMaterial(Material* material, std::string texturefile);

			GLuint* LoadTexture(std::string filename);
	};


} //! namespace scene
#endif //! MATERIALMANAGER_H_
