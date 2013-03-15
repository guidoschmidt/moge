//! Renderer.cpp
/*!
 * @date 	28.11.2012
 * @author 	Guido Schmidt
 */

#include "Renderer.h"

//! ANTTWEAKBAR CALLBACKS BEGIN
/*! Variables and button callbacks */
bool tw_rotation;
bool tw_ortho = false;
bool tw_orthoSwitched = false;
bool loaded;
float theta = 0, phi = 0;
float r = 0;
static void TW_CALL SwitchRotation(void* clientData){ tw_rotation = !tw_rotation;}
static void TW_CALL SwitchOrtho(void* clientData){ tw_ortho = !tw_ortho;}
//! ANTTWEAKBAR CALLBACKS END


//! Constructor
/*!
 *
 * @param width
 * @param height
 */
Renderer::Renderer(int width, int height)
{
	tw_rotSpeed = 0.2f;
	m_fieldOfView = 45.0f;

	time1 = 0.0f;
	time2 = 0.0f;

	loaded = false;

	tw_currentScene = MUSEUM;
	tw_currentDeferredTex = TEX_COMPOSIT;
	tw_rotation = false;
	tw_mouseLight = false;
	tw_SSR = false;
	tw_compareDepth = false;
	tw_reflvec = false;
	tw_jittering = false;
	tw_boundingbox = false;
	tw_rayStepSize = 0.0f;
	tw_blockCamera = true;
	tw_cameraMouseButton = true;

	//! Passes
	tw_pass1 = true;
	tw_pass2 = true;
	tw_pass3 = true;
	tw_pass4 = false;
	tw_pass5 = true;

	context_ptr = Singleton<Context>::Instance();
	fsq_ptr = new FSQ();
	m_shininess = 12.0f;

	m_backgroundColor = glm::vec3(0.0f, 0.0f, 0.05f);

	Initialize(width, height);
}

//! Destructor
/*!
 *
 */
Renderer::~Renderer(void)
{

}

//! Initialize GLEW
/*! Initializes GLEW.*/
void Renderer::InitGLEW(void){
	GLenum GLEWinit = glewInit();
	if(GLEWinit != GLEW_OK){
		std::cout << "ERROR: could not initialize GLEW: " << glewGetErrorString(GLEWinit) << std::endl;
	}
}

//! Initialize
/*!
 * Initializes the renderer. Includes openning a context, adding of AntTweakBar variables,
 * Loading the scene, creating shader programs, and calling the other initialize methods.
 */
