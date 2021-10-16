#include "RayTracer.h"
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

bool RayTracer::init() {
	scene.init();

	scene.createSphere(vec3(0, 0, -20), vec3(1.00, 0.32, 0.36), 4);
	scene.createSphere(vec3(5, -1, -15), vec3(0.9, 0.76, 0.46), 2);
	scene.createSphere(vec3(5, 0, -25), vec3(0.65, 0.77, 0.97), 3);
	scene.createSphere(vec3(-5.5, 0, -15), vec3(0.90, 0.90, 0.90), 3);

	return initSDL();
}

void RayTracer::handleInputs() {
	SDL_Event SDLevent;
	while(SDL_PollEvent(&SDLevent) != 0) {
		switch(SDLevent.type) {
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYUP:
			if(SDLevent.key.keysym.sym == SDLK_SPACE) {
				scene.generateImage();
			}
			break;

		default:
			break;
		}

	}
}

void RayTracer::update() {

}

void RayTracer::render(double dt) {
	renderModels(scene);

	SDL_UpdateWindowSurface(window);
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

void RayTracer::ComputeColourSphere(const vec3 sourcePt, const vec3 IntPt, const vec3 CenPt, const vec3 dir, float& ColValue) {
	vec3 lightToPt, surNorm, rVec, ttvec;
	float Ca, Cd, Cs, tt; //Ca for ambient colour; //Cd for diffuse colour; //Cs for specular highlights
	float vecdot;
	lightToPt = normalize(sourcePt - IntPt);
	surNorm = normalize(IntPt - CenPt);
	Cd = std::max(0.0, (double)dot(lightToPt, surNorm));
	Ca = 0.2;

	//compute specular value
	vecdot = dot(surNorm, lightToPt);
	ttvec.x = surNorm.x * 2.0 * vecdot;
	ttvec.y = surNorm.y * 2.0 * vecdot;
	ttvec.z = surNorm.z * 2.0 * vecdot;

	rVec = ttvec - lightToPt;
	tt = std::max(0.0, (double)dot(rVec, -dir));
	Cs = pow(tt, 20) * 0.7;

	//ColValue = Cs;
	ColValue = Ca + Cd + Cs;
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

void RayTracer::renderModels(Scene &scene) {
	std::vector<Model*> models = scene.getModels();
	std::vector<Light*> lights = scene.getLights();
	vec3** image = scene.getPixels();

	const int width = scene.getWidth();
	const int height = scene.getHeight();

	float PixelNdx, PixelNdy, PixelRdx, PixelRdy, aspectRatio, tanvalue, PCameraX, PCameraY;

	aspectRatio = width / height;
	tanvalue = tanf(40.0 * PI / 180.0);  //40 degree for big field of view //15 for zoom in

	bool Intersection;
	float t, min_t, ColorVal;

	// Used for iteration and array access to improve performance
	int i, whichone;

	vec3 ttvec, dir, org, mat_color, final_Color, IntPt;
	// Initialise with set size to avoid resizing (Only effective when there are multiple objects in scene)
	std::vector<float> t_arr(8);
	std::vector<Model*> inView(8);


	///light setting
	vec3 sourcePt;
	sourcePt.x = 3.0; sourcePt.y = 5.0; sourcePt.z = -5.0;

	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			t_arr.clear();
			inView.clear();

			// Calculate direction from camera to pixel
			PixelNdx = (x + 0.5) / (float)width;
			PixelNdy = (y + 0.5) / (float)height;
			PixelRdx = 2 * PixelNdx - 1.0;
			PixelRdy = 1.0 - 2 * PixelNdy;
			PixelRdx = PixelRdx * aspectRatio;

			PCameraX = PixelRdx * tanvalue;
			PCameraY = PixelRdy * tanvalue;

			ttvec.x = PCameraX;
			ttvec.y = PCameraY;
			ttvec.z = -1.0;

			//dir need to be normalized
			dir = normalize(ttvec);

			// Camera origin
			org = vec3(0.f);

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
					inView[whichone]->computeColour(*it, (*it)->direction, intersectPt, pixelColour);
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
