#include "Core.hpp"

PhysicsHandler::PhysicsHandler() : m_RenderSceneColliders(false) { }

void PhysicsHandler::RenderColliders() const {

}

void PhysicsHandler::SetRenderColliders(const bool set) {
	m_RenderSceneColliders = set;
}