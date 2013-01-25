/*! Main Application
 * @author Guido Schmidt
 * @date 06.12.12
 */

#include "engine/Renderer.h"

Renderer* renderer;

int main(int argc, char* argv[])
{
	//! Create a renderer instance
	renderer = new Renderer(600, 880);
	//! Start the render loop
	renderer->RenderLoop();

	return 0;
}
