#include "Core.hpp"

TimeHandler::TimeHandler() : m_FixedDeltaTime(16), m_DeltaTime(0), m_FrameStart({}) { }

void TimeHandler::InitTime() {
	m_FixedDeltaTime = (float)1.0 / (float)Object::Screen.m_Mode.refresh_rate;
	m_FrameStart = std::chrono::system_clock::now();
}

void TimeHandler::UpdateDeltaTime() {
	auto newTime = std::chrono::system_clock::now();
	m_DeltaTime = (std::chrono::duration_cast<std::chrono::microseconds>(newTime - m_FrameStart).count() / 1000.0f);
	m_FrameStart = newTime;
}