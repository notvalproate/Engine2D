#include "Core.hpp"

TimeHandler::TimeHandler() : timeScale(1), m_FixedFramerate(0), m_FixedDeltaTime(0), m_DeltaTime(0), m_FrameCount(0), m_FrameStart({}) { }

void TimeHandler::InitTime() {
	m_FixedFramerate = Object::Screen.m_Mode.refresh_rate;
	m_FixedDeltaTime = (float)1.0 / (float)m_FixedFramerate;
	m_FrameStart = std::chrono::system_clock::now();
}

void TimeHandler::UpdateDeltaTime() {
	m_FrameCount++;
	auto newTime = std::chrono::system_clock::now();
	m_DeltaTime = (std::chrono::duration_cast<std::chrono::microseconds>(newTime - m_FrameStart).count() / 1000000.0f);
	m_FrameStart = newTime;
}