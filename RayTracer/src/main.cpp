#include "RayTracer.h"
#include <memory>

int main(int argc, char* args[]) {
	std::unique_ptr<RayTracer> rayTracer = std::make_unique<RayTracer>();

	if(!rayTracer->init()) return -1;

	rayTracer->mainLoop();

	rayTracer->shutDown();

	return 0;
}
