#include "RayTracer.h"
#include "Primatives/Triangle.h"
#include "vendor/Algorithms.h"
#include <algorithm>
#include <fstream>
#include <math.h>
#include <vector>
#include <iostream>
#include <string>

bool RayTracer::initSDL() {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	else {
		//create the window
		window = SDL_CreateWindow("SDL Version", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, scene.getWidth(), scene.getHeight(), SDL_WINDOW_SHOWN);
		if(window == NULL) {
			std::cout << "SDL Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
			return false;
		}
		else {
			surface = SDL_GetWindowSurface(window);
			pixelFormat = surface->format;
			return true;
		}
	}
}

RayTracer::RayTracer() {
	cameraPos = vec3(2.f, 0.f, 0.f);
	lookAtDir = vec3(0.f, 0.f, -1.f);
	g_fov = 40.f;
	threads.reserve(threadCount);
}

bool RayTracer::init() {
	scene.init();

	occluderBuffer = new float* [scene.getWidth()];
	for(int i = 0; i < scene.getWidth(); i++) occluderBuffer[i] = new float[scene.getHeight()];

	densityBuffer = new float* [scene.getWidth()];
	for(int i = 0; i < scene.getWidth(); i++) densityBuffer[i] = new float[scene.getHeight()];

	// NOTE moving the order of creation messes up shadows 

	scene.createSphere(vec3(0, 0, -20), vec3(1.00, 0.32, 0.36), 4);
	scene.createSphere(vec3(5, -1, -15), vec3(0.9, 0.76, 0.46), 2);
	scene.createSphere(vec3(5, 0, -25), vec3(0.65, 0.77, 0.97), 3);
	scene.createSphere(vec3(-5.5, 0, -15), vec3(0.90, 0.90, 0.90), 3);

	scene.createPlane(vec3(0, -3, 0), vec3(0, 0.7132, 1.0), vec3(0, 1, 0));

	// Create triangle with vertices, normals then colour
	scene.createTriangle(vec3(0, 3, -8), vec3(-1.9, 1, -8), vec3(1.6, 1.5, -8), 
						vec3(0, 0.6, 1.0), vec3(-0.4, -0.4, 1.0), vec3(0.4, -0.4, 1), 
						vec3(0.5, 0.5, 0.0), 32);

	scene.createMesh(vec3(0, -1, -10), vec3(1), vec3(0.5, 0.5, 0), 32, "resources/cube_simple.obj");

	//scene.createMesh(vec3(0, -1, -8), vec3(1), vec3(0.5, 0.5, 0), 100, "resources/teapot_simple.obj");

	return initSDL();
}

// used for camera movement
vec3 movementDir = vec3(0);
vec3 lightMovementDir = vec3(0);
float deltaX = 0, deltaY = 0;
bool stats = false;

void RayTracer::handleInputs() {
	SDL_Event SDLevent;
	while(SDL_PollEvent(&SDLevent) != 0) {
		int keyCode = SDLevent.key.keysym.sym;
		switch(SDLevent.type) {
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			movementDir.x = keyCode == SDLK_a ? -1 : keyCode == SDLK_d ? 1 : movementDir.x;
			movementDir.y = keyCode == SDLK_LCTRL ? -1 : keyCode == SDLK_SPACE ? 1 : movementDir.y;
			movementDir.z = keyCode == SDLK_w ? -1 : keyCode == SDLK_s ? 1 : movementDir.z;

			lightMovementDir.x = keyCode == SDLK_LEFT ? -1 : keyCode == SDLK_RIGHT ? 1 : lightMovementDir.x;
			lightMovementDir.y = keyCode == SDLK_DOWN ? -1 : keyCode == SDLK_UP ? 1 : lightMovementDir.y;
			lightMovementDir.z = keyCode == SDLK_COMMA ? -1 : keyCode == SDLK_PERIOD ? 1 : lightMovementDir.z;

			if(keyCode == SDLK_F3) stats = !stats;

			break;

		case SDL_KEYUP:
			if(keyCode == SDLK_ESCAPE) {
				running = false;
				break;
			}
			if(keyCode == SDLK_g) {
				scene.generateImage();
			}
			if(keyCode == SDLK_a || keyCode == SDLK_d) {
				movementDir.x = 0;
			}
			if(keyCode == SDLK_LCTRL || keyCode == SDLK_SPACE) {
				movementDir.y = 0;
			}
			if(keyCode == SDLK_w || keyCode == SDLK_s) {
				movementDir.z = 0;
			}

			if(keyCode == SDLK_LEFT || keyCode == SDLK_RIGHT) {
				lightMovementDir.x = 0;
			}
			if(keyCode == SDLK_DOWN || keyCode == SDLK_UP) {
				lightMovementDir.y = 0;
			}
			if(keyCode == SDLK_COMMA || keyCode == SDLK_PERIOD) {
				lightMovementDir.z = 0;
			}
			break;

		case SDL_MOUSEWHEEL:
			if(SDLevent.wheel.y > 0) {
				g_fov-= 2;
			}
			else if(SDLevent.wheel.y < 0) {
				g_fov+= 2;
			}
			break;
		default:
			break;
		}

	}
}