void Renderer::Initialize(int width, int height)
{
	context_ptr->OpenWindow(width, height, "moge", 4, 2);
	context_ptr->AddAntTweakBar();

	WriteLog(CONSOLE);
	InitGLEW();
	//! Initialize image loader utility
	InitializeILUT();

	glEnable(GL_DEPTH_TEST);

	/*!
	 * AntTweakBar
	 */
	//! Deferred: render targets choice
	TwEnumVal texEV[NUM_TEXS] = {	{TEX_COMPOSIT, 		"Composited"},
									{TEX_WORLDPOSITION, "World space positions"},
									{TEX_VIEWPOSITION, 	"View space positions"},
									{TEX_COLOR, 		"Colormap"},
									{TEX_MATERIALID, 	"Material ID"},
									{TEX_WSNORMAL, 		"World space normals"},
									{TEX_VSNORMAL, 		"View space normals"},
									{TEX_ENVMAP, 		"Environment Mapping"},
									{TEX_REFLECTANCE, 	"Reflectance"},
									{TEX_REFLVEC, 		"Reflection Vector"},
									{TEX_DEPTH, 		"Depthmap"},
									{TEX_LINDEPTH, 		"Linear Depthmap"},
									{TEX_SSR, 			"SSR"},
									{TEX_BB, 			"BB"}};
	TwType texType = TwDefineEnum("TextureType", texEV, NUM_TEXS);
	TwAddVarRW(context_ptr->GetBar(), "deferredTextureChoice", texType, &tw_currentDeferredTex, "label='Rendering' group='Rendering' keyIncr='<' keyDecr='>' help='View the maps rendered in first pass.' ");
	//! Scene choice
	//TwEnumVal sceneEV[NUM_SCENES] = { {HEAD, "Head"}, {GEOMETRY, "Geometry"}};
	//TwType sceneType = TwDefineEnum("SceneType", sceneEV, NUM_SCENES);
	//TwAddVarRW(context_ptr->GetBar(), "sceneChoice", sceneType, &tw_currentScene, "label='Scene' group='Scene' keyIncr='<' keyDecr='>' help='Load another scene.' ");
	//! Material
	TwAddVarRW(context_ptr->GetBar(), "shininess", TW_TYPE_FLOAT, &m_shininess, "step='0.01' max='100.0' min='0.0' label='Shininess' group='Material'");
	//! Background color
	TwAddVarRW(context_ptr->GetBar(), "background", TW_TYPE_COLOR3F, &m_backgroundColor, "label='Background' group='Scene' colormode='hls'");
	//! Rotation
	TwAddButton(context_ptr->GetBar(), "togglerotation", SwitchRotation, NULL, "key='r' label='Toggle Rotation' group='Rotation'");
	TwAddVarRW(context_ptr->GetBar(), "rotationSpeed", TW_TYPE_FLOAT, &tw_rotSpeed, "step='0.001' max='1.0' min='0.0' label='Rotationspeed' group='Rotation'");
	//! Camera
	TwAddVarRW(context_ptr->GetBar(), "fieldofview", TW_TYPE_FLOAT, &m_fieldOfView, "step='0.1' max='170.0' min='2.0' label='Field Of View' group='Camera'");
	TwAddVarRW(context_ptr->GetBar(), "blockmovement", TW_TYPE_BOOLCPP, &tw_blockCamera, "label='Block Movement' group='Camera'");
	TwAddVarRW(context_ptr->GetBar(), "mousebuttonmovement", TW_TYPE_BOOLCPP, &tw_cameraMouseButton, "label='Move Right Mouse' group='Camera'");
	TwAddButton(context_ptr->GetBar(), "toggleOrtho", SwitchOrtho, NULL, "key='o' label='Toggle Orthographic Projection' group='Camera'");
	//! Mouse
	TwAddVarRO(context_ptr->GetBar(), "mousex", TW_TYPE_INT16, &correct_x_pos, "group='Mouse' label='X'");
	TwAddVarRO(context_ptr->GetBar(), "mousey", TW_TYPE_INT16, &correct_y_pos, "group='Mouse' label='Y'");
	TwAddVarRW(context_ptr->GetBar(), "mouselight", TW_TYPE_BOOLCPP, &tw_mouseLight, "key='m' group='Light' label='Mouse controlled'");
	//! Screen space reflections
	TwAddVarRW(context_ptr->GetBar(), "ssr", TW_TYPE_BOOLCPP, &tw_SSR, "group='SSR' label='Switch SSR'");
	TwAddVarRW(context_ptr->GetBar(), "stepsize", TW_TYPE_INT16, &tw_rayStepSize, "min='1' value='5' step='1' group='SSR' label='Step size (pixels)'");
	TwAddVarRW(context_ptr->GetBar(), "blur", TW_TYPE_BOOLCPP, &tw_blur, "group='SSR' label='Blur'");
	TwAddVarRW(context_ptr->GetBar(), "blurX", TW_TYPE_INT16, &tw_blurX, "group='SSR' min='0' max='8' label='Kernel X'");
	TwAddVarRW(context_ptr->GetBar(), "blurY", TW_TYPE_INT16, &tw_blurY, "group='SSR' min='0' max='8' label='Kernel Y'");
	TwAddVarRW(context_ptr->GetBar(), "jittering", TW_TYPE_BOOLCPP, &tw_jittering, "group='SSR' label='Jitter'");
	TwAddVarRW(context_ptr->GetBar(), "fadetoscreenedge", TW_TYPE_BOOLCPP, &tw_fadeToEdges, "group='SSR' label='Fade to screen edges'");
	//! Parallax corrected cube mapping
	TwAddVarRW(context_ptr->GetBar(), "cm", TW_TYPE_BOOLCPP, &tw_CM, "group='PCCM' label='Switch Environment Mapping'");
	TwAddVarRW(context_ptr->GetBar(), "pccm", TW_TYPE_BOOLCPP, &tw_PCCM, "group='PCCM' label='Switch PCCM'");
	//! Billboard reflections
	TwAddVarRW(context_ptr->GetBar(), "bb", TW_TYPE_BOOLCPP, &tw_BB, "group='BB' label='Switch Billboard Reflections'");
	//! Draw modes
	TwAddVarRW(context_ptr->GetBar(), "drawboundingbox", TW_TYPE_BOOLCPP, &tw_boundingbox, "group='Rendering' label='Show bounding boxes'");
	//! Normal mapping
	TwAddVarRW(context_ptr->GetBar(), "normalmapping", TW_TYPE_BOOLCPP, &tw_useNormalMapping, "group='Material' label='Normal mapping'");
	//! Light properties
	TwAddVarRW(context_ptr->GetBar(), "lightambient", TW_TYPE_COLOR3F, &LightAmbient, "label='Ambient' group='Light' colormode='hls'");
	TwAddVarRW(context_ptr->GetBar(), "lightdiffuse", TW_TYPE_COLOR3F, &LightDiffuse, "label='Diffuse' group='Light' colormode='hls'");
	TwAddVarRW(context_ptr->GetBar(), "lightdpecular", TW_TYPE_COLOR3F, &LightSpecular, "label='Specular' group='Light' colormode='hls'");
	TwAddVarRW(context_ptr->GetBar(), "drawlights", TW_TYPE_BOOLCPP, &tw_drawLights, "group='Light' label='Draw lights'");
	//! Shader Programs
	TwAddVarRW(context_ptr->GetBar(), "pass1", TW_TYPE_BOOLCPP, &tw_pass1, "group='Renderer' label='Pass 01'");
	TwAddVarRW(context_ptr->GetBar(), "pass2", TW_TYPE_BOOLCPP, &tw_pass2, "group='Renderer' label='Pass 02'");
	TwAddVarRW(context_ptr->GetBar(), "pass3", TW_TYPE_BOOLCPP, &tw_pass3, "group='Renderer' label='Pass 03'");
	TwAddVarRW(context_ptr->GetBar(), "pass4", TW_TYPE_BOOLCPP, &tw_pass4, "group='Renderer' label='Pass 04'");
	TwAddVarRW(context_ptr->GetBar(), "pass5", TW_TYPE_BOOLCPP, &tw_pass5, "group='Renderer' label='Pass 05'");

	//! Initialize singleton instances
	scenegraph_ptr = Singleton<scene::SceneGraph>::Instance();
	materialman_ptr = Singleton<scene::MaterialManager>::Instance();

	/*! Init deferred rendering
	 * *************************/
		/*! Initialization of 1st pass
		 * 	G-Buffer
		 */
		m_gBufferProgram_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/gbuffer/gbuffer.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/gbuffer/gbuffer.frag.glsl"
		);
		gBuffer_ptr = new FrameBufferObject(true);
		gBuffer_ptr->CreateGBuffer();

		/*! Initialization of 2nd pass
		 *  Deferred Lighting and Shading
		 */
		deferredProgram_Pass2_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/lighting/lighting.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/lighting/lighting.frag.glsl"
		);
		lighting_fbo_ptr = new FrameBufferObject(false);

		/*! Initialization of 3nd pass
		 * 	SSR pass (screen space reflections)
		 */
		deferredProgram_Pass3_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/ssr/ssr.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/ssr/ssr.frag.glsl"
		);
		reflection_fbo_ptr = new FrameBufferObject(false);

		/*! Initialization of 4th pass
		 * 	Billboard reflections
		 */
		/*
		deferredProgram_Pass4_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/4-deferred_bb.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/4-deferred_bb.frag.glsl"
		);
		bb_fbo_ptr = new FrameBufferObject(false);
		*/

		//! Initialization of 5th pass
		deferredProgram_Pass5_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/compositing/compositing.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/compositing/compositing.frag.glsl"
		);
		//! Fullscreen quad initialization

		//! Create a fullscreen quad to display deferred rendered textures
		fsq_ptr->CreateFSQ();

	InitializeMatrices();
	InitializeLight();

	m_running = true;
}


