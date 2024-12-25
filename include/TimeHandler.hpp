#pragma once

#include <cstdint>
#include <chrono>

namespace engine2d {

class TimeHandler {
public:
    inline float GetFixedDeltaTime() const { return m_FixedDeltaTime * timeScale; }
    inline float GetUnscaledFixedDeltaTime() const { return m_FixedDeltaTime; }
    inline float GetDeltaTime() const { return m_DeltaTime * timeScale; }
    inline float GetUnscaledDeltaTime() const { return m_DeltaTime; }
    inline unsigned int GetFixedFramerate() const { return static_cast<unsigned int>(m_FixedFramerate); }
    inline unsigned int GetFramerate() const { return static_cast<unsigned int>(1 / m_DeltaTime); }
    inline uint32_t GetFrameCount() const { return m_FrameCount; }

    float timeScale;
private:
    TimeHandler();

    void InitTime();
    void UpdateDeltaTime();

    unsigned int m_FixedFramerate;
    float m_FixedDeltaTime;
    float m_DeltaTime;
    uint32_t m_FrameCount;

    std::chrono::system_clock::time_point m_FrameStart;

    friend class Object;
    friend class Engine2D;
};

} // namespace engine2d