/*
 * SceneOrganizer.cpp
 *
 *  Created on: 09.01.2013
 *      Author: guidoschmidt
 */

#include "SceneOrganizer.h"

namespace scene {

	//!
	/*!
	 *
	 */
	SceneOrganizer::SceneOrganizer()
	{
		scenegraph = Singleton<SceneGraph>::Instance();
		materialman = Singleton<MaterialManager>::Instance();
	}


	//!
	/*!
	 *
	 */
	SceneOrganizer::~SceneOrganizer()
	{
	}


	//! Organizes the render queue
	/*!
	 *
	 */
	std::vector<Node*>* SceneOrganizer::OrganizeScene(void)
	{
		std::cout << "Organizing Scene" << std::endl;
		for(unsigned int m = 0; m < materialman->MaterialCount(); m++)
		{
			for(unsigned int i = 0; i < scenegraph->NodeCount(); i++)
			{
				if(dynamic_cast<Mesh*>(scenegraph->GetNode(i)) != 0)
				{
					int node_mat_id = dynamic_cast<Mesh*>(scenegraph->GetNode(i))->GetMaterial()->GetID();
					int sort_mat_id = materialman->GetMaterial(m)->GetID();
					if(node_mat_id == sort_mat_id)
						oneMaterial.push_back(scenegraph->GetNode(i));
				}
			}
		}

		//! Put organized node-list onto console
		std::cout << "Organized vector: " << std::endl;
		for(unsigned int x=0; x < oneMaterial.size(); x++)
		{
			std::cout << static_cast<Mesh*>(oneMaterial[x])->GetMaterial()->GetName() << std::endl;
		}

		return &oneMaterial;
	}


} //! namespace scene