//! Initializes matrix setup
/*!
 * Creates a matrix setup for all non-dynamic matrices.
 */
void Renderer::InitializeMatrices(void)
{
	//! Identitymatrix
	IdentityMatrix = glm::mat4(1.0f);

	//! Viewmatrix
}


//! Initialize light setup
/*!
 * Creates a light setup, including light's position, ambient, diffuse & specular color.
 * Adds AntTweakBar variables, too.
 */
void Renderer::InitializeLight(void)
{
	LightPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	LightDiffuse = glm::vec3(0.95f, 0.85f, 0.75f);
	LightSpecular = glm::vec3(0.55f, 0.65f, 0.95f);
}

//! Initializes the DevIL image loader utility
/*!
 *
 */
void Renderer::InitializeILUT(void)
{
	ilutRenderer(ILUT_OPENGL);
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
}


//! Writes the log
/*!
 * Wrtites a render log including the used OpenGL renderer (GPU) and it's vendor, OpenGL Version,
 * GLSL Version. The log can be written to console or to a log file
 */
void Renderer::WriteLog(log logLocation)
{
	glinfo_renderer_ptr = glGetString(GL_RENDERER);
	glinfo_vendor_ptr = glGetString(GL_VENDOR);
	glindo_openglVersion_ptr = glGetString(GL_VERSION);
	glinfo_glslVersion_ptr = glGetString(GL_SHADING_LANGUAGE_VERSION);

	glGetIntegerv(GL_MAJOR_VERSION, &glinfo_major);
	glGetIntegerv(GL_MINOR_VERSION, &glinfo_minor);

	if(logLocation == CONSOLE){
		std::cout << "\n------------------------------------------------------------" << std::endl;
		std::cout << "RENDERER INFO LOG" << std::endl;
		std::cout.width(25);
		std::cout << std::left <<"OpenGL Vendor:";
		std::cout.width(10);
		std::cout << std::left << glinfo_vendor_ptr << std::endl;
		std::cout.width(25);
		std::cout << std::left << "OpenGL Renderer:";
		std::cout.width(10);
		std::cout << std::left << glinfo_renderer_ptr << std::endl;
		std::cout.width(25);
		std::cout << "OpenGL Version:";
		std::cout.width(10);
		std::cout << std::left << glindo_openglVersion_ptr << std::endl;
		std::cout.width(25);
		std::cout << "GLSL Version:";
		std::cout.width(10);
		std::cout << std::left << glinfo_glslVersion_ptr << std::endl;
		std::cout << "------------------------------------------------------------" << std::endl;
	}
	else if(logLocation == FILE){
	}
}


//! Calculates frames per second
/*!
 * Calculates the frames per second and can write them to glfw window title or to
 * an AntTweakBar read only variable.
 */
void Renderer::CalculateFPS(double timeInterval, bool toWindowTitle)
{
	//! Static values which only get initialized the first time the function runs
	static double initTime = glfwGetTime(); //! Set the initial time to now
	static int frameCount = 0;             //! Set the initial FPS frame count to 0
	static double fps = 0.0;
	static bool fpsAtBar = false;

	//! Get the current time in seconds since the program started (non-static, so executed every time)
	double currentTime = glfwGetTime();

	//! Calculate and display the FPS every specified time interval
	if ((currentTime - initTime) > timeInterval)
	{
		//! Calculate the FPS as the number of frames divided by the interval in seconds
		fps = (double)frameCount / (currentTime - initTime);

		//! Reset the FPS frame counter and set the initial time to be now
		frameCount = 0;
		initTime = glfwGetTime();
	}
	else //! FPS calculation time interval hasn't elapsed yet? Simply increment the FPS frame counter
	{
		frameCount++;
	}

	if(toWindowTitle){
		std::stringstream sstream;
		sstream << fps;
		std::string newTitle = *context_ptr->GetTitle() + " @ " + sstream.str() + " fps";
		glfwSetWindowTitle(newTitle.c_str());
	}else{
		if(!fpsAtBar){
			TwAddVarRO(context_ptr->GetBar(), "FPS", TW_TYPE_DOUBLE, &fps, "group='General'");
			fpsAtBar = !fpsAtBar;
		}
	}
}


//! Keyboard function
/*!
 * Processes keyboard input using glfw.
 */
void Renderer::KeyboardFunction(void)
{
	//! Reload shader code
	if(glfwGetKey('1'))
	{
		m_gBufferProgram_ptr->ReloadAllShaders();
	}
	if(glfwGetKey('2'))
	{
		deferredProgram_Pass2_ptr->ReloadAllShaders();
	}
	if(glfwGetKey('3'))
	{
		deferredProgram_Pass3_ptr->ReloadAllShaders();
	}
	if(glfwGetKey('4'))
	{
		deferredProgram_Pass4_ptr->ReloadAllShaders();
	}
	if(glfwGetKey('5'))
	{
		deferredProgram_Pass5_ptr->ReloadAllShaders();
	}
	if(glfwGetKey('Z'))
	{
		LightPosition.y += m_speed;
	}
	if(glfwGetKey('H'))
	{
		LightPosition.y -= m_speed;
	}
	if(glfwGetKey('N'))
	{
		tw_useNormalMapping = !tw_useNormalMapping;
	}
	if(glfwGetKey('L'))
	{
		tw_drawLights = !tw_drawLights;
	}
	if(glfwGetKey('R'))
	{
		loaded = !loaded;
	}
	if(tw_mouseLight)
	{
		LightPosition.x = 50.0f * (correct_x_pos/float(context_ptr->GetWidth()));
		LightPosition.z = 50.0f * (correct_y_pos/float(context_ptr->GetHeight()));
	}
}


