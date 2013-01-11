//! Renderer.h
/*!
 * @date 	28.11.2012
 * @author  Guido Schmidt
 */

#ifndef RENDERER_H_
#define RENDERER_H_

//! C++ includes
#include <iostream>
#include <vector>
#include <sstream>
//! OpenGL include
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
//! AntTweakBar include
#include <AntTweakBar.h>
//! glm includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
//! Local includes
#include "Context.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "FSQ.h"
#include "FrameBufferObject.h"
#include "../utilities/Singleton.h"
#include "../scene/Node.h"
#include "../scene/SceneGraph.h"
#include "../scene/MaterialManager.h"
#include "../scene/SceneOrganizer.h"

class Renderer
{
	private:
		//! Singleton instances
		scene::SceneGraph* scenegraph_ptr;
		std::vector<scene::Node*>* renderQ_ptr;

		//! OpenGL Info
		const GLubyte* glinfo_renderer_ptr;
		const GLubyte* glinfo_vendor_ptr;
		const GLubyte* glindo_openglVersion_ptr;
		const GLubyte* glinfo_glslVersion_ptr;
		GLint glinfo_major, glinfo_minor;

		typedef enum{FILE=0, CONSOLE=1} log;
		GLboolean m_running;
		Context* context_ptr;
		FSQ* fsq_ptr;
		ShaderProgram* forwardProgram_ptr, *deferredProgram_Pass1_ptr, *deferredProgram_Pass2_ptr;
		FrameBufferObject* firstPassFBO_ptr;
		float m_angle;
		float m_rotSpeed;

		//! AnttWeakBar texture choose
		typedef enum {TEX_COMPOSIT=-1, TEX_POSITION=0, TEX_COLOR=1, TEX_NORMAL=2, TEX_DEPTH=3} DeferredTexture;
		#define NUM_TEXS 5
		DeferredTexture tw_currentDeferredTex;

		typedef enum {HEAD=0, GEOMETRY=1, CONFERENCE=2, BUDDHA=3, TEAPOT=4} Scenes;
		#define NUM_SCENES 2
		Scenes tw_currentScene;

		//! Matrices
		glm::mat4 IdentityMatrix;
		glm::mat4 ModelMatrix;
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
		glm::mat4 MVPMatrix;
		//! Lights
		glm::vec4 LightPosition;
		glm::vec3 LightAmbient;
		glm::vec3 LightDiffuse;
		glm::vec3 LightSpecular;
		//! Camera
		glm::vec3 CameraPosition;
		glm::vec3 CameraTargetPosition;
		glm::vec3 CameraUp;

		//! Material
		float m_shininess;
		//! Background color
		glm::vec3 m_backgroundColor;

	protected:
		void InitGLEW(void);

	public:

		Renderer(int width=500, int height=500);
		~Renderer(void);
		void CalculateFPS(double timeInterval, bool toWindowTitle);
		void Initialize(int width, int height);
		void InitializeMatrices(void);
		void InitializeLight(void);
		void InitializeILUT(void);
		void KeyboardFunction(void);
		void CameraMovement(void);
		void RenderLoop(void);
		void WriteLog(log logLocation);
};

#endif //! RENDERER_H_
