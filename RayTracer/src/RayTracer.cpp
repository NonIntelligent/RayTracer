#include "pch.h"
#include <SDL/SDL.h>

SDL_Window* window = nullptr;
SDL_Surface* surface = nullptr;

int main(int argc, char* args[]) {
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		std::cout << "SDL could not initialise SDL_Error: %s\n" << SDL_GetError() << std::endl;
		return 0;
	}

	window = SDL_CreateWindow("RayTracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_SHOWN);
	if(window == nullptr) {
		std::cout << "SDL could not create window: %s\n" << SDL_GetError() << std::endl;
		return 0;
	}

	surface = SDL_GetWindowSurface(window);

	SDL_Delay(4000);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
