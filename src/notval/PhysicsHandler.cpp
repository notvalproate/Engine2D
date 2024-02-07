#include "Core.hpp"

PhysicsHandler::PhysicsHandler() { }

void PhysicsHandler::StepCurrentWorld(const float deltaTime) const {
	GetCurrentWorld()->Step(deltaTime, 6, 2);
}