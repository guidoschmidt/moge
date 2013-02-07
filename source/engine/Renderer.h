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
#include "../utilities/Singleton.h"
#include "../scene/scenegraph/Node.h"
#include "../scene/scenegraph/Material.h"
#include "../scene/scenegraph/SceneGraph.h"
#include "../scene/MaterialManager.h"
#include "../scene/SceneOrganizer.h"
#include "Context.h"
#include "ShaderProgram.h"
#include "FSQ.h"
#include "FrameBufferObject.h"

class Renderer
{
	private:
		//! Singleton instances
		scene::SceneGraph* scenegraph_ptr;
		scene::MaterialManager* materialman_ptr;
		std::vector<scene::Node*>* renderQ_ptr;

		//! OpenGL Info
		const GLubyte* glinfo_renderer_ptr;
		const GLubyte* glinfo_vendor_ptr;
		const GLubyte* glindo_openglVersion_ptr;
		const GLubyte* glinfo_glslVersion_ptr;
		GLint glinfo_major, glinfo_minor;

		typedef enum{FILE = 0, CONSOLE = 1} log;
		GLboolean m_running;
		Context* context_ptr;
		FSQ* fsq_ptr;
		ShaderProgram 	*forwardProgram_ptr,
						*m_gBufferProgram_ptr,
						*deferredProgram_Pass2_ptr,
						*deferredProgram_Pass3_ptr,
						*deferredProgram_Pass4_ptr;
		FrameBufferObject* gBuffer_ptr, *fbo_ptr;
		float m_angle;
		float m_fieldOfView;

		double m_speed;

		float time1, time2, timedif;

		//! AnttWeakBar texture choose
		typedef enum {TEX_COMPOSIT=-1, TEX_POSITION=0, TEX_COLOR=1, TEX_NORMAL=2, TEX_MATID=3, TEX_REFL=4, TEX_REFLVEC=5, TEX_REALDEPTH=6, TEX_DEPTH=7} DeferredTexture;
		#define NUM_TEXS 9
		DeferredTexture tw_currentDeferredTex;

		typedef enum {HEAD=0, GEOMETRY=1, CONFERENCE=2, BUDDHA=3, TEAPOT=4} Scenes;
		#define NUM_SCENES 2
		Scenes tw_currentScene;

		float tw_rotSpeed;
		bool tw_mouseLight;
		bool tw_useNormalMapping;
		bool tw_drawLights;
		bool tw_mouseSlider;
		float tw_deltaDepth;
		float tw_rayMultiplier;
		bool tw_SSR;
		bool tw_blur;
		bool tw_compareDepth;
		bool tw_reflvec;
		bool tw_jittering;
		bool tw_drawSkyBox;
		bool tw_boundingbox;
		//! Passes
		bool tw_pass1, tw_pass2, tw_pass3, tw_pass4;

		int x_pos, y_pos;
		int correct_x_pos, correct_y_pos;

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
