#include "Core.hpp"

RenderingHandler::RenderingHandler() : m_Renderer(nullptr), m_SortingLayers({ SortingLayer("Default") }) { }

bool RenderingHandler::InitRenderer() {
	if (!(m_Renderer = SDL_CreateRenderer(Object::Screen.m_Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE))) {
		std::cout << "Error: Couldn't Initialize Renderer..." << std::endl;
		return false;
	}

	SDL_SetRenderTarget(m_Renderer, NULL);
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

	std::cout << "Stage: Initialized Renderer..." << std::endl;

	return true;
}

void RenderingHandler::RenderSprite(SDL_Texture* texture, const SDL_Rect src, const SDL_Rect dest, const double angle) {
	SDL_RenderCopyEx(m_Renderer, texture, &src, &dest, angle, NULL, SDL_FLIP_NONE);
}


void RenderingHandler::AddSortingLayer(const std::string_view name) {
	m_SortingLayers.push_back(SortingLayer(name));
}

void RenderingHandler::AddGameObjectToRenderer(GameObject* gameObject) {
	m_SortingLayers[0].m_GameObjectsInLayer.push_back(gameObject);
}

bool RenderingHandler::SetSortingLayer(GameObject* gameObject, const std::string_view layerName, const std::string_view previousLayer) {
	auto it = std::find_if(
		m_SortingLayers.begin(),
		m_SortingLayers.end(),
		[&previousLayer](const SortingLayer& layer) {
			return layer.name == previousLayer;
		});

	auto& prevLayerObjects = (*it).m_GameObjectsInLayer;
	
	for (std::size_t i = 0; i < prevLayerObjects.size(); i++) {
		if (prevLayerObjects[i] == gameObject) {
			prevLayerObjects.erase(prevLayerObjects.begin() + i);
			break;
		}
	}

	auto it2 = std::find_if(
		m_SortingLayers.begin(), 
		m_SortingLayers.end(), 
		[&layerName](const SortingLayer& layer) {
			return layer.name == layerName; 
		});

	if (it2 == m_SortingLayers.end()) {
		m_SortingLayers[0].m_GameObjectsInLayer.push_back(gameObject);
		return false;
	}

	(*it2).m_GameObjectsInLayer.push_back(gameObject);
	return true;
}

void RenderingHandler::ClearSortingLayers() {
	for (auto& layer : m_SortingLayers) {
		layer.m_GameObjectsInLayer.clear();
	}
}

void RenderingHandler::RenderSortingLayers() {
	for (const auto& layer : m_SortingLayers) {
		for (const auto& gO : layer.m_GameObjectsInLayer) {
			gO->Render();
		}
	}
}

void RenderingHandler::PresentRenderer() {
	SDL_RenderPresent(m_Renderer);
	SDL_RenderClear(m_Renderer);

	if (!Object::Screen.InFocus()) {
		SDL_WaitEvent(NULL);
	}
}