//!
void Renderer::CameraMovement()
{
	//! Mouse position
	glfwGetMousePos(&x_pos, &y_pos);
	correct_x_pos = x_pos - context_ptr->GetWidth()/2;
	correct_y_pos = y_pos - context_ptr->GetHeight()/2;

	/* Zoom *************************************************/
	//! TODO Try camera movement instead of fov
	int x = glfwGetMouseWheel();
	m_fieldOfView = 50.0f - x;
	scenegraph_ptr->GetActiveCamera()->SetFOV(m_fieldOfView);

	/* Orientation *************************************************/
	if(!tw_cameraMouseButton)
	{
		if(!tw_blockCamera)
		{
			float deltaX = (m_speed * correct_x_pos)/10.0;
			float deltaY = (m_speed * correct_y_pos)/10.0;
			scenegraph_ptr->GetActiveCamera()->Rotate(deltaX, deltaY);
		}
		if(glfwGetKey('B')) //! Block movement key
		{
			tw_blockCamera = !tw_blockCamera;
		}
	}
	else
	{
		if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
		{
			float deltaX = (m_speed * correct_x_pos)/10.0;
			float deltaY = (m_speed * correct_y_pos)/10.0;
			scenegraph_ptr->GetActiveCamera()->Rotate(deltaX, deltaY);
		}
	}

	/* Translation *************************************************/
	if(glfwGetKey('W')) //! Forwards
	{
		scenegraph_ptr->GetActiveCamera()->MoveZ(m_speed);
	}
	if(glfwGetKey('S')) //! Backwards
	{
		scenegraph_ptr->GetActiveCamera()->MoveZ(-m_speed);
	}
	if(glfwGetKey('D')) //! Right
	{
		scenegraph_ptr->GetActiveCamera()->MoveX(m_speed);
	}
	if(glfwGetKey('A')) //! Left
	{
		scenegraph_ptr->GetActiveCamera()->MoveX(-m_speed);
	}
	if(glfwGetKey('U')) //! Up
	{
		scenegraph_ptr->GetActiveCamera()->MoveY(m_speed);
	}
	if(glfwGetKey('J')) //! Down
	{
		scenegraph_ptr->GetActiveCamera()->MoveY(-m_speed);
	}
}

//! Render loop
/*!
 * While variabke RUNNING is true, the renderer loops through this function.
 */
