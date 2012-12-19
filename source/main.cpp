/*! Main Application
 * @author Guido Schmidt
 * @date 06.12.12
 */
#include "engine/Renderer.h"

Renderer* renderer;

int main(int argc, char* argv[]){
	renderer = new Renderer(1200, 800);

	renderer->RenderLoop();

	return 0;
}

