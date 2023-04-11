#pragma once
#include "SDL.h"

class Game {
public:
	Game();
	~Game();

	void Init(const char* title, const char* iconpath, const int& x, const int& y, int width, int height);
	void HandleEvents();
	void Update();
	void Render();
	void Clean();
	inline bool Exit() { return IsRunning; }

private:
	bool IsRunning;
	float FrameDelta;
	int w, h;
	SDL_Window* Window;
	SDL_Renderer* Renderer;
	SDL_DisplayMode Mode;
	SDL_Event Event;

	bool InFocus();
};