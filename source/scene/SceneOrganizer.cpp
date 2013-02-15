/*
 * SceneOrganizer.cpp
 *
 *  Created on: 09.01.2013
 *      Author: guidoschmidt
 */

#include "SceneOrganizer.h"

namespace scene {

	//! Constructor
	SceneOrganizer::SceneOrganizer()
	{
		m_scenegraph_ptr = Singleton<SceneGraph>::Instance();
		m_materialman_ptr = Singleton<MaterialManager>::Instance();
	}


	//! Destructor
	SceneOrganizer::~SceneOrganizer()
	{
	}


	//! Organizes the render queue
	/*!
	 * Sorts the render queue by materials
	 */
	std::vector<Node*>* SceneOrganizer::OrganizeByMaterial(void)
	{
		std::cout << "Organizing Scene" << std::endl;
		for(unsigned int m = 0; m < m_materialman_ptr->MaterialCount(); m++)
		{
			for(unsigned int i = 0; i < m_scenegraph_ptr->NodeCount(); i++)
			{
				//! Meshes
				if(m_scenegraph_ptr->GetNode(i)->GetType() == "Mesh")
				{
					int node_mat_id = dynamic_cast<Mesh*>(m_scenegraph_ptr->GetNode(i))->GetMaterial()->GetMaterialID();
					int sort_mat_id = m_materialman_ptr->GetMaterial(m)->GetMaterialID();
					if(node_mat_id == sort_mat_id)
						m_renderQ.push_back(m_scenegraph_ptr->GetNode(i));
				}
				//! Lights
				if(m_scenegraph_ptr->GetNode(i)->GetType() == "Light")
				{
					int node_mat_id = dynamic_cast<Light*>(m_scenegraph_ptr->GetNode(i))->GetMaterial()->GetMaterialID();
					int sort_mat_id = m_materialman_ptr->GetMaterial(m)->GetMaterialID();
					if(node_mat_id == sort_mat_id)
						m_renderQ.push_back(m_scenegraph_ptr->GetNode(i));
				}
			}
		}

		//! Put organized node-list onto console
		std::cout << "Organized RenderQ (by material): ";
		for(unsigned int x=0; x < m_renderQ.size(); x++)
		{
			if(m_renderQ[x]->GetType() == "Mesh")
				std::cout << "(" << x << ")" << static_cast<Mesh*>(m_renderQ[x])->GetMaterial()->GetName() << ", ";
			if(m_renderQ[x]->GetType() == "Light")
				std::cout << "(" << x << ")" << "Light" << ", ";
		}

		return &m_renderQ;
	}


} //! namespace scene
