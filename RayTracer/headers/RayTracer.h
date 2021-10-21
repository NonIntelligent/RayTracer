#pragma once
#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include <thread>
#include <atomic>
#include "macros/Macro_chrono.h"
#include "macros/Definitions.h"
#include "Primatives/Sphere.h"
#include "Scene/Scene.h"

const int CORE_COUNT = std::thread::hardware_concurrency();

using namespace glm;

class RayTracer {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Surface* surface = nullptr;
	SDL_PixelFormat* pixelFormat = nullptr;

	// update rate of 30hz
	const nanoSeconds NS_PER_TICK = nanoSeconds(32222us);
	long targetFps = 8000;
	volatile bool running = false;
	const int threadCount = CORE_COUNT == 0 ? 12 : CORE_COUNT;
	float frameSamples[60];
	int frameSampleIndex = 0;
	std::vector<std::thread> threads;
	std::atomic<int> raysCast = 0;
	std::atomic<int> intersectFunctionsCalled = 0;
	std::atomic<int> intersectHits = 0;

	// Temporary objects
	Scene scene;

	// Buffers for shadow calculations
	float** occluderBuffer = nullptr;
	float** densityBuffer = nullptr;

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
	void render();

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
	bool traceShadows(const Light* light, const IntersectData& originData, IntersectData& shadowData, const std::vector<Model*> models);
	void renderSoftShadows(Scene& scene, int start, int end);
	
	void traceReflections();

	void renderStats();

	float searchCrossPattern(float** buffer, int x, int y, int limitX, int limitY);

	void shutDown();
};