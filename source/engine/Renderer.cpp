//! Renderer.cpp
/*!
 * @date 	28.11.2012
 * @author 	Guido Schmidt
 */

#include "Renderer.h"


//! ANTTWEAKBAR CALLBACKS BEGIN
/*! Variables and button callbacks */
bool deferred;
bool loaded;
bool rotation;
static void TW_CALL SwitchDeffered(void* clientData){ deferred = !deferred;}
static void TW_CALL SwitchRotation(void* clientData){ rotation = !rotation;}
//! ANTTWEAKBAR CALLBACKS END


//! Constructor
/*!
 *
 * @param width
 * @param height
 */
Renderer::Renderer(int width, int height)
{
	deferred = true;
	rotation = true;
	rotSpeed = 0.05f;

	currentScene = CONFERENCE;
	currentDeferredTex = TEX_COMPOSIT;
	loaded = false;

	context = new Context();
	fsq = new FSQ();
	Shininess = 12.0f;

	BackgroundColor = glm::vec3(0.15f, 0.25f, 0.35f);

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
	context->OpenWindow(width, height, "Render Window", 4, 2);
	context->AddAntTweakBar();

	WriteLog(CONSOLE);
	InitGLEW();
	//! Initialize image loader utility
	InitializeILUT();

	glEnable(GL_DEPTH_TEST);

	//! AntTweakBar
	TwAddButton(context->GetBar(), "toggledeferred", SwitchDeffered, NULL, "key='space' label='Toggle Deferred Rendering' group='Rendering'");
	//! Deferred: render targets choice
	TwEnumVal texEV[NUM_TEXS] = { {TEX_COMPOSIT, "Composited"}, {TEX_POSITION, "Positionmap"} ,{TEX_COLOR, "Colormap"}, {TEX_NORMAL, "Normalmap"}, {TEX_DEPTH, "Depthmap"}};
	TwType texType = TwDefineEnum("TextureType", texEV, NUM_TEXS);
	TwAddVarRW(context->GetBar(), "deferredTextureChoice", texType, &currentDeferredTex, "label='Rendering' group='Rendering' keyIncr='<' keyDecr='>' help='View the maps rendered in first pass.' ");
	//! Scene choice
	TwEnumVal sceneEV[NUM_SCENES] = { {HEAD, "Head"}, {GEOMETRY, "Geometry"}};
	TwType sceneType = TwDefineEnum("SceneType", sceneEV, NUM_SCENES);
	TwAddVarRW(context->GetBar(), "sceneChoice", sceneType, &currentScene, "label='Scene' group='Scene' keyIncr='<' keyDecr='>' help='Load another scene.' ");
	//! Material
	TwAddVarRW(context->GetBar(), "shininess", TW_TYPE_FLOAT, &Shininess, "step='0.01' max='100.0' min='0.0' label='Shininess' group='Material'");
	//! Background color
	TwAddVarRW(context->GetBar(), "background", TW_TYPE_COLOR3F, &BackgroundColor, "label='Background' group='Scene'");
	//! Rotation
	TwAddButton(context->GetBar(), "togglerotation", SwitchRotation, NULL, "key='r' label='Toggle Rotation' group='Rotation'");
	TwAddVarRW(context->GetBar(), "rotationSpeed", TW_TYPE_FLOAT, &rotSpeed, "step='0.001' max='1.0' min='0.0' label='Rotationspeed' group='Rotation'");

	//! Initialize singleton instances
	scenegraph = Singleton<scene::SceneGraph>::Instance();

	/*! Init forward rendering
	 **************************/
		forwardProgram = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/forward/forward.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/forward/forward.frag.glsl"
		);

	/*! Init deferred rendering
	 * *************************/
		//! Inits for 1st pass
		deferredProgram_Pass1 = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/pass_one.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/pass_one.frag.glsl"
		);
		firstPassFBO = new FrameBufferObject(
				context->GetWidth(), context->GetHeight()
		);

		//! Inits for 2nd pass
		deferredProgram_Pass2 = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/deferred.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/deferred.frag.glsl"
		);
		fsq->CreateFSQ();

	InitializeMatrices();
	InitializeLight();

	RUNNING = true;
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
	CameraPosition = glm::vec3(0.0, 2.0, 10.0);
	CameraTargetPosition = glm::vec3(0.0, 1.0, 0.0);
	CameraUp = glm::vec3(0, 1, 0);

	//! Projectionmatrix
	ProjectionMatrix = glm::perspective(60.0f, (float)context->GetWidth()/(float)context->GetHeight(), 0.01f, 100.0f);
}