void RayTracer::update() {
	updateCameraPosition(deltaX, deltaY);
}

void RayTracer::render() {
	const int height = scene.getHeight();
	const int width = scene.getWidth();
	for(int i = 0; i < threadCount; i++) {
		threads.push_back(std::thread(&RayTracer::renderModels, this, std::ref(scene), i * height / threadCount, (i + 1) * height / threadCount));
	}

	for(int i = 0; i < threadCount; i++) {
		threads[i].join();
	}

	//renderSoftShadows(scene, 0, scene.getHeight());

	SDL_UpdateWindowSurface(window);

	threads.clear();

}

void RayTracer::mainLoop() {
	// variables needed to run the game application loop
	running = true;
	std::chrono::nanoseconds nsPerRenderLimit(1000000000L / targetFps); // todo check targetFps != 0

	int tps = 0, fps = 0, ticks = 0, frames = 0;

	float averageTimeToRenderFrame = 0.f;


	auto previousTime = high_res_clock::now(), currentTime = high_res_clock::now();

	//std::chrono::duration<__int64, std::nano> physicsTimer, frameTimer, timer;
	nanoSeconds loopTime(0ns), physicsTimer(0ns), frameTimer(0ns), timer(0ns), currentTimeInNano(0ns);

	frameTimer = currentTime.time_since_epoch();
	timer = currentTime.time_since_epoch();

	// Loop starts here
	while(running) {
		// How much time has passed since the last frame
		currentTime = high_res_clock::now();
		currentTimeInNano = currentTime.time_since_epoch(); // Store return value

		loopTime = currentTime - previousTime;

		previousTime = currentTime;
		// time accumulator
		physicsTimer += loopTime;

		handleInputs();
		
		// Do multiple physics ticks when behind
		while(physicsTimer >= NS_PER_TICK) {
			update();
			ticks++;
			physicsTimer -= NS_PER_TICK;
		}

		// Render once per loop at a set cap.
		// NOTE Turning on v-sync will also limit the amount of times this if statement is called
		if(currentTimeInNano - frameTimer >= nsPerRenderLimit) {
			uint64 start = SDL_GetPerformanceCounter();
			render(); // Render in between physics updates
			uint64 end = SDL_GetPerformanceCounter();

			averageTimeToRenderFrame += (end - start) / (float)SDL_GetPerformanceFrequency();

			frames++;
			frameTimer += nsPerRenderLimit;
		}

		// Do stuff every second
		if(currentTimeInNano - timer >= singleSecond) {
			tps = ticks;
			fps = frames;

			std::cout << "tps: " << tps << " fps: " << fps << std::endl;
			std::cout << "time to render = " << (averageTimeToRenderFrame * 1000) / frames << "ms" << std::endl;
			std::cout << "Total rays cast: " << raysCast << std::endl;
			std::cout << "ray-primitive functions: " << intersectFunctionsCalled << std::endl;
			std::cout << "ray-primitive hits: " << intersectHits << std::endl;

			if(stats) renderStats();

			ticks = 0;
			frames = 0;
			averageTimeToRenderFrame = 0.f;
			raysCast = 0;
			intersectFunctionsCalled = 0;
			intersectHits = 0;
			timer += singleSecond;
		}
	}
}

void RayTracer::updateCameraPosition(float deltaX, float deltaY) {
	cameraPos.x += movementDir.x * cameraSpeed * 0.0333f;
	cameraPos.y += movementDir.y * cameraSpeed * 0.0333f;
	cameraPos.z += movementDir.z * cameraSpeed * 0.0333f;

	scene.mainLight->position.x += lightMovementDir.x * cameraSpeed * 0.0333f;
	scene.mainLight->position.y += lightMovementDir.y * cameraSpeed * 0.0333f;
	scene.mainLight->position.z += lightMovementDir.z * cameraSpeed * 0.0333f;
}

