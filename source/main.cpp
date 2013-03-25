/*! Main Application
 * @author Guido Schmidt
 * @date 06.12.12
 */

#include "engine/Renderer.h"
#include <iostream>

Renderer* renderer;

int main(int argc, char* argv[])
{

	//! Create a renderer instance
	int width, height;
		width = 800;
		height = 800;
	int scene;

	 std::cout << "Please enter an width value: ";
	 std::cin >> width;

	 std::cout << "Please enter an height value: ";
	 std::cin >> height;

	 std::cout << "Please enter a valid scene number: "<< std::endl <<"0: TESTSCENE\n1: MUSEUM\n2: CHURCH" << std::endl;
	 std::cin >> scene;

	renderer = new Renderer(width, height, scene);
	//! Start the render loop
	renderer->RenderLoop();

	return 0;
}
