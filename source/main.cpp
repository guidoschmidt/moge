/*! Main Application
 * @author Guido Schmidt
 * @date 06.12.12
 */

#include "engine/Renderer.h"

Renderer* renderer;

int main(int argc, char* argv[])
{

	//! Create a renderer instance
	int width, height;
	if(*argv[0] != 0 && *argv[1] != 0)
	{
		width = 800;
		height = 800;
	}
	else
	{
		width = 800;
		height = 800;
	}


	renderer = new Renderer(width, height);
	//! Start the render loop
	renderer->RenderLoop();

	return 0;
}
