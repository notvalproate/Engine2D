#include "Game.hpp"
#include "SDL_image.h"
#include <iostream>

Game::Game() : IsRunning(false), FrameDelta(16), Window(nullptr), Renderer(nullptr) { }

Game::~Game() { }

void Game::Init(const char* title, const char* iconpath, const int& x, const int& y, int width, int height) {
	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		std::cout << "Error: Couldn't Initialize Subsystems..." << std::endl;
		return;
	}
	std::cout << "Stage: Initialized Subsystems..." << std::endl;

	//Get the user's display mode
	if (SDL_GetDisplayMode(0, 0, &Mode)) {
		std::cout << "Error: Couldn't Get Display Mode...Framerate set to 60" << std::endl;
	}
	else {
		std::cout << "Stage: Display Mode Initialized..." << std::endl;
		FrameDelta = (float)1000 / (float)Mode.refresh_rate;
		//Set minimum frame-time to 1000 / refresh rate of the user's monitor.
	}

	//Width and Height of the screen
	w = width;
	h = height;

	//Create an SDL Window
	if (!(Window = SDL_CreateWindow(title, x, y, width, height, 0))) {
		std::cout << "Error: Couldn't Initialize Window..." << std::endl;
		return;
	}

	SDL_Surface* TempSurface = IMG_Load(iconpath);
	SDL_SetWindowIcon(Window, TempSurface); //Setting window icon
	SDL_FreeSurface(TempSurface);
	std::cout << "Stage: Initialized Window..." << std::endl;

	//Create an SDL Renderer
	if (!(Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED))) {
		std::cout << "Error: Couldn't Initialize Renderer..." << std::endl;
		return;
	}
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	std::cout << "Stage: Initialized Renderer..." << std::endl;

	IsRunning = true; //Boolean to check if window close
}

void Game::HandleEvents() {
	SDL_PollEvent(&Event);
	if (Event.type == SDL_QUIT) {
		IsRunning = false;
		return;
	}
	if (Event.type == SDL_KEYDOWN) {
		switch (Event.key.keysym.sym) { //Check for which key, pause if esc, toggle fullscreen if f11
		case SDLK_F11:
			//If the window doesnt have fullscreen flag, set the window to fullscreen
			std::cout << SDL_GetWindowGrab(Window) << std::endl;
			if (!(SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN)) {
				SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN);
				SDL_SetWindowSize(Window, Mode.w, Mode.h);
				break;
			}
			//Else remove window fullscreen flag and set resolution
			SDL_SetWindowFullscreen(Window, 0);
			SDL_SetWindowSize(Window, 1280, 720);
			break;
		}
	}
}

void Game::Update() {
	SDL_RenderClear(Renderer); 
}
 
void Game::Render() {
	SDL_RenderPresent(Renderer); 

	if (!InFocus()) {
		SDL_WaitEvent(NULL); //If Window not in focus, do not render and wait for window to come in focus.
	}
}

void Game::Clean() {
	SDL_DestroyWindow(Window); 
	SDL_DestroyRenderer(Renderer); 
	SDL_Quit(); 
	std::cout << "Game Cleaned" << std::endl;
} 

bool Game::InFocus() {
	//Get the window's flags and check if it is In Focus.
	return (SDL_GetWindowFlags(Window) & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS));
}