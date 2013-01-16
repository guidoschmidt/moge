//! Context.cpp
/*!
 * @date 	30.11.2012
 * @author	Guido Schmidt
 */

#include "Context.h"

//! GLFW callback functions
/*********************************************************************/
//! GLFW keyboard function
/*!
 *
 */
void GLFWCALL KeyboardFunction(int key, int action)
{
	if(!(TwEventKeyGLFW(key, action)))
	{
		switch(key){
			default:
				//std::cout << "key: " << key << ", action: " << action << std::endl;
				break;
			}
	}
}

//! GLFW resize function
/*!
 * Defines the GLFW resize function
 */
void GLFWCALL ResizeFunction(int width, int height)
{
	Singleton<Context>::Instance()->SetSize(width, height);
	glViewport(0, 0, width, height);
	TwWindowSize(width, height);
}


//! GLFW mouse function
/*!
 *
 */
void GLFWCALL MousePositionFunction(int mouseX, int mouseY)
{
	if(!TwEventMousePosGLFW(mouseX, mouseY))
	{
		//std::cout << "Mouse @ (" << x << ", " << y << ")" << std::endl;
	}
}
/*********************************************************************/


//! Constructor
Context::Context(int height, int width)
{
	bar_ptr = 0;
	m_width = width;
	m_height = height;
	if(!glfwInit()){
		std::cout << "ERROR: could not initialize GLFW!" << std::endl;
		glfwTerminate();
	}
}


//! Destructor
/*!
 *
 */
Context::~Context()
{
	glfwTerminate();
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
	m_width = width;
	m_height = height;
	m_title = title;

	//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, openglVersionMajor);
	//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, openglVersionMinor);

	if(!glfwOpenWindow(m_width, m_height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)){
		std::cout << "ERROR: could not open a GLFW window!" << std::endl;
		glfwTerminate();
	}

	glViewport(0, 0, m_width, m_height);

	glfwSetWindowPos(0, 0 );
	glfwSetWindowTitle(m_title.c_str());
	glfwSetWindowSizeCallback(ResizeFunction);
	glfwSetKeyCallback(KeyboardFunction);
	glfwSetMousePosCallback(MousePositionFunction);
}


//! Sets window title
/*!
 * Sets the GLFW window title.
 *
 * @param title
 */
void Context::SetTitle(std::string title){
	m_title = title;
}


//! Sets the window size
/*!
 * Sets the GLFW window size.
 *
 * @param width
 * @param height
 */
void Context::SetSize(int width, int height)
{
	m_width = width;
	m_height = height;
}


//! Swaps the context's front and back buffer
/*!
 * Calls GLFW's swap buffers function.
 */
void Context::SwapBuffers(void)
{
	glfwSwapBuffers();
}


//! Checks the context's existance
/*!
 *
 * @return the context's existance
 */
bool Context::IsExiting(void)
{
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
	return &m_title;
}

//! Returns the AntTweakBar bar used in this context
/*!
 *
 * @return context's TwBar
 */
TwBar* Context::GetBar()
{
	return bar_ptr;
}


//! Returns the context's width
/*!
 *
 * @return context's Width
 */
int Context::GetWidth()
{
	return m_width;
}


//! Returns the context's height
/*!
 *
 * @return context's height
 */
int Context::GetHeight()
{
	return m_height;
}


//! Adds a graphical user interface (GUI) to this context
/*!
 * Adds a GUI to this context, using a TwBar from AntTweakBar.
 */
void Context::AddAntTweakBar(void)
{
	TwWindowSize(m_width, m_height);
	TwInit(TW_OPENGL, 0);

	bar_ptr = TwNewBar("GUI");

	TwDefine("GUI label='Engine Properties' contained='true' fontstyle='fixed' color='0 0 0' alpha='100' text='light' position='0 0' size='250 600'");

	//! Set GLFW event callbacks
	//! Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetMouseButtonCallback((GLFWmousebuttonfun)TwEventMouseButtonGLFW);
	//! Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetMouseWheelCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);
	glfwSetCharCallback((GLFWcharfun)TwEventCharGLFW);
}
