//! Mesh.h
/*!
 * @date 	02.01.2013
 * @author	Guido Schmidt
 */

#ifndef MESH_H_
#define MESH_H_

//! C++ includes
#include <vector>
#include <iostream>
//! OpenGL includes
#include <GL/glew.h>
//! Assimp includes
#include <assimp/mesh.h>
//! DevIL includes
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
//! Local includes
#include "../../utilities/Singleton.h"
#include "../MaterialManager.h"
#include "Node.h"
#include "Material.h"

namespace scene {

	class Mesh : public Node {
		struct BoundingBox{
			glm::vec3 max;
			glm::vec3 min;
			std::vector<GLfloat> vertices;
			std::vector<GLint> indices;
		};

		private:
			//! Topology variables
			aiMesh* mesh_ptr;
			std::vector<GLfloat> vertices;
			std::vector<GLint> indices;
			std::vector<GLfloat> normals;
			std::vector<GLfloat> uvs;
			BoundingBox m_boundingBox;

			//! Buffer handlers
			GLenum ErrorCheckMesh;
			GLuint VAO_id, BBVAO_id;
			GLuint VBO_id, IBO_id, NBO_id, UVBO_id, BBO_id, BBIO_id;

			//! Material
			Material* material_ptr;

		protected:
			void Initialize(void);
			void CreateBuffers(void);
			void CreateBoundingBox(void);

		public:
			Mesh(aiMesh* m);
			virtual ~Mesh();


			//! Material
			// Setter
			void SetMaterial(Material* material);
			// Getter
			Material* GetMaterial(void);
			// Check material
			bool HasMaterial(void);

			//! Texture
			GLuint* GetTextureHandle(texturetype type);

			void Draw(void);
			void DrawBoundingBox(void);
	};

} //! namespace scene
#endif //! MESH_H_
