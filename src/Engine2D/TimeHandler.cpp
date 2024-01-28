#include "Core.hpp"

TimeHandler::TimeHandler() : m_DeltaTime(16) { }

void TimeHandler::InitTime() {
	m_DeltaTime = (float)1.0 / (float)Object::Screen.m_Mode.refresh_rate;
}