//! Context.cpp
/*!
 * @date 	30.11.2012
 * @author	Guido Schmidt
 */

#include "Context.h"

int initWIDTH, initHEIGHT;
int WIDTH, HEIGHT;

//! Constructor
Context::Context(int height, int width)
{
	bar = 0;
	WIDTH = width;
	HEIGHT = height;
	if(!glfwInit()){
		std::cout << "ERROR: could not initialize GLFW!" << std::endl;
		glfwTerminate();
	}
}

//! Destructor
Context::~Context()
{
	glfwTerminate();
}

//! GLFW resize function
/*!
 * Defines the GLFW resize function
 */
void GLFWCALL ResizeFunction(int width, int height)
{
	WIDTH = width;
	HEIGHT = height;
	glViewport(0, 0, WIDTH, HEIGHT);
	TwWindowSize(WIDTH, HEIGHT);
}

//! Open window function
/*!
 * Opens a GLFW window.
 *
 * @param width
 * @param height
 * @param title
 * @param openglVersionMajor
 * @param openglVersionMinor
 */
void Context::OpenWindow(int width, int height, std::string title, int openglVersionMajor, int openglVersionMinor){
	WIDTH = width;
	HEIGHT = height;
	TITLE = title;

	//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, openglVersionMajor);
	//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, openglVersionMinor);

	if(!glfwOpenWindow(WIDTH, HEIGHT, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)){
		std::cout << "ERROR: could not open a GLFW window!" << std::endl;
		glfwTerminate();
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	glfwSetWindowPos(0, 0 );
	glfwSetWindowTitle(TITLE.c_str());
	glfwSetWindowSizeCallback(ResizeFunction);
}

//! Sets window title
/*!
 * Sets the GLFW window title.
 *
 * @param title
 */
void Context::setTitle(std::string title){
	TITLE = title;
}

//! Sets the window size
/*!
 * Sets the GLFW window size.
 *
 * @param width
 * @param height
 */
void Context::setSize(int width, int height){
	WIDTH = width;
	HEIGHT = height;
}


//! Swaps the context's front and back buffer
/*!
 * Calls GLFW's swap buffers function.
 */
void Context::SwapBuffers(void){
	glfwSwapBuffers();
}

//! Checks the context's existance
/*!
 *
 * @return the context's existance
 */
bool Context::IsExiting(void){
	if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
		return true;
	else
		return false;
}

//! Returns the window title
/*!
 *
 * @return context's window title
 */
std::string* Context::GetTitle(void)
{
	return &TITLE;
}

//! Returns the AntTweakBar bar used in this context
/*!
 *
 * @return context's TwBar
 */
TwBar* Context::GetBar(){
	return bar;
}

//! Returns the context's width
/*!
 *
 * @return context's Width
 */
int Context::GetWidth()
{
	return WIDTH;
}

//! Returns the context's height
/*!
 *
 * @return context's height
 */
int Context::GetHeight()
{
	return HEIGHT;
}

//! Adds a graphical user interface (GUI) to this context
/*!
 * Adds a GUI to this context, using a TwBar from AntTweakBar.
 */
void Context::AddAntTweakBar(void){
	TwWindowSize(WIDTH, HEIGHT);
	TwInit(TW_OPENGL, 0);

	bar = TwNewBar("GUI");

	TwDefine("GUI label='Engine Properties' contained='true' fontstyle='fixed' color='0 0 0' alpha='100' text='light' position='10 10' size='400 600'");

	//! Set GLFW event callbacks
	//! Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetMouseButtonCallback((GLFWmousebuttonfun)TwEventMouseButtonGLFW);
	//! Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetMousePosCallback((GLFWmouseposfun)TwEventMousePosGLFW);
	//! Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetMouseWheelCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);
	//! Directly redirect GLFW key events to AntTweakBar
	glfwSetKeyCallback((GLFWkeyfun)TwEventKeyGLFW);
	//! Directly redirect GLFW char events to AntTweakBar
	glfwSetCharCallback((GLFWcharfun)TwEventCharGLFW);
}
