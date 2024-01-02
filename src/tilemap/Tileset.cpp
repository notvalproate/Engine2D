#include "Tileset.hpp"

Tileset::Tileset() {

}

Tileset::~Tileset() {
	SDL_DestroyTexture(m_Atlas);
}