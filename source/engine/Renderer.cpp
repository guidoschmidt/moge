//! Renderer.cpp
/*!
 * @date 	28.11.2012
 * @author 	Guido Schmidt
 */

#include "Renderer.h"


//! ANTTWEAKBAR CALLBACKS BEGIN
/*! Variables and button callbacks */
bool tw_deferred;
bool tw_rotation;
bool loaded;
float theta = 0, phi = 0;
float r = 0;
static void TW_CALL SwitchDeffered(void* clientData){ tw_deferred = !tw_deferred;}
static void TW_CALL SwitchRotation(void* clientData){ tw_rotation = !tw_rotation;}
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
	m_fieldOfView = 60.0f;

	time1 = 0.0f;
	time2 = 0.0f;

	loaded = false;

	tw_deferred = true;
	tw_currentScene = HEAD;
	tw_currentDeferredTex = TEX_COMPOSIT;
	tw_rotation = false;
	tw_mouseLight = false;
	tw_deltaDepth = 0.0125f;
	tw_SSR = false;
	tw_compareDepth = false;
	tw_reflvec = false;
	tw_jittering = false;

	context_ptr = Singleton<Context>::Instance();
	fsq_ptr = new FSQ();
	m_shininess = 12.0f;

	m_backgroundColor = glm::vec3(0.15f, 0.25f, 0.35f);

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
	context_ptr->OpenWindow(width, height, "Render Window", 4, 2);
	context_ptr->AddAntTweakBar();

	WriteLog(CONSOLE);
	InitGLEW();
	//! Initialize image loader utility
	InitializeILUT();

	glEnable(GL_DEPTH_TEST);

	/*!
	 * AntTweakBar
	 * TODO Set HSV as standard for color inputs
	 */
	TwAddButton(context_ptr->GetBar(), "toggledeferred", SwitchDeffered, NULL, "key='space' label='Toggle Deferred Rendering' group='Rendering'");
	//! Deferred: render targets choice
	TwEnumVal texEV[NUM_TEXS] = { {TEX_COMPOSIT, "Composited"}, {TEX_POSITION, "Positionmap"} ,{TEX_COLOR, "Colormap"}, {TEX_NORMAL, "Normalmap"}, {TEX_MATID, "Material ID"}, {TEX_REFL, "Reflectance"}, {TEX_DEPTH, "Depthmap"}};
	TwType texType = TwDefineEnum("TextureType", texEV, NUM_TEXS);
	TwAddVarRW(context_ptr->GetBar(), "deferredTextureChoice", texType, &tw_currentDeferredTex, "label='Rendering' group='Rendering' keyIncr='<' keyDecr='>' help='View the maps rendered in first pass.' ");
	//! Scene choice
	TwEnumVal sceneEV[NUM_SCENES] = { {HEAD, "Head"}, {GEOMETRY, "Geometry"}};
	TwType sceneType = TwDefineEnum("SceneType", sceneEV, NUM_SCENES);
	TwAddVarRW(context_ptr->GetBar(), "sceneChoice", sceneType, &tw_currentScene, "label='Scene' group='Scene' keyIncr='<' keyDecr='>' help='Load another scene.' ");
	//! Material
	TwAddVarRW(context_ptr->GetBar(), "shininess", TW_TYPE_FLOAT, &m_shininess, "step='0.01' max='100.0' min='0.0' label='Shininess' group='Material'");
	//! Background color
	TwAddVarRW(context_ptr->GetBar(), "background", TW_TYPE_COLOR3F, &m_backgroundColor, "label='Background' group='Scene' colormode='hls'");
	//! Rotation
	TwAddButton(context_ptr->GetBar(), "togglerotation", SwitchRotation, NULL, "key='r' label='Toggle Rotation' group='Rotation'");
	TwAddVarRW(context_ptr->GetBar(), "rotationSpeed", TW_TYPE_FLOAT, &tw_rotSpeed, "step='0.001' max='1.0' min='0.0' label='Rotationspeed' group='Rotation'");
	//! Camera
	TwAddVarRW(context_ptr->GetBar(), "fieldofview", TW_TYPE_FLOAT, &m_fieldOfView, "step='0.1' max='170.0' min='2.0' label='Field Of View' group='Camera'");
	//! Mouse
	TwAddVarRO(context_ptr->GetBar(), "mousex", TW_TYPE_INT16, &correct_x_pos, "group='Mouse' label='X'");
	TwAddVarRO(context_ptr->GetBar(), "mousey", TW_TYPE_INT16, &correct_y_pos, "group='Mouse' label='Y'");
	TwAddVarRW(context_ptr->GetBar(), "mouselight", TW_TYPE_BOOLCPP, &tw_mouseLight, "key='m' group='Light' label='Mouse controlled'");
	//! SSR parameters
	TwAddVarRW(context_ptr->GetBar(), "ssrdeltadepth", TW_TYPE_FLOAT, &tw_deltaDepth, "step='0.0001' group='SSR' label='Depth delta'");
	TwAddVarRW(context_ptr->GetBar(), "ssr", TW_TYPE_BOOLCPP, &tw_SSR, "group='SSR' label='Switch SSR'");
	TwAddVarRW(context_ptr->GetBar(), "blur", TW_TYPE_BOOLCPP, &tw_blur, "group='SSR' label='Blur'");
	TwAddVarRW(context_ptr->GetBar(), "jittering", TW_TYPE_BOOLCPP, &tw_jittering, "group='SSR' label='Jitter'");
	TwAddVarRW(context_ptr->GetBar(), "comparedepth", TW_TYPE_BOOLCPP, &tw_compareDepth, "group='SSR' label='Debug dephts'");
	TwAddVarRW(context_ptr->GetBar(), "showvecs", TW_TYPE_BOOLCPP, &tw_reflvec, "group='SSR' label='Debug reflection vectors'");

	//! Initialize singleton instances
	scenegraph_ptr = Singleton<scene::SceneGraph>::Instance();

	/*! Init forward rendering
	 **************************/
		forwardProgram_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/forward/forward.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/forward/forward.frag.glsl"
		);

	/*! Init deferred rendering
	 * *************************/
		//! Inits for 1st pass
		deferredProgram_Pass1_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/1-pass_one.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/1-pass_one.frag.glsl"
		);
		gBuffer_ptr = new FrameBufferObject(context_ptr->GetWidth(), context_ptr->GetHeight());
		//! Inits for 2nd pass
		deferredProgram_Pass2_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/2-deferred_lighting.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/2-deferred_lighting.frag.glsl"
		);
		fbo_ptr = new FrameBufferObject(context_ptr->GetWidth(), context_ptr->GetHeight());
		//! Inits for 3nd pass
		deferredProgram_Pass3_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/3-deferred_reflections.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/3-deferred_reflections.frag.glsl"
		);
		//! Fullscreen quad init
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
	LightPosition = glm::vec4(0.0f, 1.0f, -10.0f, 0.0f);
	LightDiffuse = glm::vec3(0.65f, 0.45f, 0.45f);
	LightSpecular = glm::vec3(0.55f, 0.65f, 0.95f);

	TwAddVarRW(context_ptr->GetBar(), "lightDiffuse", TW_TYPE_COLOR3F, &LightDiffuse, "label='Diffuse' group='Light' colormode='hls'");
	TwAddVarRW(context_ptr->GetBar(), "lightSpecular", TW_TYPE_COLOR3F, &LightSpecular, "label='Specular' group='Light' colormode='hls'");
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
		deferredProgram_Pass1_ptr->ReloadAllShaders();
	}
	if(glfwGetKey('2'))
	{
		deferredProgram_Pass2_ptr->ReloadAllShaders();
	}
	if(glfwGetKey('3'))
	{
		deferredProgram_Pass3_ptr->ReloadAllShaders();
	}
}


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

	/* Orientation *************************************************/
	if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
	{
		//! Calculate angles
		phi = correct_y_pos * 0.00001f;
		theta = correct_x_pos * 0.00001f;

		scenegraph_ptr->GetActiveCamera()->Pitch(phi);
		scenegraph_ptr->GetActiveCamera()->Yaw(theta);
	}

	/* Translation *************************************************/
	if(glfwGetKey('W')) //! Forwards
	{
		scenegraph_ptr->GetActiveCamera()->Move(0, 0, m_speed);
	}
	if(glfwGetKey('S')) //! Backwards
	{
		scenegraph_ptr->GetActiveCamera()->Move(0, 0, -m_speed);
	}
	if(glfwGetKey('A')) //! Left
	{
		scenegraph_ptr->GetActiveCamera()->Move(m_speed, 0, 0);
	}
	if(glfwGetKey('D')) //! Right
	{
		scenegraph_ptr->GetActiveCamera()->Move(-m_speed, 0, 0);
	}
	if(glfwGetKey('U')) //! Up
	{
		scenegraph_ptr->GetActiveCamera()->Move(0, m_speed, 0);
	}
	if(glfwGetKey('J')) //! Down
	{
		scenegraph_ptr->GetActiveCamera()->Move(0, -m_speed, 0);
	}

	/* Reset *************************************************/
	if(glfwGetKey('E'))
	{
		scenegraph_ptr->GetActiveCamera()->Reset();
	}
}


