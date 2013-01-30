//! Renderpass.cpp
/*!
 *
 * @date	30.01.2013
 * @author	Guido Schmidt
 */

#include "Renderpass.h"

//! Constructor
Renderpass::Renderpass(std::string name)
{
	m_name = name;
	m_shaderProgram_ptr = new ShaderProgram();
	m_frameBufferObject_ptr = new FrameBufferObject();
}

//! Destructor
Renderpass::~Renderpass()
{
	// TODO Auto-generated destructor stub
}

void Renderpass::Run(void)
{
	/*
	deferredProgram_Pass1_ptr->SetUniformCubemap("cubeMapTex", *(materialman_ptr->GetCubeMap(0)), 0);
	deferredProgram_Pass1_ptr->SetUniform("CameraPosition", scenegraph_ptr->GetActiveCamera()->GetPosition());

	for(unsigned int n=0; n < renderQ_ptr->size(); n++)
	{
		if(static_cast<scene::Mesh*>((*renderQ_ptr)[n]))
		{
			ModelMatrix = static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetModelMatrix();
			ModelMatrix = glm::mat4(1.0f) * RotationMatrix * ModelMatrix;
			ViewMatrix = scenegraph_ptr->GetActiveCamera()->GetViewMatrix();
			ProjectionMatrix = scenegraph_ptr->GetActiveCamera()->GetProjectionMatrix();
			//! Matrix uniforms
			deferredProgram_Pass1_ptr->SetUniform("NormalMatrix", glm::transpose(glm::inverse(ViewMatrix * ModelMatrix)));
			deferredProgram_Pass1_ptr->SetUniform("ModelMatrix", ModelMatrix);
			deferredProgram_Pass1_ptr->SetUniform("ViewMatrix", ViewMatrix);
			deferredProgram_Pass1_ptr->SetUniform("ModelViewMatrix", ViewMatrix * ModelMatrix);
			deferredProgram_Pass1_ptr->SetUniform("ProjectionMatrix", ProjectionMatrix);
			deferredProgram_Pass1_ptr->SetUniform("MVPMatrix", ProjectionMatrix * ViewMatrix * ModelMatrix);
			if(static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->HasTexture())
			{
				int current_mat_id = dynamic_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->GetID();
				deferredProgram_Pass1_ptr->SetUniformSampler("colorTex", *(Singleton<scene::MaterialManager>::Instance()->GetTexture(current_mat_id)), 5);
				deferredProgram_Pass1_ptr->SetUniform("Material.id", current_mat_id);
				deferredProgram_Pass1_ptr->SetUniform("Material.reflectance", dynamic_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->GetReflectivity());
			}
			static_cast<scene::Mesh*>((*renderQ_ptr)[n])->Draw();

			//! Draws the bounding boxes
			if(tw_boundingbox)
			{
				static_cast<scene::Mesh*>((*renderQ_ptr)[n])->DrawBoundingBox();
			}
		}
	}

	gBuffer_ptr->Unuse();
	deferredProgram_Pass1_ptr->Unuse();
	*/
}
