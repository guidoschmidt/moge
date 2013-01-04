//! Material.cpp
/*!
 * @date	03.01.2013
 * @author	Guido Schmidt
 */

#include "Material.h"

namespace scene {

	//!
	Material::Material() {
		Texture_id = 0;
		ErrorCheckTexture = 0;
		Image_id = 0;
	}

	//!
	Material::~Material() {
		// TODO Auto-generated destructor stub
	}

	//! Load texture from file
	void Material::LoadTexture(const std::string filename)
	{
		ILboolean loadSuccess = ilLoadImage(filename.c_str());

		if(loadSuccess){
			glGenTextures(1, &Texture_id);
			glBindTexture(GL_TEXTURE_2D, Texture_id);
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
	}

	//! Returns the texture handle
	GLuint Material::GetTexture(void)
	{
		return Texture_id;
	}

	void Material::Use(void)
	{

	}

} //! namespace scene
