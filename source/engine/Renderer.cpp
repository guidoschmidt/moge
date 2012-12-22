/*
 * Renderer.cpp
 *
 *  Created on: 28.11.2012
 *      Author: guidoschmidt
 */

#include "Renderer.h"


//! ANTTWEAKBAR CALLBACKS BEGIN
/*! Variables and button callbacks */
bool deferred;
bool rotation;
static void TW_CALL SwitchDeffered(void* clientData){ deferred = !deferred;}
static void TW_CALL SwitchRotation(void* clientData){ rotation = !rotation;}
//! ANTTWEAKBAR CALLBACKS END


//! Constructor
Renderer::Renderer(int width, int height)
{
	deferred = true;
	context = new Context();
	scene = new Scene();
	fsq = new FSQ();
	Shininess = 5.0f;
	Initialize(width, height);
}

//! Destructor
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
void Renderer::Initialize(int width, int height){
	context->OpenWindow(width, height, "Render Window", 3, 3);
	context->AddAntTweakBar();

	WriteLog(CONSOLE);
	InitGLEW();

	glEnable(GL_DEPTH_TEST);

	//! AntTweakBar
	TwAddButton(context->GetBar(), "toggledeferred", SwitchDeffered, NULL, "key='space' label='Toggle Deferred Rendering' group='Rendering'");
	TwAddButton(context->GetBar(), "togglerotation", SwitchRotation, NULL, "label='Toggle Rotation' group='Rotation'");
	TwEnumVal texEV[NUM_TEXS] = { {TEX_COMPOSIT, "Composited"}, {TEX_POSITION, "Positionmap"} ,{TEX_COLOR, "Colormap"}, {TEX_NORMAL, "Normalmap"}, {TEX_DEPTH, "Depthmap"}};
	TwType texType = TwDefineEnum("TextureType", texEV, NUM_TEXS);
	TwAddVarRW(context->GetBar(), "deferredTextureChoice", texType, &currentDeferredTex, "label='Rendering' group='Rendering' keyIncr='<' keyDecr='>' help='View the maps rendered in first pass.' ");
	TwAddVarRW(context->GetBar(), "shininess", TW_TYPE_FLOAT, &Shininess, "step='0.01' max='100.0' min='0.0' label='Shininess' group='Material'");
	TwAddVarRW(context->GetBar(), "rotationSpeed", TW_TYPE_FLOAT, &rotSpeed, "step='0.001' max='1.0' min='0.0' label='Rotationspeed' group='Rotation'");

	scene->Import3DModel("./assets/geometry/blend/RainScene.blend");
	scene->LoadTexture("./assets/texture/jpg/Road.jpg");

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
	CameraPosition = glm::vec3(0.0, 3.0, 10.0);
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
void Renderer::InitializeLight(void){
	LightPosition = glm::vec4(0.0f, 5.0f, 2.0f, 0.0f);
	LightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
	LightDiffuse = glm::vec3(0.65f, 0.65f, 0.65f);
	LightSpecular = glm::vec3(0.35f, 0.35f, 0.35f);

	TwAddVarRW(context->GetBar(), "lightAmbient", TW_TYPE_COLOR3F, &LightAmbient, "label='Ambient' group='Light'");
	TwAddVarRW(context->GetBar(), "lightDiffuse", TW_TYPE_COLOR3F, &LightDiffuse, "label='Diffuse' group='Light'");
	TwAddVarRW(context->GetBar(), "lightSpecular", TW_TYPE_COLOR3F, &LightSpecular, "label='Specular' group='Light'");
}

//! Writes the log
/*!
 * Wrtites a render log including the used OpenGL renderer (GPU) and it's vendor, OpenGL Version,
 * GLSL Version. The log can be written to console or to a log file
 */
void Renderer::WriteLog(log logLocation){
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
	if(glfwGetKey('W')){
		CameraPosition[2] -= 0.001;
		CameraTargetPosition[2] -= 0.001;
	}
	else if(glfwGetKey('S')){
		CameraPosition[2] += 0.001;
		CameraTargetPosition[2] += 0.001;
	}
	else if(glfwGetKey('A')){
		CameraPosition[0] -= 0.001;
		CameraTargetPosition[0] -= 0.001;
	}
	else if(glfwGetKey('D')){
		CameraPosition[0] += 0.001;
		CameraTargetPosition[0] += 0.001;
	}
}

//! Render loop
/*!
 * While variabke RUNNING is true, the renderer loops thorugh this function.
 */
void Renderer::RenderLoop(void){
	glClearColor(0.75f, 0.75f, 0.75f, 1.0f);

	while(RUNNING){
		//! Calculations
		CalculateFPS(0.5, false);
		KeyboardFunction();
		if(rotation)
			angle += rotSpeed;
		//! Modelmatrix
		glm::vec3 RotationAxis(0, 1, 0);
		glm::mat4 RotationMatrix = glm::rotate(angle, RotationAxis);
		ModelMatrix = IdentityMatrix * RotationMatrix;
		//! Accumulate matrices
		ViewMatrix = glm::lookAt(CameraPosition, CameraTargetPosition, CameraUp);
		MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;


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
			deferredProgram_Pass1->SetUniform("mvp", MVPMatrix);
			deferredProgram_Pass1->SetUniform("Light.Position", LightPosition);
			deferredProgram_Pass1->SetUniform("Light.Ambient", LightAmbient);
			deferredProgram_Pass1->SetUniform("Light.Diffuse", LightDiffuse);
			deferredProgram_Pass1->SetUniform("Light.Specular", LightSpecular);
			deferredProgram_Pass1->SetUniformSampler("colorTex", scene->GetTexture(0), 0);

			//! drawing
			scene->Draw();




			/*!* * * * * * * * * * * * * * *
			 *		DEFERRED RENDERING	   *
			 *		2ND RENDER PASS		   *
			 * * * * * * * * * * * * * * * */
			firstPassFBO->Unuse();
			deferredProgram_Pass1->Unuse();

			deferredProgram_Pass2->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			deferredProgram_Pass2->SetUniform("Light.Position", LightPosition);
			deferredProgram_Pass2->SetUniform("Light.Ambient", LightAmbient);
			deferredProgram_Pass2->SetUniform("Light.Diffuse", LightDiffuse);
			deferredProgram_Pass2->SetUniform("Light.Specular", LightSpecular);
			deferredProgram_Pass2->SetUniform("Shininess", Shininess);

			deferredProgram_Pass2->SetUniform("textureID", currentDeferredTex);

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

			forwardProgram->SetUniform("mvp", MVPMatrix);
			forwardProgram->SetUniform("Light.Position", LightPosition);
			forwardProgram->SetUniform("Light.Ambient", LightAmbient);
			forwardProgram->SetUniform("Light.Diffuse", LightDiffuse);
			forwardProgram->SetUniform("Light.Specular", LightSpecular);
			forwardProgram->SetUniformSampler("colorTex", scene->GetTexture(0), 0);

			scene->Draw();

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
