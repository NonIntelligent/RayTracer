#include "RayTracer.h"
#include <algorithm>
#include <fstream>
#include <math.h>
#include <vector>
#include <iostream>

bool RayTracer::initSDL() {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	else {
		//create the window
		window = SDL_CreateWindow("SDL Version", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
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
	return initSDL();
}

void RayTracer::handleInputs() {
	while(SDL_PollEvent(&SDLevent) != 0) {
		if(SDLevent.type == SDL_QUIT) {
			running = false;
		}
	}
}

void RayTracer::update() {
	redsphere = Sphere(4, vec3(0, 0, -20), vec3(1.00, 0.32, 0.36));
	yellowsphere = Sphere(2, vec3(5, -1, -15), vec3(0.9, 0.76, 0.46));
	bluesphere = Sphere(3, vec3(5, 0, -25), vec3(0.65, 0.77, 0.97));
	greysphere = Sphere(3, vec3(-5.5, 0, -15), vec3(0.90, 0.90, 0.90));
	float PixelNdx, PixelNdy, PixelRdx, PixelRdy, Iar, tanvalue, PCameraX, PCameraY;

	//create two dimensional pixel array for the image
	image = new vec3 * [width];
	for(int i = 0; i < width; i++) image[i] = new vec3[height];

	Iar = width / (float)height;
	tanvalue = tanf(15.0 * PI / 180.0);  //40 degree for big field of view //15 for zoom in

	bool Intersection;
	float t, min_t, ColorVal;
	int i, whichone;
	vec3 ttvec, dir, org, mat_color, final_Color, IntPt;
	std::vector<float> t_arr;
	std::vector<vec3> color_arr;


	///light setting
	vec3 sourcePt;
	sourcePt.x = 0.0; sourcePt.y = 20.0; sourcePt.z = 0.0;

	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			t_arr.clear();
			color_arr.clear();

			PixelNdx = (x + 0.5) / (float)width;
			PixelNdy = (y + 0.5) / (float)height;
			PixelRdx = 2 * PixelNdx - 1.0;
			PixelRdy = 1.0 - 2 * PixelNdy;
			PixelRdx = PixelRdx * Iar;

			PCameraX = PixelRdx * tanvalue;
			PCameraY = PixelRdy * tanvalue;

			ttvec.x = PCameraX;
			ttvec.y = PCameraY;
			ttvec.z = -1.0;
			//dir need to be normalized
			dir = normalize(ttvec);

			org.x = 0.0; org.y = 0.0; org.z = 0.0;

			Intersection = intersectSphere(redsphere.getCenter(), org, dir, redsphere.getRadius(), t);
			if(Intersection) {
				t_arr.push_back(t);
				color_arr.push_back(redsphere.getMyColor());
			}

			Intersection = intersectSphere(yellowsphere.getCenter(), org, dir, yellowsphere.getRadius(), t);
			if(Intersection) {
				t_arr.push_back(t);
				color_arr.push_back(yellowsphere.getMyColor());
			}

			Intersection = intersectSphere(bluesphere.getCenter(), org, dir, bluesphere.getRadius(), t);
			if(Intersection) {
				t_arr.push_back(t);
				color_arr.push_back(bluesphere.getMyColor());
			}

			Intersection = intersectSphere(greysphere.getCenter(), org, dir, greysphere.getRadius(), t);
			if(Intersection) {
				t_arr.push_back(t);
				color_arr.push_back(greysphere.getMyColor());
			}

			if(t_arr.size() == 0) {
				image[x][y].x = 1.0;
				image[x][y].y = 1.0;
				image[x][y].z = 1.0;

				putPixel32_nolock(x, y, convertColour(image[x][y]));
			}
			else {
				min_t = 1000.0;
				whichone = 0;
				for(i = 0; i < t_arr.size(); i++) {
					if(t_arr[i] < min_t) {
						whichone = i; min_t = t_arr[i];
					}
				}
				image[x][y].x = color_arr[whichone].x;
				image[x][y].y = color_arr[whichone].y;
				image[x][y].z = color_arr[whichone].z;

				putPixel32_nolock(x, y, convertColour(image[x][y]));
			}
		}
	}

	std::ofstream ofs("./test.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";

	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			ofs << (unsigned char)(std::min((float)1, (float)image[x][y].x) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].y) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].z) * 255);
		}
	}
	ofs.close();

}

void RayTracer::render(double dt) {
	//delete(image);
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

	bool doOnce = true;

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
			if(doOnce) {
				update();
				doOnce = false;
			}
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

bool RayTracer::intersectSphere(vec3 center, vec3 orig, vec3 dir, float radius, float& t) {
	float t0, t1; // solutions for t if the ray intersects 

// geometric solution  // vector dir has to be normalize, length is 1.0
	vec3 L = center - orig;
	float tca = dot(L, dir);
	if(tca < 0) return false;
	float d = dot(L, L) - tca * tca;
	if(d > (radius * radius)) return false;

	float thc = sqrt(radius * radius - d);
	t0 = tca - thc;
	t1 = tca + thc;

	if(t0 > t1) std::swap(t0, t1);

	if(t0 < 0) {
		t0 = t1; // if t0 is negative, let's use t1 instead 
		if(t0 < 0) return false; // both t0 and t1 are negative 
	}

	t = t0;
	return true;
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

void RayTracer::computeColourSphere(const vec3 sourcePt, const vec3 IntPt, const vec3 CenPt, const vec3 dir, float& ColValue) {
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

void RayTracer::shutDown() {
	delete(image);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
