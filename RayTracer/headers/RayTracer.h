#pragma once
#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include <thread>
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
	const int threadCount = 32;
	std::vector<std::thread> threads;

	// Temporary objects
	Scene scene;

	// Camera options
	vec3 cameraPos;
	vec3 lookAtDir;
	float horizontalAngle, verticalAngle, g_fov;
	const float cameraSpeed = 2.f;

	bool initSDL();

public:
	RayTracer();
	~RayTracer() { };
	bool init();

	void handleInputs();
	void update();
	void render(double dt);

	void mainLoop();

	void updateCameraPosition(float deltaX, float deltaY);

	vec3 constructRayDir(Scene& scene, int x, int y);

	Uint32 convertColour(vec3 colour);

	// Sets the colour on the screenSurface
	void putPixel32_nolock(int x, int y, Uint32 colour);

	// FPS calculation guide
	// https://www.youtube.com/watch?v=4cwpXJIHaMo
	int caclulateFpsLows();

	void renderModels(Scene &scene, int start, int end);

	void shutDown();
};