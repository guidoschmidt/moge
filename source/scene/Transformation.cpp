//! Transformation.cpp
/*!
 * @date	02.01.2013
 * @author	Guido Schmidt
 */

#include "Transformation.h"

namespace scene {

	Transformation::Transformation()
	{
		// TODO
	}

	Transformation::~Transformation()
	{
		// TODO Auto-generated destructor stub
	}

	//!
	void Transformation::Translate(glm::vec3 t){ m_position = t; }
	void Transformation::TranslateX(float dx){ m_position.x += dx; }
	void Transformation::TranslateY(float dy){ m_position.y += dy; }
	void Transformation::TranslateZ(float dz){ m_position.z += dz; }
	//!
	void Transformation::RotateEuler(float angle, glm::vec3 vector){ m_rotationMatrix = glm::rotate(angle, vector.x, vector.y, vector.z); }
	void Transformation::RotateQuat(glm::quat rotation){ m_rotation = rotation; }
	//!
	void Transformation::Scale(glm::vec3 s){ m_scale = s; }

	//!
	/*!
	 *
	 */
	void Transformation::AccumulateModelMatrix(void)
	{
		m_translationMatrix = glm::translate(m_position);
		m_rotationMatrix = glm::mat4_cast(m_rotation);
		m_scaleMatrix = glm::scale(m_scale);

		m_modelMatrix = glm::mat4(1.0f) * m_translationMatrix * m_scaleMatrix * m_rotationMatrix;
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::mat4 Transformation::GetModelMatrix(void)
	{
		AccumulateModelMatrix();
		return m_modelMatrix;
	}

	//!
	/*!
	 *
	 * @return
	 */
	glm::vec3 Transformation::GetPosition(void)
	{
		return m_position;
	}

} //! namespace scene
