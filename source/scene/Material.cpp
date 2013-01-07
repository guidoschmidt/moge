//! Material.cpp
/*!
 * @date	03.01.2013
 * @author	Guido Schmidt
 */

#include "Material.h"

namespace scene {

	//!
	/*!
	 *
	 */
	Material::Material() {
		texture_ptr = 0;
		reflectivity = 0;
		glm::vec4 diffuseColor(0.8f);
		glm::vec4 specularColor(0.8f);
	}


	//!
	/*!
	 *
	 */
	Material::~Material() {
		// TODO Auto-generated destructor stub
	}


	//! Load texture from file
	/*!
	 *
	 * @param filename
	 */
	void Material::LoadTexture(const std::string filename)
	{
//		ILboolean loadSuccess = ilLoadImage(filename.c_str());
//
//		if(loadSuccess){
//			glGenTextures(1, &Texture_id);
//			glBindTexture(GL_TEXTURE_2D, Texture_id);
//			//! Set texture's clamping
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//			//! Set texture's filtering
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//			//! Load image into texture
//			glTexImage2D(	GL_TEXTURE_2D,
//							0,
//							ilGetInteger(IL_IMAGE_BPP),
//							ilGetInteger(IL_IMAGE_WIDTH),
//							ilGetInteger(IL_IMAGE_HEIGHT),
//							0,
//							ilGetInteger(IL_IMAGE_FORMAT),
//							GL_UNSIGNED_BYTE,
//							ilGetData());
//			//! Delete image
//			ilDeleteImages(1, &Image_id);
//			std::cout << "DeVIL: texture was generated from " << filename << "!" << std::endl;
//		}
//		else if(!loadSuccess){
//			ErrorCheckTexture = ilGetError();
//			std::cout << "ERROR | DeVIL: Image load error " << iluErrorString(ErrorCheckTexture) << std::endl;
//		}
	}


	//! Returns the texture handle
	/*!
	 *
	 * @return
	 */
	GLuint Material::GetTexture(void)
	{
		//! TODO
		return 0;
	}


	//!
	/*!
	 *
	 */
	inline glm::vec4 Material::GetDiffuseColor(void){ return diffuseColor; }
	inline glm::vec4 Material::GetSpecularColor(void){ return specularColor; }
	inline float Material::GetReflectivity(void){ return reflectivity; }

	//!
	/*!
	 *
	 */
	inline void Material::SetDiffuseColor(glm::vec4 diffuse){ diffuseColor = diffuse; }
	inline void Material::SetSpecularColor(glm::vec4 specular){ specularColor = specular; }
	inline void Material::SetReflectivity(float reflection){ reflectivity= reflection; }

} //! namespace scene
