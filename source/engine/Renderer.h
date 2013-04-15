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
						*m_lightingProgram_ptr,
						*m_ssrProgram_ptr,
						*m_blurProgram_ptr,
						*m_compositingProgram_ptr;
		FrameBufferObject* gBuffer_ptr, *lighting_fbo_ptr, *reflection_fbo_ptr, *pass4_fbo_ptr;
		float m_angle;
		float m_fieldOfView;

		double m_speed;
		std::string m_sceneName;

		float time1, time2, timedif;

		//! AnttWeakBar texture choose
		typedef enum {	TEX_COMPOSIT		=	-1,
						TEX_WORLDPOSITION	=	 0,
						TEX_VIEWPOSITION	=	 1,
						TEX_COLOR			=	 2,
						TEX_MATERIALID		=	 3,
						TEX_WSNORMAL		=	 4,
						TEX_VSNORMAL		=	 5,
						TEX_DEPTH			=	 6,
						TEX_REFLECTANCE		=	 7,
						TEX_ENVMAP			=	 8,
						TEX_SSR				=	 9,
						TEX_BB				=	 10
		} DeferredTexture;
		#define NUM_TEXS 12
		DeferredTexture tw_currentDeferredTex;

		typedef enum {TESTSCENE=0, MUSEUM=1, CHURCH=2, STREET=3} Scenes;
		#define NUM_SCENES 1
		Scenes tw_currentScene;

		float tw_rotSpeed;
		bool tw_mouseLight;
		bool tw_useNormalMapping;
		bool tw_drawLights;
		glm::vec3 tw_camerapos;
		glm::vec3 tw_cameralookat;
		bool tw_mouseSlider;
		int tw_rayStepSize;
		bool tw_SSR;
		bool tw_optimizedSSR;
		bool tw_CM;
		bool tw_BB;
		bool tw_PCCM;
		bool tw_blur;
		float tw_blurX;
		float tw_blurY;
		bool tw_compareDepth;
		bool tw_reflvec;
		bool tw_jittering;
		bool tw_fadeToEdges;
		float tw_fadeY;
		bool tw_drawSkyBox;
		bool tw_boundingbox;
		bool tw_blockCamera;
		bool tw_cameraMouseButton;
		bool tw_cameraOrtho;
		int m_billboardTexCounter;
		//! Passes
		bool tw_pass1, tw_pass2, tw_pass3, tw_pass4, tw_pass5;

		int x_pos, y_pos;
		int correct_x_pos, correct_y_pos;

		//! Matrices
		glm::mat4 IdentityMatrix;
		glm::mat4 ModelMatrix;
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
		glm::mat4 MVPMatrix;
		//! Lights
		int tw_currentLight;
		glm::vec3 LightAmbient;
		glm::vec3 LightPosition;
		GLfloat LightPositions[3][3];
		glm::vec3 LightDiffuse;
		GLfloat LightDiffuses[3][3];
		glm::vec3 LightSpecular;
		//! Camera
		glm::vec3 CameraPosition;
		glm::vec3 CameraTargetPosition;
		glm::vec3 CameraUp;
		//! Billboards
		glm::mat4 BillboardMatrices[20];
		GLuint BillboardTextures[20];

		//! Material
		float m_shininess;
		//! Background color
		glm::vec3 m_backgroundColor;

	protected:
		void InitGLEW(void);

	public:

		Renderer(int width=500, int height=500, int scene=1);
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
