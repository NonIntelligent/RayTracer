#include "RayTracer.h"
#include "Primatives/Triangle.h"
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
			return true;
		}
	}
}

RayTracer::RayTracer() {
	cameraPos = vec3(0.f, 0.f, 0.f);
	lookAtDir = vec3(0.f, 0.f, -1.f);
	g_fov = 40.f;
	threads.reserve(threadCount);
}

bool RayTracer::init() {
	scene.init();

	scene.createSphere(vec3(0, 0, -20), vec3(1.00, 0.32, 0.36), 4);
	scene.createSphere(vec3(5, -1, -15), vec3(0.9, 0.76, 0.46), 2);
	scene.createSphere(vec3(5, 0, -25), vec3(0.65, 0.77, 0.97), 3);
	scene.createSphere(vec3(-5.5, 0, -15), vec3(0.90, 0.90, 0.90), 3);

	scene.createPlane(vec3(0, -3, 0), vec3(0, 0.7132, 1.0), vec3(0, 1, 0));

	// Create triangle with vertices, normals then colour
	scene.createTriangle(vec3(0, 3, -8), vec3(-1.9, 1, -8), vec3(1.6, 1.5, -8), 
						vec3(0, 0.6, 1.0), vec3(-0.4, -0.4, 1.0), vec3(0.4, -0.4, 1), 
						vec3(0.5, 0.5, 0.0), 100);

	scene.createMesh(vec3(-2, -1, -13), vec3(1), vec3(0.5, 0.5, 0), 64, "resources/cube_simple.obj");

	//scene.createMesh(vec3(0, -1, -8), vec3(1), vec3(0.5, 0.5, 0), 100, "resources/teapot_simple.obj");

	return initSDL();
}

// used for camera movement
vec3 movementDir = vec3(0);
float deltaX = 0, deltaY = 0;

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

void RayTracer::render(double dt) {
	const int height = scene.getHeight();
	const int width = scene.getWidth();
	for(int i = 0; i < threadCount; i++) {
		threads.push_back(std::thread(&RayTracer::renderModels, this, std::ref(scene), i * height / threadCount, (i + 1) * height / threadCount));
	}

	SDL_UpdateWindowSurface(window);

	for(int i = 0; i < threadCount; i++) {
		threads[i].join();
	}

	threads.clear();

}

void RayTracer::mainLoop() {
	// variables needed to run the game application loop
	running = true;
	std::chrono::nanoseconds nsPerRenderLimit(1000000000L / targetFps); // todo check targetFps != 0

	int tps = 0;
	int fps = 0;
	int ticks = 0;
	int frames = 0;

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
			render(physicsTimer / nsPerRenderLimit); // Render in between physics updates
			frames++;
			frameTimer += nsPerRenderLimit;
		}

		// Do stuff every second
		if(currentTimeInNano - timer >= singleSecond) {
			tps = ticks;
			fps = frames;

			std::cout << "tps: " << tps << " fps: " << fps << std::endl;

			ticks = 0;
			frames = 0;
			timer += singleSecond;
		}
	}
}

void RayTracer::updateCameraPosition(float deltaX, float deltaY) {
	cameraPos.x += movementDir.x * cameraSpeed * 0.0167f;
	cameraPos.y += movementDir.y * cameraSpeed * 0.0167f;
	cameraPos.z += movementDir.z * cameraSpeed * 0.0167f;
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
	Uint32 rgb;

	//check which order SDL is storing bytes
	rgb = (r << 16) + (g << 8) + b;

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

	bool Intersection;
	float t, min_t, u = 2.f, v = 2.f, ColorVal;

	// Used for iteration and array access to improve performance
	int i, whichone;

	vec3 dir, org, mat_color, final_Color, IntPt;
	// Initialise with set size to avoid resizing (Only effective when there are multiple objects in scene)
	const int RESERVE_SIZE = 8;

	std::vector<float> t_arr(RESERVE_SIZE);
	std::vector<Model*> inView(RESERVE_SIZE);

	// TODO swap order to optimise array memory access
	for(int y = start; y < end; ++y) {
		for(int x = 0; x < width; ++x) {
			t_arr.clear();
			inView.clear();

			dir = constructRayDir(scene, x, y);

			org = cameraPos;

			vec3 intersectPt, normalVec;
			vec3 lightNorm;

			auto it = models.begin();

			while(it != models.end()) {
				Model* ptr = *it;

				// If the camera intersects with the model
				if(ptr->rayIntersect(org, dir, t)) {
					t_arr.push_back(t);
					inView.push_back(ptr);
				}

				it++;
			}

			// Nothing on screen so render a blank image
			if(t_arr.size() == 0) {
				image[x][y].x = 1.0;
				image[x][y].y = 1.0;
				image[x][y].z = 1.0;

				putPixel32_nolock(x, y, convertColour(image[x][y]));
			}
			else {
				min_t = 1000.0;
				whichone = 0;
				// Find the model closest to the Ray origin (Camera origin)
				for(i = 0; i < t_arr.size(); i++) {
					if(t_arr[i] < min_t) {
						whichone = i; min_t = t_arr[i];
					}
				}

				// Get the intersection point and surface normal of the model
				inView[whichone]->getSurfaceData(org, dir, t_arr[whichone], intersectPt, normalVec);

				// Compute the colour at the pixel caused by all light objects
				vec3 pixelColour;
				vec3 accumulator(0);
				auto it = lights.begin();

				while(it != lights.end()) {
					inView[whichone]->computeColour(*it, dir, intersectPt, pixelColour);
					accumulator += pixelColour;
					it++;
				}

				image[x][y] = accumulator;

				// Overflow the colours onto the image pixels
				putPixel32_nolock(x, y, convertColour(image[x][y]));
			}
		}
	}
}

void RayTracer::shutDown() {
	scene.deleteAllObjects();
	scene.deleteResources();
	SDL_DestroyWindow(window);
	SDL_Quit();
}
