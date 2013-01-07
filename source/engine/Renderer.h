//! Renderer.h
/*!
 * @date 	28.11.2012
 * @author  Guido Schmidt
 */

#ifndef RENDERER_H_
#define RENDERER_H_

//! C++ includes
#include <iostream>
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
#include "../scene/SceneGraph.h"
#include "../scene/MaterialManager.h"

class Renderer
{
	private:
		//! Singleton instances
		scene::SceneGraph* scenegraph;

		const GLubyte* renderer;
		const GLubyte* vendor;
		const GLubyte* openglVersion;
		const GLubyte* glslVersion;
		GLint major, minor;

		typedef enum{FILE=0, CONSOLE=1} log;
		GLboolean RUNNING;
		Context* context;
		FSQ* fsq;
		ShaderProgram* forwardProgram, *deferredProgram_Pass1, *deferredProgram_Pass2;
		FrameBufferObject* firstPassFBO;
		float angle;
		float rotSpeed;

		//! AnttWeakBar texture choose
		typedef enum {TEX_COMPOSIT=-1, TEX_POSITION=0, TEX_COLOR=1, TEX_NORMAL=2, TEX_DEPTH=3} DeferredTexture;
		#define NUM_TEXS 5
		DeferredTexture currentDeferredTex;

		typedef enum {HEAD=0, GEOMETRY=1, CONFERENCE=2} Scenes;
		#define NUM_SCENES 2
		Scenes currentScene;

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
		float Shininess;
		//! Background color
		glm::vec3 BackgroundColor;

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
		void RenderLoop(void);
		void WriteLog(log logLocation);
};

#endif //! RENDERER_H_
