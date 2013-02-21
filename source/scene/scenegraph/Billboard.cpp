//! Billboard.cpp
/*!
 * @date	11.02.2013
 * @author	Guido Schmidt
 */

#include "Billboard.h"

namespace scene {

	//! Constructor
	Billboard::Billboard() {
		m_type = "Billboard";
		m_translationMatrix = glm::mat4(1.0f);
		m_rotationMatrix = glm::mat4(0.0f);
		m_scaleMatrix = glm::mat4(1.0f);
		m_modelMatrix = glm::mat4(1.0f);

		m_mesh_ptr = 0;

	}

	Billboard::Billboard(aiMesh* m)
	{
		m_mesh_ptr = m;
	}

	//! Destructor
	Billboard::~Billboard() {
		// TODO Auto-generated destructor stub
	}

	//!
	void Billboard::CalculateNormal(void)
	{

	}

	glm::vec3 Billboard::GetNormal(void)
	{
		return m_normal;
	}

	//!
	/*!
	 *
	 */
	void Billboard::Draw(void)
	{
		AccumulateMatrices();
		glBindVertexArray(VAO_id);
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}




} //! namespace scene
