#include "Core.hpp"

CollisionHandler::CollisionHandler() {
	m_World = std::make_unique<b2World>(b2Vec2(0.0f, 9.8f));
}