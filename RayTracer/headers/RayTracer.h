#pragma once
#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include "macros/Macro_chrono.h"
#include "macros/Definitions.h"
#include "Primatives/Sphere.h"

using namespace glm;

class RayTracer {
	int width = 640;
	int height = 480;

	SDL_Window* window = nullptr;
	SDL_Surface* surface = nullptr;
	SDL_Event SDLevent;

	// update rate of 60hz
	const nanoSeconds NS_PER_TICK = nanoSeconds(16667us);
	long targetFps = 8000;
	volatile bool running = false;

	// Temporary objects
	vec3** image;
	Sphere redsphere;
	Sphere yellowsphere;
	Sphere bluesphere;
	Sphere greysphere;

	bool initSDL();

public:
	RayTracer() { };
	~RayTracer() { };
	bool init();

	void handleInputs();
	void update();
	void render(double dt);

	void mainLoop();

	bool intersectSphere(vec3 center, vec3 orig, vec3 dir, float radius, float& t);

	Uint32 convertColour(vec3 colour);

	void computeColourSphere(const vec3 sourcePt, const vec3 IntPt, const vec3 CenPt, const vec3 dir, float& ColValue);

	// Applies colour to pixel on screen
	void putPixel32_nolock(int x, int y, Uint32 colour);

	void shutDown();
};