glm::vec3 RayTracer::constructRayDir(Scene& scene, int x, int y) {
	float PixelNdx, PixelNdy, PixelRdx, PixelRdy, aspectRatio, tanvalue, PCameraX, PCameraY;

	aspectRatio = scene.getWidth() / scene.getHeight();
	tanvalue = tanf(g_fov * PI / 180.0);  //40 degree for big field of view //15 for zoom in

	// Calculate direction from camera to pixel
	// pixel represented as a % along the screen
	PixelNdx = (x + 0.5f) / (float)scene.getWidth();
	PixelNdy = (y + 0.5f) / (float)scene.getHeight();

	// Convert to world space between (-1, 1)
	PixelRdx = 2 * PixelNdx - 1.0;
	PixelRdy = 1.f - 2 * PixelNdy;
	// Stretch width to match aspect ratio
	PixelRdx = PixelRdx * aspectRatio;

	// Multiply by fov result
	PCameraX = PixelRdx * tanvalue;
	PCameraY = PixelRdy * tanvalue;

	vec3 ttvec = vec3(PCameraX, PCameraY, -1.f);

	//dir need to be normalized
	return normalize(ttvec);
}

Uint32 RayTracer::convertColour(vec3 colour) {
	int tt;
	Uint8 r, g, b;

	// Limit max values to 255 for red, green and blue
	tt = (int)(colour.r * 255);
	r = tt <= 255 ? tt : 255;

	tt = (int)(colour.g * 255);
	g = tt <= 255 ? tt : 255;

	tt = (int)(colour.b * 255);
	b = tt <= 255 ? tt : 255;

	// Combine data into one 32-byte type
	// NOTE my display has a BGR sub pixel layout and so
	// triangles don't look the same on other monitors
	uint32 rgb = SDL_MapRGB(pixelFormat, r, g, b);

	return rgb;
}

void RayTracer::putPixel32_nolock(int x, int y, Uint32 colour) {
	Uint8* pixel = (Uint8*)surface->pixels;

	// Move to pixel at coords x,y and set it to colour
	pixel += (y * surface->pitch) + (x * sizeof(Uint32));
	*((Uint32*)pixel) = colour;
}

int RayTracer::caclulateFpsLows() {
	static const int NUM_SAMPLES = 1000;
	static int last_1000_frames[NUM_SAMPLES];
	return 10;
}

void RayTracer::renderModels(Scene &scene, int start, int end) {
	std::vector<Model*> models = scene.getModels();
	std::vector<Light*> lights = scene.getLights();
	vec3** image = scene.getPixels();

	const int width = scene.getWidth();
	const int height = scene.getHeight();

	float t, min_t, max_t, u = 2.f, v = 2.f, ColorVal;

	// Used for iteration and array access to improve performance
	int i, whichone, furthestModel;

	vec3 dir, org, mat_color, final_Color, IntPt;

	// Initialise with set size to avoid resizing (Only effective when there are multiple objects in scene)
	const int RESERVE_SIZE = 8;

	std::vector<IntersectData> intersections(RESERVE_SIZE);
	std::vector<Model*> inView(RESERVE_SIZE);
	std::vector<IntersectData> shadows(RESERVE_SIZE);

	// TODO swap order to optimise array memory access
	for(int y = start; y < end; ++y) {
		for(int x = 0; x < width; ++x) {
			intersections.clear();
			inView.clear();
			shadows.clear();
			occluderBuffer[x][y] = 1.f;

			dir = constructRayDir(scene, x, y);

			org = cameraPos;

			auto it = models.begin();
			auto itLight = lights.begin();

			while(it != models.end()) {
				Model* ptr = *it;
				IntersectData intersect;

				// If the camera intersects with the model
				if(ptr->rayIntersect(org, dir, intersect)) {
					intersections.push_back(intersect);
					inView.push_back(ptr);
					// Ray-trace shadows
					// Iterate over each light
					while (itLight != lights.end()) {
						IntersectData shadowData;
						if (traceShadows((*itLight), intersect, shadowData, models)) {
							shadows.push_back(shadowData);
						}
						occluderBuffer[x][y] = shadowData.occluderDistance;
						densityBuffer[x][y] = shadowData.density;
						raysCast++;
						itLight++;
					}

					intersectHits++;
				}
				raysCast++;
				intersectFunctionsCalled++;
				it++;
			}

			// Nothing on screen so render a blank image
			if(intersections.size() == 0) {
				image[x][y].x = 1.0;
				image[x][y].y = 1.0;
				image[x][y].z = 1.0;

				putPixel32_nolock(x, y, convertColour(image[x][y]));
			}
			else {
				min_t = 1000.f;
				max_t = 0.f;
				whichone = 0;
				furthestModel = 0;
				// Find the model closest to the Ray origin (Camera origin)
				// TODO replace data at front of array instead of pushing on first rayIntersect
				for(i = 0; i < intersections.size(); i++) {
					if(intersections[i].t < min_t) {
						whichone = i;
						min_t = intersections[i].t;
					}
					if(intersections[i].t > max_t) {
						furthestModel = i;
						max_t = intersections[i].t;
					}
				}

				// Compute the colour at the pixel caused by all light objects
				vec3 pixelColour;
				vec3 accumulator(0.f);
				float avgShadowGradient = 1.f;
				bool renderShadow = true;
				auto it = lights.begin();

				// CASE1: all shadows are hard therefore no need to compute colour
				// CASE2: There are no shadows meaning all colour can be calculated
				// This loop mixes all of the gradients together
				for (int i = 0; i < shadows.size(); i++) {
					avgShadowGradient *= shadows[i].shadowGradient;
				}

				bool notHardShadow = avgShadowGradient > SHADOW_HARD;
				float originToShadowT = length(shadows[0].intersectPoint - org);
				bool closest = intersections[whichone].t < intersections[furthestModel].t;
				closest = intersections[whichone].t < originToShadowT;

				// There is no hard shadow on this pixel
				if (notHardShadow || closest) {
					// Only compute colour for the closest model in our view
					while(it != lights.end()) {
						inView[whichone]->computeColour(*it, dir, intersections[whichone], pixelColour);
						accumulator += pixelColour;
						it++;
					}
				}
				else {
					accumulator = vec3(SHADOW_HARD + 0.1f);
				}

				image[x][y] = accumulator;

				// Overflow the colours onto the image pixels
				putPixel32_nolock(x, y, convertColour(image[x][y]));
			}
		}
	}

}