//! Render loop
/*!
 * While variabke RUNNING is true, the renderer loops through this function.
 */
void Renderer::RenderLoop(void){
	while(m_running){
		//! Time calculations
		timedif = time2 - time1;
		m_speed = 2.0f * timedif;

		time1 = glfwGetTime();

		//! Scene loading
		if(!loaded){
			switch (tw_currentScene) {
				case HEAD:
					scenegraph_ptr->LoadSceneFromFile("./assets/scenes/collada/SimpleScene.dae");
					break;
				case GEOMETRY:
					scenegraph_ptr->LoadSceneFromFile("./assets/scenes/blend/Scene.blend");
					break;
				case BUDDHA:
					scenegraph_ptr->LoadSceneFromFile("./assets/scenes/collada/Buddha.dae");
					break;
				case TEAPOT:
					scenegraph_ptr->LoadSceneFromFile("./assets/scenes/blend/Teapot.blend");
					break;
				default:
					scenegraph_ptr->LoadSceneFromFile("./assets/scenes/blend/Head.blend");
					break;
			}
			renderQ_ptr = Singleton<scene::SceneOrganizer>::Instance()->OrganizeScene();
			std::cout << "RenderQ has size: " << renderQ_ptr->size() << std::endl;
			loaded = true;
		}

		//! Set background color
		glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, 1.0f);

		//! Input handling
		CalculateFPS(0.5, false);
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
		//! Viewmatrix
		ViewMatrix = scenegraph_ptr->GetActiveCamera()->GetViewMatrix();
		//! Projectionmatrixs
		ProjectionMatrix = scenegraph_ptr->GetActiveCamera()->GetProjectionMatrix();

		/************************************
		 *  RENDER LOOP
		 ************************************/
		if(tw_deferred){
			/*!* * * * * * * * * * * * * * *
			 *		DEFERRED RENDERING	   *
			 *		1ST RENDER PASS		   *
			 * * * * * * * * * * * * * * * */
			//! Framebuffer object setup and clear buffers
			gBuffer_ptr->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//! Shader program setup & uniform bindings
			deferredProgram_Pass1_ptr->Use();

			//! Drawing
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
				}
			}

			gBuffer_ptr->Unuse();
			deferredProgram_Pass1_ptr->Unuse();
			/*!* * * * * * * * * * * * * * *
			 *		DEFERRED RENDERING	   *
			 *		2ND RENDER PASS		   *
			 * * * * * * * * * * * * * * * */
			fbo_ptr->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			deferredProgram_Pass2_ptr->Use();
			//! Light uniforms
			deferredProgram_Pass2_ptr->SetUniform("Light.Position", LightPosition);
			deferredProgram_Pass2_ptr->SetUniform("Light.Ambient", LightAmbient);
			deferredProgram_Pass2_ptr->SetUniform("Light.Diffuse", LightDiffuse);
			deferredProgram_Pass2_ptr->SetUniform("Light.Specular", LightSpecular);
			//! Material uniforms
			deferredProgram_Pass2_ptr->SetUniform("Shininess", m_shininess);
			//! Matrix uniforms
			deferredProgram_Pass2_ptr->SetUniform("ViewMatrix", scenegraph_ptr->GetActiveCamera()->GetViewMatrix());
			deferredProgram_Pass2_ptr->SetUniform("ProjectionMatrix", scenegraph_ptr->GetActiveCamera()->GetProjectionMatrix());
			//! Mouse uniforms
			deferredProgram_Pass2_ptr->SetUniform("Mouse.X", static_cast<float>(x_pos));
			deferredProgram_Pass2_ptr->SetUniform("Mouse.Y", static_cast<float>(y_pos));
			deferredProgram_Pass2_ptr->SetUniform("mouseLight", tw_mouseLight);
			//! Camera uniforms
			deferredProgram_Pass2_ptr->SetUniform("Camera.Position", scenegraph_ptr->GetActiveCamera()->GetPosition());
			deferredProgram_Pass2_ptr->SetUniform("Camera.NearPlane", scenegraph_ptr->GetActiveCamera()->GetNearPlane());
			deferredProgram_Pass2_ptr->SetUniform("Camera.FarPlane", scenegraph_ptr->GetActiveCamera()->GetFarPlane());
			//! Window uniforms
			deferredProgram_Pass2_ptr->SetUniform("Screen.Width", static_cast<float>(context_ptr->GetWidth()));
			deferredProgram_Pass2_ptr->SetUniform("Screen.Height", static_cast<float>(context_ptr->GetHeight()));
			//! Colorattachments
			deferredProgram_Pass2_ptr->SetUniformSampler("deferredPositionTex", gBuffer_ptr->GetTexture(0), 0);
			deferredProgram_Pass2_ptr->SetUniformSampler("deferredColorTex", gBuffer_ptr->GetTexture(1), 1);
			deferredProgram_Pass2_ptr->SetUniformSampler("deferredNormalTex", gBuffer_ptr->GetTexture(2), 2);
			deferredProgram_Pass2_ptr->SetUniformSampler("deferredMaterialIDTex", gBuffer_ptr->GetTexture(3), 3);
			deferredProgram_Pass2_ptr->SetUniformSampler("deferredReflectanceTex", gBuffer_ptr->GetTexture(4), 4);
			deferredProgram_Pass2_ptr->SetUniformSampler("deferredDepthTex", gBuffer_ptr->GetDepthTexture(), 5);

			//! Drawing
			fsq_ptr->Draw();

			fbo_ptr->Unuse();
			deferredProgram_Pass2_ptr->Unuse();
			/*!* * * * * * * * * * * * * * *
			 *		DEFERRED RENDERING	   *
			 *		3RD RENDER PASS		   *
			 * * * * * * * * * * * * * * * */
			deferredProgram_Pass3_ptr->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//! Raytrace uniform parameters
			deferredProgram_Pass3_ptr->SetUniform("deltaDepth", tw_deltaDepth);
			deferredProgram_Pass3_ptr->SetUniform("SSR", tw_SSR);
			deferredProgram_Pass3_ptr->SetUniform("blur", tw_blur);
			deferredProgram_Pass3_ptr->SetUniform("jittering", tw_jittering);
			deferredProgram_Pass3_ptr->SetUniform("compareDepth", tw_compareDepth);
			deferredProgram_Pass3_ptr->SetUniform("showReflVecs", tw_reflvec);
			//! Camera uniforms
			deferredProgram_Pass3_ptr->SetUniform("Camera.Position", scenegraph_ptr->GetActiveCamera()->GetPosition());
			deferredProgram_Pass3_ptr->SetUniform("Camera.NearPlane", scenegraph_ptr->GetActiveCamera()->GetNearPlane());
			deferredProgram_Pass3_ptr->SetUniform("Camera.FarPlane", scenegraph_ptr->GetActiveCamera()->GetFarPlane());
			//! Matrix uniforms
			deferredProgram_Pass3_ptr->SetUniform("ViewMatrix", scenegraph_ptr->GetActiveCamera()->GetViewMatrix());
			deferredProgram_Pass3_ptr->SetUniform("ProjectionMatrix", scenegraph_ptr->GetActiveCamera()->GetProjectionMatrix());
			//! Mouse uniforms
			deferredProgram_Pass3_ptr->SetUniform("Mouse.X", static_cast<float>(x_pos));
			deferredProgram_Pass3_ptr->SetUniform("Mouse.Y", static_cast<float>(y_pos));
			//! Window uniforms
			deferredProgram_Pass3_ptr->SetUniform("Screen.Width", static_cast<float>(context_ptr->GetWidth()));
			deferredProgram_Pass3_ptr->SetUniform("Screen.Height", static_cast<float>(context_ptr->GetHeight()));
			//! ColorattachmentID to choose which attachment is displayed
			deferredProgram_Pass3_ptr->SetUniform("textureID", tw_currentDeferredTex);
			//! Colorattachments
			deferredProgram_Pass3_ptr->SetUniformSampler("deferredPositionTex", gBuffer_ptr->GetTexture(0), 0);
			deferredProgram_Pass3_ptr->SetUniformSampler("deferredColorTex", gBuffer_ptr->GetTexture(1), 1);
			deferredProgram_Pass3_ptr->SetUniformSampler("deferredNormalTex", gBuffer_ptr->GetTexture(2), 2);
			deferredProgram_Pass3_ptr->SetUniformSampler("deferredMaterialIDTex", gBuffer_ptr->GetTexture(3), 3);
			deferredProgram_Pass3_ptr->SetUniformSampler("deferredReflectanceTex", gBuffer_ptr->GetTexture(4), 4);
			deferredProgram_Pass3_ptr->SetUniformSampler("deferredDepthTex", gBuffer_ptr->GetDepthTexture(), 5);
			deferredProgram_Pass3_ptr->SetUniformSampler("deferredDiffuseTex", fbo_ptr->GetTexture(0), 6);

			//! Drawing
			fsq_ptr->Draw();

			deferredProgram_Pass3_ptr->Unuse();
		}
		else{
			/*!* * * * * * * * * * * * * * *
			 *		FORWARD RENDERING	   *
			 * * * * * * * * * * * * * * * */
			forwardProgram_ptr->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//! Light uniforms
			forwardProgram_ptr->SetUniform("Light.Position", LightPosition);
			forwardProgram_ptr->SetUniform("Light.Ambient", LightAmbient);
			forwardProgram_ptr->SetUniform("Light.Diffuse", LightDiffuse);
			forwardProgram_ptr->SetUniform("Light.Specular", LightSpecular);
			//! Material uniforms
			forwardProgram_ptr->SetUniform("Shininess", m_shininess);
			//! Mouse uniforms
			forwardProgram_ptr->SetUniform("Mouse.X", static_cast<float>(x_pos));
			forwardProgram_ptr->SetUniform("Mouse.Y", static_cast<float>(y_pos));
			forwardProgram_ptr->SetUniform("mouseLight", tw_mouseLight);
			//! Window uniforms
			forwardProgram_ptr->SetUniform("Screen.Width", static_cast<float>(context_ptr->GetWidth()));
			forwardProgram_ptr->SetUniform("Screen.Height", static_cast<float>(context_ptr->GetHeight()));

			//! Drawing
			for(unsigned int n=0; n < renderQ_ptr->size(); n++)
			{
				if(static_cast<scene::Mesh*>((*renderQ_ptr)[n]))
				{
					ModelMatrix = static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetModelMatrix();
					ModelMatrix = glm::mat4(1.0f) * RotationMatrix * ModelMatrix;
					ViewMatrix = scenegraph_ptr->GetActiveCamera()->GetViewMatrix();
					ProjectionMatrix = scenegraph_ptr->GetActiveCamera()->GetProjectionMatrix();
					//! Matrix uniforms
					forwardProgram_ptr->SetUniform("NormalMatrix", glm::transpose(glm::inverse(ModelMatrix)));
					forwardProgram_ptr->SetUniform("ModelMatrix", ModelMatrix);
					forwardProgram_ptr->SetUniform("ViewMatrix", ViewMatrix);
					forwardProgram_ptr->SetUniform("ModelViewMatrix", ViewMatrix * ModelMatrix);
					forwardProgram_ptr->SetUniform("ProjectionMatrix", ProjectionMatrix);
					forwardProgram_ptr->SetUniform("MVPMatrix", ProjectionMatrix * ViewMatrix * ModelMatrix);
					if(static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->HasTexture())
					{
						int current_tex_id = dynamic_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->GetID();
						forwardProgram_ptr->SetUniformSampler("colorTex", *(Singleton<scene::MaterialManager>::Instance()->GetTexture(current_tex_id)), 5);
					}
					static_cast<scene::Mesh*>((*renderQ_ptr)[n])->Draw();
				}
			}

			forwardProgram_ptr->Unuse();
		}

		//! Draw AntTweakBar-GUI
		TwDraw();
		context_ptr->SwapBuffers();

		time2 = glfwGetTime();
		//! Check for context exiting
		if(context_ptr->IsExiting()){
			m_running = false;
		}
	}
}