//! Initialize light setup
/*!
 * Creates a light setup, including light's position, ambient, diffuse & specular color.
 * Adds AntTweakBar variables, too.
 */
void Renderer::InitializeLight(void)
{
	LightPosition = glm::vec4(0.0f, 5.0f, 2.0f, 0.0f);
	LightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
	LightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	LightSpecular = glm::vec3(0.35f, 0.45f, 0.55f);

	TwAddVarRW(context->GetBar(), "lightAmbient", TW_TYPE_COLOR3F, &LightAmbient, "label='Ambient' group='Light'");
	TwAddVarRW(context->GetBar(), "lightDiffuse", TW_TYPE_COLOR3F, &LightDiffuse, "label='Diffuse' group='Light'");
	TwAddVarRW(context->GetBar(), "lightSpecular", TW_TYPE_COLOR3F, &LightSpecular, "label='Specular' group='Light'");
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
	renderer = glGetString(GL_RENDERER);
	vendor = glGetString(GL_VENDOR);
	openglVersion = glGetString(GL_VERSION);
	glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	if(logLocation == CONSOLE){
		std::cout << "\n------------------------------------------------------------" << std::endl;
		std::cout << "RENDERER INFO LOG" << std::endl;
		std::cout.width(25);
		std::cout << std::left <<"OpenGL Vendor:";
		std::cout.width(10);
		std::cout << std::left << vendor << std::endl;
		std::cout.width(25);
		std::cout << std::left << "OpenGL Renderer:";
		std::cout.width(10);
		std::cout << std::left << renderer << std::endl;
		std::cout.width(25);
		std::cout << "OpenGL Version:";
		std::cout.width(10);
		std::cout << std::left << openglVersion << std::endl;
		std::cout.width(25);
		std::cout << "GLSL Version:";
		std::cout.width(10);
		std::cout << std::left << glslVersion << std::endl;
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
	static double initTime       = glfwGetTime(); //! Set the initial time to now
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
		std::string newTitle = *context->GetTitle() + " @ " + sstream.str() + " fps";
		glfwSetWindowTitle(newTitle.c_str());
	}else{
		if(!fpsAtBar){
			TwAddVarRO(context->GetBar(), "FPS", TW_TYPE_DOUBLE, &fps, "group='General'");
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
	//! Camera
	double speed = 0.005;
	if(glfwGetKey('W'))
	{
		scenegraph->GetActiveCamera()->TranslateZ(-speed);
	}
	if(glfwGetKey('S'))
	{
		scenegraph->GetActiveCamera()->TranslateZ(speed);
	}
	if(glfwGetKey('A'))
	{
		scenegraph->GetActiveCamera()->TranslateX(-speed);
	}
	if(glfwGetKey('D'))
	{
		scenegraph->GetActiveCamera()->TranslateX(speed);
	}

	//! Light
	if(glfwGetKey('I'))
	{
		LightPosition.z += speed;
	}
	if(glfwGetKey('K'))
	{
		LightPosition.z -= speed;
	}
	if(glfwGetKey('J'))
	{
		LightPosition.x += speed;
	}
	if(glfwGetKey('L'))
	{
		LightPosition.x -= speed;
	}
}


//! Render loop
/*!
 * While variabke RUNNING is true, the renderer loops thorugh this function.
 */
void Renderer::RenderLoop(void){
	while(RUNNING){
		if(!loaded){
			switch (currentScene) {
				case HEAD:
					scenegraph->LoadSceneFromFile("./assets/geometry/collada/Head.dae");
					loaded = true;
					break;
				case GEOMETRY:
					scenegraph->LoadSceneFromFile("./assets/geometry/blend/Scene.blend");
					loaded = true;
					break;
				default:
					scenegraph->LoadSceneFromFile("./assets/geometry/blend/Head.blend");
					loaded = true;
					break;
			}
		}

		//! Set background color
		glClearColor(BackgroundColor.x, BackgroundColor.y, BackgroundColor.z, 1.0f);

		//! Calculations
		CalculateFPS(0.5, false);
		KeyboardFunction();
		if(rotation)
			angle += rotSpeed;
		//! Modelmatrix
		glm::vec3 RotationAxis(0, 1, 0);
		glm::mat4 RotationMatrix = glm::rotate(angle, RotationAxis);
		//! Viewmatrix
		//ViewMatrix = glm::lookAt(CameraPosition, CameraTargetPosition, CameraUp);
		ViewMatrix = scenegraph->GetActiveCamera()->GetViewMatrix();


		/************************************
		 *  DRAWING
		 ************************************/
		if(deferred){
			/*!* * * * * * * * * * * * * * *
			 *		DEFERRED RENDERING	   *
			 *		1ST RENDER PASS		   *
			 * * * * * * * * * * * * * * * */
			//! Framebuffer object setup and clear buffers
			firstPassFBO->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//! Shader program setup & uniform bindings
			deferredProgram_Pass1->Use();
			//! uniform camera binding
			deferredProgram_Pass1->SetUniform("camera", CameraPosition);

			//! Drawing
			//! Head
			deferredProgram_Pass1->SetUniformSampler("colorTex", *(Singleton<scene::MaterialManager>::Instance()->GetTexture(0)), 5);
			ModelMatrix = scenegraph->DrawNode(2);
			ModelMatrix = glm::mat4(1.0f) * RotationMatrix * ModelMatrix;
			MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
			deferredProgram_Pass1->SetUniform("mvp", MVPMatrix);
			//! Street
			deferredProgram_Pass1->SetUniformSampler("colorTex", *(Singleton<scene::MaterialManager>::Instance()->GetTexture(1)), 5);
			ModelMatrix = scenegraph->DrawNode(1);
			ModelMatrix = glm::mat4(1.0f) * RotationMatrix * ModelMatrix;
			MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
			deferredProgram_Pass1->SetUniform("mvp", MVPMatrix);

			firstPassFBO->Unuse();
			deferredProgram_Pass1->Unuse();
			/*!* * * * * * * * * * * * * * *
			 *		DEFERRED RENDERING	   *
			 *		2ND RENDER PASS		   *
			 * * * * * * * * * * * * * * * */

			deferredProgram_Pass2->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			deferredProgram_Pass2->SetUniform("Light.Position", LightPosition);
			deferredProgram_Pass2->SetUniform("Light.Ambient", LightAmbient);
			deferredProgram_Pass2->SetUniform("Light.Diffuse", LightDiffuse);
			deferredProgram_Pass2->SetUniform("Light.Specular", LightSpecular);
			deferredProgram_Pass2->SetUniform("Shininess", Shininess);

			deferredProgram_Pass2->SetUniform("textureID", currentDeferredTex);

			deferredProgram_Pass1->SetUniform("camera", CameraPosition);

			deferredProgram_Pass2->SetUniformSampler("deferredPositionTex", firstPassFBO->GetTexture(0), 1);
			deferredProgram_Pass2->SetUniformSampler("deferredColorTex", firstPassFBO->GetTexture(1), 2);
			deferredProgram_Pass2->SetUniformSampler("deferredNormalTex", firstPassFBO->GetTexture(2), 3);
			deferredProgram_Pass2->SetUniformSampler("deferredDepthTex", firstPassFBO->GetDepthTexture(), 4);

			fsq->Draw();

			deferredProgram_Pass2->Unuse();
		}
		else{
			/*!* * * * * * * * * * * * * * *
			 *		FORWARD RENDERING	   *
			 * * * * * * * * * * * * * * * */
			forwardProgram->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//! Uniform bindings
			forwardProgram->SetUniform("Light.Position", LightPosition);
			forwardProgram->SetUniform("Light.Ambient", LightAmbient);
			forwardProgram->SetUniform("Light.Diffuse", LightDiffuse);
			forwardProgram->SetUniform("Light.Specular", LightSpecular);

			forwardProgram->SetUniform("mvp", MVPMatrix);

			//! Drawing
			//! Head
			forwardProgram->SetUniformSampler("colorTex", *(Singleton<scene::MaterialManager>::Instance()->GetTexture(0)), 5);
			ModelMatrix = scenegraph->DrawNode(2);
			ModelMatrix = glm::mat4(1.0f) * RotationMatrix * ModelMatrix;
			MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
			forwardProgram->SetUniform("mvp", MVPMatrix);
			//! Street
			forwardProgram->SetUniformSampler("colorTex", *(Singleton<scene::MaterialManager>::Instance()->GetTexture(1)), 5);
			ModelMatrix = scenegraph->DrawNode(1);
			ModelMatrix = glm::mat4(1.0f) * RotationMatrix * ModelMatrix;
			MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
			forwardProgram->SetUniform("mvp", MVPMatrix);

			forwardProgram->Unuse();
		}

		//! Draw AntTweakBar-GUI
		TwDraw();
		context->SwapBuffers();

		//! Check for context exiting
		if(context->IsExiting()){
			RUNNING = false;
		}
	}
}