void Renderer::RenderLoop(void){
	//! Single called Open GL calls
	glLineWidth(2.0f);
	glEnable(GL_LINE_SMOOTH);

	//! Draw loop
	while(m_running){
		//! Time calculations
		timedif = time2 - time1;
		m_speed = 5.0f * timedif;
		time1 = glfwGetTime();

		//! Scene loading and organizing
		if(!loaded){
			switch (tw_currentScene) {
				case TESTSCENE:
					m_sceneName = "testscene";
					break;
				case MUSEUM:
					m_sceneName = "museum";
					break;
			}
			scenegraph_ptr->LoadScene(m_sceneName);


			TwEnumVal lightEV[scenegraph_ptr->GetLightCount()];
			for(int l = 0; l < scenegraph_ptr->GetLightCount(); l++)
			{
				lightEV[l].Label = scenegraph_ptr->GetLight(l)->GetName().c_str();
				lightEV[l].Value = l;
				//! Positions
				LightPositions[l][0] = scenegraph_ptr->GetLight(l)->GetPosition().x;
				LightPositions[l][1] = scenegraph_ptr->GetLight(l)->GetPosition().y;
				LightPositions[l][2] = scenegraph_ptr->GetLight(l)->GetPosition().z;
				//! Colors
				LightDiffuses[l][0]  = scenegraph_ptr->GetLight(l)->GetColor().x;
				LightDiffuses[l][1]  = scenegraph_ptr->GetLight(l)->GetColor().y;
				LightDiffuses[l][2]  = scenegraph_ptr->GetLight(l)->GetColor().z;
			}

			TwType lightChoice = TwDefineEnum("Light #", lightEV, scenegraph_ptr->GetLightCount());
			TwAddVarRW(context_ptr->GetBar(), "lightchoice", lightChoice, &tw_currentLight, "label='Light' group='Light' keyIncr='<' keyDecr='>' help='Select editable light.' ");

			renderQ_ptr = Singleton<scene::SceneOrganizer>::Instance()->OrganizeByMaterial();
			std::cout << "RenderQ has size: " << renderQ_ptr->size() << std::endl;
			loaded = true;
		}

		//! Set background color
		glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, 1.0f);

		//! Input handling
		CalculateFPS(0.5, true);
		KeyboardFunction();

		/***************************************/
		//! Manipulating the model matrix
		if(tw_rotation)
			m_angle += tw_rotSpeed;
		glm::vec3 RotationAxis(0, 1, 0);
		glm::mat4 RotationMatrix = glm::rotate(m_angle, RotationAxis);

		/***************************************/
		//! Camera
		CameraMovement();
		if(tw_ortho)
		{
			scenegraph_ptr->GetActiveCamera()->SetOrtho(true);
			tw_orthoSwitched = true;
		}
		if(!tw_ortho)
		{
			scenegraph_ptr->GetActiveCamera()->SetOrtho(false);
			tw_orthoSwitched = true;
		}
		//! Viewmatrix
		ViewMatrix = scenegraph_ptr->GetActiveCamera()->GetViewMatrix();
		//! Projectionmatrixs
		ProjectionMatrix = scenegraph_ptr->GetActiveCamera()->GetProjectionMatrix();

		/************************************
		 *  RENDER LOOP
		 ************************************/

		/*!* * * * * * * * * * * * * * *
		 *		DEFERRED RENDERING	   *
		 *		1ST RENDER PASS		   *
		 *		G-BUFFER	     	   *
		 * * * * * * * * * * * * * * * */
		if(tw_pass1){
			//! Framebuffer object setup and clear buffers
			gBuffer_ptr->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//! Shader program setup & uniform bindings
			m_gBufferProgram_ptr->Use();

			//! Front face culling
			/*
			glEnable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glCullFace(GL_FRONT);
			*/
			//! Multisampling
			glEnable(GL_MULTISAMPLE);

			//! Normal mapping uniform
			m_gBufferProgram_ptr->SetUniform("useNormalMapping", tw_useNormalMapping);
			//! Parallax-corrected cube mapping uniform
			m_gBufferProgram_ptr->SetUniform("toggleCM", tw_CM);
			m_gBufferProgram_ptr->SetUniform("togglePCCM", tw_PCCM);
			//! Billboard reflections uniform
			m_gBufferProgram_ptr->SetUniform("toggleBB", tw_BB);
			//! *** Environment mapping ***
			m_gBufferProgram_ptr->SetUniformCubemap("CubeMapTex", *(materialman_ptr->GetCubeMapByID(0)), 0);

			//! *** Parallax corrected cube maps ***
			//! Scene approximation AAB for parallax corrected cubemaps
			m_gBufferProgram_ptr->SetUniform("AABB.max", scenegraph_ptr->GetSceneApproximation()->max);
			m_gBufferProgram_ptr->SetUniform("AABB.min", scenegraph_ptr->GetSceneApproximation()->min);
			//! Testscene
			m_gBufferProgram_ptr->SetUniform("wsCubeMapPosition", glm::vec3(0, 8.256, 0));
			//! Museum
			//m_gBufferProgram_ptr->SetUniform("wsCubeMapPosition", glm::vec3(0, 12.0, 0));

			//! Billboards
			m_gBufferProgram_ptr->SetUniform("Impostor[0].ModelMatrix", scenegraph_ptr->GetImpostor(0)->GetModelMatrix());
			m_gBufferProgram_ptr->SetUniform("Impostor[1].ModelMatrix", scenegraph_ptr->GetImpostor(1)->GetModelMatrix());
			//! Billboard texture
			m_gBufferProgram_ptr->SetUniformSampler("ImpostorTex[0]", *(scenegraph_ptr->GetImpostor(0)->GetTextureHandle(scene::DIFFUSE)), 1);
			m_gBufferProgram_ptr->SetUniformSampler("ImpostorTex[1]", *(scenegraph_ptr->GetImpostor(1)->GetTextureHandle(scene::DIFFUSE)), 2);
			m_billboardTexCounter = 2;
			for(unsigned int n=0; n < renderQ_ptr->size(); n++)
			{
				//! Mesh geometry
				/******************************************************************************/
				if((*renderQ_ptr)[n]->GetType() == "Mesh")
				{
					//! Camera uniforms
					m_gBufferProgram_ptr->SetUniform("Camera.Position", scenegraph_ptr->GetActiveCamera()->GetPosition());
					m_gBufferProgram_ptr->SetUniform("Camera.NearPlane", scenegraph_ptr->GetActiveCamera()->GetNearPlane());
					m_gBufferProgram_ptr->SetUniform("Camera.FarPlane", scenegraph_ptr->GetActiveCamera()->GetFarPlane());

					//! Camera matrices
					ViewMatrix = scenegraph_ptr->GetActiveCamera()->GetViewMatrix();
					ProjectionMatrix = scenegraph_ptr->GetActiveCamera()->GetProjectionMatrix();
					//! Model matrix
					ModelMatrix = static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetModelMatrix();
					ModelMatrix = glm::mat4(1.0f) * ModelMatrix;


					//! Matrix uniform bindings
					m_gBufferProgram_ptr->SetUniform("wsNormalMatrix", glm::transpose(glm::inverse(ModelMatrix)));
					m_gBufferProgram_ptr->SetUniform("vsNormalMatrix", glm::transpose(glm::inverse(ViewMatrix * ModelMatrix)));
					m_gBufferProgram_ptr->SetUniform("ModelMatrix", ModelMatrix);
					m_gBufferProgram_ptr->SetUniform("ViewMatrix", ViewMatrix);
					m_gBufferProgram_ptr->SetUniform("ProjectionMatrix", ProjectionMatrix);
					m_gBufferProgram_ptr->SetUniform("MVPMatrix", ProjectionMatrix * ViewMatrix * ModelMatrix);
					if(static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->HasTexture())
					{
						m_gBufferProgram_ptr->SetUniformSampler("ColorTex", *(static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetTextureHandle(scene::DIFFUSE)), ++m_billboardTexCounter);
						m_gBufferProgram_ptr->SetUniformSampler("NormalTex", *(static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetTextureHandle(scene::NORMAL)), ++m_billboardTexCounter);
						float materialID = (float)((float)dynamic_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->GetMaterialID()/100.0f);
						m_gBufferProgram_ptr->SetUniform("Material.id", materialID);
						m_gBufferProgram_ptr->SetUniform("Material.reflectance", dynamic_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->GetReflectivity());
					}
					static_cast<scene::Mesh*>((*renderQ_ptr)[n])->Draw();

					//! Draws the bounding boxes
					if(tw_boundingbox)
					{
						m_gBufferProgram_ptr->SetUniform("Material.id", 0.09f);
						m_gBufferProgram_ptr->SetUniform("Material.reflectance", 0.0f);
						static_cast<scene::Mesh*>((*renderQ_ptr)[n])->DrawBoundingBox();
					}
				}

				//! Light geometry
				/******************************************************************************/
				else if((*renderQ_ptr)[n]->GetType() == "Light" && tw_drawLights)
				{
					scenegraph_ptr->SetActiveLight(tw_currentLight);
					scenegraph_ptr->GetActiveLight()->SetPosition(LightPosition);
					scenegraph_ptr->GetActiveLight()->SetColor(LightDiffuse);
					LightPositions[tw_currentLight][0] = LightPosition.x;
					LightPositions[tw_currentLight][1] = LightPosition.y;
					LightPositions[tw_currentLight][2] = LightPosition.z;
					LightDiffuses[tw_currentLight][0] = LightDiffuse.r;
					LightDiffuses[tw_currentLight][1] = LightDiffuse.g;
					LightDiffuses[tw_currentLight][2] = LightDiffuse.b;

					//! Uniforms
					ModelMatrix = static_cast<scene::Light*>((*renderQ_ptr)[n])->GetModelMatrix();
					m_gBufferProgram_ptr->SetUniform("NormalMatrix", glm::transpose( glm::inverse(ViewMatrix * ModelMatrix) ) );
					m_gBufferProgram_ptr->SetUniform("ModelMatrix", ModelMatrix);
					m_gBufferProgram_ptr->SetUniform("ViewMatrix", ViewMatrix);
					m_gBufferProgram_ptr->SetUniform("ModelViewMatrix", ViewMatrix * ModelMatrix);
					m_gBufferProgram_ptr->SetUniform("ProjectionMatrix", ProjectionMatrix);
					m_gBufferProgram_ptr->SetUniform("MVPMatrix", ProjectionMatrix * ViewMatrix * ModelMatrix);

					m_gBufferProgram_ptr->SetUniformSampler("colorTex", *(static_cast<scene::Light*>((*renderQ_ptr)[n])->GetTextureHandle()), 4);
					m_gBufferProgram_ptr->SetUniform("lightColor", (static_cast<scene::Light*>((*renderQ_ptr)[n])->GetColor()));

					m_gBufferProgram_ptr->SetUniform("Material.id", 0.00f);
					m_gBufferProgram_ptr->SetUniform("Material.reflectance", 0.0f);

					scenegraph_ptr->GetLightMesh(scene::POINT)->Draw();
				}
			}

			//! Disable front face culling
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);

			gBuffer_ptr->Unuse();
			m_gBufferProgram_ptr->Unuse();
		}

		/*!* * * * * * * * * * * * * * *
		 *		DEFERRED RENDERING	   *
		 *		2ND RENDER PASS		   *
		 *		LIGHTING			   *
		 * * * * * * * * * * * * * * * */
		if(tw_pass2)
		{
			lighting_fbo_ptr->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			deferredProgram_Pass2_ptr->Use();

			//! Light uniforms
			deferredProgram_Pass2_ptr->SetUniform("ambientColor", LightAmbient);
			deferredProgram_Pass2_ptr->SetUniform("Light.Position", 3, &LightPositions[0][0]);
			deferredProgram_Pass2_ptr->SetUniform("Light.Diffuse", 3, &LightDiffuses[0][0]);

			deferredProgram_Pass2_ptr->SetUniform("Light.Specular", LightSpecular);
			deferredProgram_Pass2_ptr->SetUniform("Light.Count", scenegraph_ptr->GetLightCount());

			//! Material uniforms
			deferredProgram_Pass2_ptr->SetUniform("Shininess", m_shininess);
			//! Matrix uniforms
			deferredProgram_Pass2_ptr->SetUniform("ViewMatrix", scenegraph_ptr->GetActiveCamera()->GetViewMatrix());
			deferredProgram_Pass2_ptr->SetUniform("ProjectionMatrix", scenegraph_ptr->GetActiveCamera()->GetProjectionMatrix());
			//! Mouse uniforms
			//deferredProgram_Pass2_ptr->SetUniform("Mouse.X", static_cast<float>(x_pos));
			//deferredProgram_Pass2_ptr->SetUniform("Mouse.Y", static_cast<float>(y_pos));
			//deferredProgram_Pass2_ptr->SetUniform("mouseLight", tw_mouseLight);
			//! Camera uniforms
			deferredProgram_Pass2_ptr->SetUniform("Camera.Position", scenegraph_ptr->GetActiveCamera()->GetPosition());
			deferredProgram_Pass2_ptr->SetUniform("Camera.NearPlane", scenegraph_ptr->GetActiveCamera()->GetNearPlane());
			deferredProgram_Pass2_ptr->SetUniform("Camera.FarPlane", scenegraph_ptr->GetActiveCamera()->GetFarPlane());
			//! Window uniforms
			deferredProgram_Pass2_ptr->SetUniform("Screen.Width", static_cast<float>(context_ptr->GetWidth()));
			deferredProgram_Pass2_ptr->SetUniform("Screen.Height", static_cast<float>(context_ptr->GetHeight()));
			//! Color attachments
			deferredProgram_Pass2_ptr->SetUniformSampler("wsPositionTex", gBuffer_ptr->GetTexture(0), 0);
			deferredProgram_Pass2_ptr->SetUniformSampler("vsPositionTex", gBuffer_ptr->GetTexture(1), 1);
			deferredProgram_Pass2_ptr->SetUniformSampler("ColorTex", gBuffer_ptr->GetTexture(2), 2);
			deferredProgram_Pass2_ptr->SetUniformSampler("wsNormalTex", gBuffer_ptr->GetTexture(3), 3);
			deferredProgram_Pass2_ptr->SetUniformSampler("vsNormalTex", gBuffer_ptr->GetTexture(4), 4);

			//! Drawing
			fsq_ptr->Draw();

			lighting_fbo_ptr->Unuse();
			deferredProgram_Pass2_ptr->Unuse();
		}

		/*!* * * * * * * * * * * * * * *
		 *		DEFERRED RENDERING	   *
		 *		3RD RENDER PASS		   *
		 * * * * * * * * * * * * * * * */
		if(tw_pass3)
		{
			reflection_fbo_ptr->Use();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			deferredProgram_Pass3_ptr->Use();

			//! *** Screen space reflections ***
			//! Screen space reflections: uniform parameters
			deferredProgram_Pass3_ptr->SetUniform("toggleSSR", tw_SSR);
			deferredProgram_Pass3_ptr->SetUniform("user_pixelStepSize", tw_rayStepSize);
			deferredProgram_Pass3_ptr->SetUniform("fadeToEdges", tw_fadeToEdges);

			//! Camera uniforms
			deferredProgram_Pass3_ptr->SetUniform("Camera.FOV", scenegraph_ptr->GetActiveCamera()->GetFOV());
			deferredProgram_Pass3_ptr->SetUniform("Camera.NearPlane", scenegraph_ptr->GetActiveCamera()->GetNearPlane());
			deferredProgram_Pass3_ptr->SetUniform("Camera.FarPlane", scenegraph_ptr->GetActiveCamera()->GetFarPlane());
			deferredProgram_Pass3_ptr->SetUniform("Camera.Position", scenegraph_ptr->GetActiveCamera()->GetPosition());
			deferredProgram_Pass3_ptr->SetUniform("Camera.LookAt", scenegraph_ptr->GetActiveCamera()->GetLookAt());
			//! Matrix uniforms
			deferredProgram_Pass3_ptr->SetUniform("ViewMatrix", scenegraph_ptr->GetActiveCamera()->GetViewMatrix());
			deferredProgram_Pass3_ptr->SetUniform("ProjectionMatrix", scenegraph_ptr->GetActiveCamera()->GetProjectionMatrix());
			//! Mouse uniforms
			deferredProgram_Pass3_ptr->SetUniform("Mouse.X", static_cast<float>(x_pos));
			deferredProgram_Pass3_ptr->SetUniform("Mouse.Y", static_cast<float>(y_pos));
			//! Window uniforms
			deferredProgram_Pass3_ptr->SetUniform("Screen.Width", static_cast<float>(context_ptr->GetWidth()));
			deferredProgram_Pass3_ptr->SetUniform("Screen.Height", static_cast<float>(context_ptr->GetHeight()));
			//! Texture uniforms from G-Buffer
			deferredProgram_Pass3_ptr->SetUniformSampler("wsPositionTex", 			gBuffer_ptr->GetTexture(0), 0);
			deferredProgram_Pass3_ptr->SetUniformSampler("vsPositionTex", 			gBuffer_ptr->GetTexture(1), 1);
			deferredProgram_Pass3_ptr->SetUniformSampler("wsNormalTex", 			gBuffer_ptr->GetTexture(3), 2);
			deferredProgram_Pass3_ptr->SetUniformSampler("vsNormalTex", 			gBuffer_ptr->GetTexture(4), 3);
			deferredProgram_Pass3_ptr->SetUniformSampler("ReflectanceTex", 			gBuffer_ptr->GetTexture(5), 4);
			deferredProgram_Pass3_ptr->SetUniformSampler("LinearDepthTex", 			gBuffer_ptr->GetTexture(7), 6);
			deferredProgram_Pass3_ptr->SetUniformSampler("DepthTex",				gBuffer_ptr->GetDepthTexture(), 7);
			deferredProgram_Pass3_ptr->SetUniformSampler2DMS("DepthTexMS",			gBuffer_ptr->GetDepthMSTexture(), 8);
			//! Texture uniforms from lighting pass
			deferredProgram_Pass3_ptr->SetUniformSampler("DiffuseTex", lighting_fbo_ptr->GetTexture(0), 8);

			//! Drawing
			fsq_ptr->Draw();

			reflection_fbo_ptr->Unuse();
			deferredProgram_Pass3_ptr->Unuse();
		}

		/*!* * * * * * * * * * * * * * *
		 *		DEFERRED RENDERING	   *
		 *		4TH RENDER PASS		   *
		 * * * * * * * * * * * * * * * */
		/*
		if(tw_pass4)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			deferredProgram_Pass4_ptr->Use();
			bb_fbo_ptr->Use();

			//! Billboards
			//! Impostor uniforms
			deferredProgram_Pass4_ptr->SetUniform("Impostors[0].position", 			scenegraph_ptr->GetImpostor(0)->GetPosition());
			deferredProgram_Pass4_ptr->SetUniform("Impostors[0].normal", 			scenegraph_ptr->GetImpostor(0)->GetNormal());
			deferredProgram_Pass4_ptr->SetUniform("Impostors[0].ModelMatrix", 		scenegraph_ptr->GetImpostor(0)->GetCamera()->GetModelMatrix());
			deferredProgram_Pass4_ptr->SetUniform("Impostors[0].ViewMatrix", 		scenegraph_ptr->GetImpostor(0)->GetCamera()->GetViewMatrix());
			deferredProgram_Pass4_ptr->SetUniform("Impostors[0].ProjectionMatrix",	scenegraph_ptr->GetImpostor(0)->GetCamera()->GetProjectionMatrix());
			deferredProgram_Pass4_ptr->SetUniformSampler("ImpostorTextures[0]",   *(scenegraph_ptr->GetImpostor(0)->GetTextureHandle(scene::DIFFUSE)), 0);

			//! Matrix uniforms
			ViewMatrix = scenegraph_ptr->GetActiveCamera()->GetViewMatrix();
			ProjectionMatrix = scenegraph_ptr->GetActiveCamera()->GetProjectionMatrix();
			deferredProgram_Pass4_ptr->SetUniform("ModelMatrix", ModelMatrix);
			deferredProgram_Pass4_ptr->SetUniform("ViewMatrix", ViewMatrix);
			deferredProgram_Pass4_ptr->SetUniform("ProjectionMatrix", ProjectionMatrix);
			//! Camera uniforms
			deferredProgram_Pass4_ptr->SetUniform("Camera.Position", scenegraph_ptr->GetActiveCamera()->GetPosition());
			deferredProgram_Pass4_ptr->SetUniform("Camera.NearPlane", scenegraph_ptr->GetActiveCamera()->GetNearPlane());
			deferredProgram_Pass4_ptr->SetUniform("Camera.FarPlane", scenegraph_ptr->GetActiveCamera()->GetFarPlane());
			//! Texture uniforms from G-Buffer
			deferredProgram_Pass4_ptr->SetUniformSampler("wsPositionTex", gBuffer_ptr->GetTexture(0), 1);
			deferredProgram_Pass4_ptr->SetUniformSampler("wsNormalTex", gBuffer_ptr->GetTexture(3), 2);
			deferredProgram_Pass4_ptr->SetUniformSampler("ReflectanceTex", gBuffer_ptr->GetTexture(5), 3);
			deferredProgram_Pass4_ptr->SetUniformSampler("DepthTex", gBuffer_ptr->GetDepthTexture(), 4);
			//! Texture uniform from lighting pass
			deferredProgram_Pass4_ptr->SetUniformSampler("DiffuseTex", lighting_fbo_ptr->GetTexture(0), 5);

			fsq_ptr->Draw();

			bb_fbo_ptr->Unuse();
			deferredProgram_Pass4_ptr->Unuse();
		}
		*/
		/*!* * * * * * * * * * * * * * *
		 *		DEFERRED RENDERING	   *
		 *		5TH RENDER PASS		   *
		 *		COMPOSITING			   *
		 * * * * * * * * * * * * * * * */
		if(tw_pass5)
		{
			deferredProgram_Pass5_ptr->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//! ColorattachmentID to choose which attachment is displayed
			deferredProgram_Pass5_ptr->SetUniform("textureID", tw_currentDeferredTex);

			//! Blurring uniforms
			deferredProgram_Pass5_ptr->SetUniform("blurSwitch", tw_blur);
			deferredProgram_Pass5_ptr->SetUniform("kernelX", tw_blurX);
			deferredProgram_Pass5_ptr->SetUniform("kernelY", tw_blurY);
			//! Camera uniforms
			deferredProgram_Pass5_ptr->SetUniform("Camera.Position", scenegraph_ptr->GetActiveCamera()->GetPosition());
			deferredProgram_Pass5_ptr->SetUniform("Camera.NearPlane", scenegraph_ptr->GetActiveCamera()->GetNearPlane());
			deferredProgram_Pass5_ptr->SetUniform("Camera.FarPlane", scenegraph_ptr->GetActiveCamera()->GetFarPlane());
			//! Window uniforms
			deferredProgram_Pass5_ptr->SetUniform("Screen.Width", static_cast<float>(context_ptr->GetWidth()));
			deferredProgram_Pass5_ptr->SetUniform("Screen.Height", static_cast<float>(context_ptr->GetHeight()));
			//! Texture uniforms from G-Buffer
			deferredProgram_Pass5_ptr->SetUniformSampler("wsPositionTex",	gBuffer_ptr->GetTexture(0), 0);
			deferredProgram_Pass5_ptr->SetUniformSampler("vsPositionTex",	gBuffer_ptr->GetTexture(1), 1);
			deferredProgram_Pass5_ptr->SetUniformSampler("ColorTex",		gBuffer_ptr->GetTexture(2), 2);
			deferredProgram_Pass5_ptr->SetUniformSampler("wsNormalTex",		gBuffer_ptr->GetTexture(3), 3);
			deferredProgram_Pass5_ptr->SetUniformSampler("vsNormalTex",		gBuffer_ptr->GetTexture(4), 4);
			deferredProgram_Pass5_ptr->SetUniformSampler("ReflectanceTex",	gBuffer_ptr->GetTexture(5), 5);
			deferredProgram_Pass5_ptr->SetUniformSampler("ReflecVecTex", 	gBuffer_ptr->GetTexture(6), 6);
			deferredProgram_Pass5_ptr->SetUniformSampler("LinearDepthTex", 	gBuffer_ptr->GetTexture(7), 7);
			deferredProgram_Pass5_ptr->SetUniformSampler("DepthTex",		gBuffer_ptr->GetDepthTexture(), 8);
			deferredProgram_Pass5_ptr->SetUniformSampler2DMS("DepthMSTex",	gBuffer_ptr->GetDepthMSTexture(), 9);
			//! Texture uniforms from lighting pass
			deferredProgram_Pass5_ptr->SetUniformSampler("DiffuseTex", lighting_fbo_ptr->GetTexture(0), 9);
			//! Texture uniform from SSR pass
			deferredProgram_Pass5_ptr->SetUniformSampler("SSRTex", reflection_fbo_ptr->GetTexture(0), 10);
			//! Texture uniform from BB pass
			//deferredProgram_Pass5_ptr->SetUniformSampler("BBTex", bb_fbo_ptr->GetTexture(0), 11);

			//! Drawing
			fsq_ptr->Draw();

			deferredProgram_Pass5_ptr->Unuse();
		}

		//! Draw AntTweakBar-GUI
		TwDraw();

		context_ptr->SwapBuffers();

		//! Time calculations end
		time2 = glfwGetTime();

		//! Check for context exiting
		if(context_ptr->IsExiting()){
			m_running = false;
		}
	}
}
