//! Material.h
/*!
 * @date	03.01.2013
 * @author	Guido Schmidt
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

//! C++ includes
#include <string>
#include <iostream>
//! OpenGL includes
#include <GL/glew.h>
#include <GL/gl.h>
//! DevIL includes
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

namespace scene {

	class Material {
		private:
			GLuint Texture_id;
			ILenum ErrorCheckTexture;
			ILuint Image_id;

		public:
			Material();
			virtual ~Material();

			void LoadTexture(const std::string filename);

			void Use(void);
	};

} //! namespace scene
#endif //! MATERIAL_H_
