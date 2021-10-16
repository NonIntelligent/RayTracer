#pragma once
#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include "macros/Macro_chrono.h"
#include "macros/Definitions.h"
#include "Primatives/Sphere.h"
#include "Scene/Scene.h"

using namespace glm;

class RayTracer {
	SDL_Window* window = nullptr;
	SDL_Surface* surface = nullptr;

	// update rate of 60hz
	const nanoSeconds NS_PER_TICK = nanoSeconds(16667us);
	long targetFps = 8000;
	volatile bool running = false;

	// Temporary objects
	Scene scene;

	bool initSDL();

public:
	RayTracer() { };
	~RayTracer() { };
	bool init();

	void handleInputs();
	void update();
	void render(double dt);

	void mainLoop();

	Uint32 convertColour(vec3 colour);

	void ComputeColourSphere(const vec3 sourcePt, const vec3 IntPt, const vec3 CenPt, const vec3 dir, float& ColValue);

	// Sets the colour on the screenSurface
	void putPixel32_nolock(int x, int y, Uint32 colour);

	// FPS calculation guide
	// https://www.youtube.com/watch?v=4cwpXJIHaMo
	int caclulateFpsLows();

	void renderModels(Scene &scene);

	void shutDown();
};