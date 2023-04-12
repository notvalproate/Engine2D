#include "Game.hpp"

int main(int argc, char *argv[]) {

	//Get the static game object
	Game& game = Game::Get();
	game.Init("Platformer Game", "assets/textures/icon.png", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720);

	while (game.Exit()) {
		game.HandleEvents();
		game.Update();
		game.Render();
	}

	//Clean the game object
	game.Clean();

	return 0;
}