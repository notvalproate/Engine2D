#pragma once

#include <filesystem>
#include <vector>

#include "SDL.h"

#include "Vector2D.hpp"

namespace engine2d {

enum class CursorLockMode : uint8_t {
    None = 0,
    Locked = 1,
    Confined = 2,
    CursorLockModeCount = 3
};

enum class SystemCursorType : uint8_t {
    Arrow = 0,
    IBeam = 1,
    Wait = 2,
    Crosshair = 3,
    WaitArrow = 4,
    NWSEArrow = 5,
    NESWArrow = 6,
    EWArrow = 7,
    NSArrow = 8,
    AllArrow = 9,
    No = 10,
    Hand = 11,
    SystemCursorCount = 12
};

class CustomCursor {
public:
    CustomCursor(const std::filesystem::path& cursorPath, const Vector2D& hotspot);
    ~CustomCursor();
private:
    SDL_Cursor* m_Cursor;

    friend class CursorHandler;
};

class CursorHandler {
public:
    void SetVisibility(bool visible);
    void SetLockState(CursorLockMode lockType);
    void SetCursor(SystemCursorType cursor);
    void SetCursor(const CustomCursor& cursor);

    bool GetVisibility() const;
    CursorLockMode GetLockState() const;
private:
    CursorHandler();

    void InitSystemCursors();
    void DestroySystemCursors();

    bool m_Visibility;
    CursorLockMode m_LockState;
    std::vector<SDL_Cursor*> m_SystemCursors;

    friend class Object;
    friend class Engine2D;
};

} // namespace engine2d