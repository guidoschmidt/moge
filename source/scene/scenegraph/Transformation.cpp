//! Transformation.cpp
/*!
 * @date	02.01.2013
 * @author	Guido Schmidt
 */

#include "Transformation.h"

namespace scene {

	Transformation::Transformation()
	{

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

	//! Accumulates matrixes
	/*!
	 * Accumulates all transformation matrixes for returning the
	 * model matrix
	 */
	void Transformation::AccumulateMatrices(void)
	{
		m_translationMatrix = glm::translate(m_position);
		m_rotationMatrix = glm::mat4_cast(m_rotation);
		m_scaleMatrix = glm::scale(m_scale);

		m_modelMatrix = glm::mat4(1.0f) * m_translationMatrix * m_scaleMatrix * m_rotationMatrix;
	}

	//! Accumulates and returns the model matrix
	/*!
	 *
	 * @return
	 */
	glm::mat4 Transformation::GetModelMatrix(void)
	{
		AccumulateMatrices();
		return m_modelMatrix;
	}



	void Transformation::SetPosition(glm::vec3 position)
	{
		m_position = position;
	}

	//! Returns the position
	/*!
	 *
	 * @return
	 */
	glm::vec3 Transformation::GetPosition(void)
	{
		return m_position;
	}

	//! Returns the rotation
	/*!
	 *
	 * @return
	 */
	glm::quat Transformation::GetRotation(void)
	{
		return m_rotation;
	}

	//! Returns the scale
	/*!
	 *
	 * @return
	 */
	glm::vec3 Transformation::GetScale(void)
	{
		return m_scale;
	}

} //! namespace scene