bool RayTracer::traceShadows(const Light* light, const IntersectData& originData, IntersectData& shadowData, const std::vector<Model*> models)
{
	vec3 shadowDir = normalize(light->position - originData.intersectPoint);
	vec3 intersectPtBias = originData.intersectPoint + originData.normal * SHADOW_BIAS;
	float lightOcclusion = length(intersectPtBias - light->position);

	for (int i = 0; i < models.size(); i++) {
		// If the shadow ray intersected with another object
		if (models[i]->rayIntersect(intersectPtBias, shadowDir, shadowData)) {
			shadowData.shadowGradient = SHADOW_HARD;
			shadowData.occluderDistance = shadowData.t / lightOcclusion;
			return true;
		}
	}

	// No occlusion
	shadowData.shadowGradient = SHADOW_NONE;
	shadowData.occluderDistance = 1.f;
	return false;
}

void RayTracer::renderSoftShadows(Scene& scene, int start, int end) {
	float occluderDist;

	for(int y = start; y < end; ++y) {
		for(int x = 0; x < scene.getWidth(); ++x) {
			occluderDist = occluderBuffer[x][y];
			// Find closest
			if(occluderDist == 1.f) {
				occluderDist = searchCrossPattern(occluderBuffer, x, y, scene.getWidth(), scene.getHeight());
				occluderBuffer[x][y] = std::min(occluderDist / 0.01f, 1.f);
			}
			else {
				occluderDist = occluderDist;
			}

			uint32 colour = convertColour(scene.getPixels()[x][y] *= occluderBuffer[x][y]);
			
			putPixel32_nolock(x, y, colour);


			// Calculate penumbra size
			vec3 lightToPlane = scene.getModels()[0]->getCentre() - scene.mainLight->position;
			float lightDistance = length(lightToPlane);

			float prenumbraSize = (scene.mainLight->radius * occluderDist) / lightDistance;
		}
	}

}

void RayTracer::renderStats() {
	//this opens a font style and sets a size
	//TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 24);

	// this is the color in rgb format,
	// maxing out all would give you the color white,
	// and it will be your text's color
	//SDL_Color White = {255, 255, 255};

	// as TTF_RenderText_Solid could only be used on
	// SDL_Surface then you have to create the surface first
	//SDL_Surface* surfaceMessage =
		//TTF_RenderText_Solid(Sans, "put your text here", White);

	// now you can convert it into a texture
	//SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	//SDL_Rect Message_rect;
	//Message_rect.x = 0;
	//Message_rect.y = 0;
	//Message_rect.w = 100;
	//Message_rect.h = 100;

	//SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
}

float RayTracer::searchCrossPattern(float** buffer, int x, int y, int limitX, int limitY) {
	const int pixelSample = 16;
	// Limit values of x and y so that the buffer doesn't overflow
	int x_max = std::min(x + pixelSample, limitX);
	int x_min = std::max(0, x - pixelSample);
	int y_max = std::min(y + pixelSample, limitY);
	int y_min = std::max(0, y - pixelSample);

	float highestVal = 0.f;

	for(int i = x_min; i < x_max; i++) {
		highestVal = buffer[i][x] > highestVal ? buffer[i][x] : highestVal;
	}

	for(int j = y_min; j < y_max; j++) {
		highestVal = buffer[y][j] > highestVal ? buffer[y][j] : highestVal;
	}

	return highestVal;
}

void RayTracer::shutDown() {
	scene.deleteAllObjects();
	scene.deleteResources();

	delete(occluderBuffer);
	delete(densityBuffer);

	SDL_DestroyWindow(window);
	SDL_Quit();
}
