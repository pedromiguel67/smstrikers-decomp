#pragma once

#include <cstdint>
#include <unordered_set>

namespace smstrikers::pc {

enum class HostKey : std::uint8_t {
    Escape,
    Enter,
    Space,
    Q,
    E,
    W,
    A,
    S,
    D,
    I,
    J,
    K,
    L,
    Up,
    Down,
    Left,
    Right,
};

enum class GameButton : std::uint16_t {
    A = 1 << 0,
    B = 1 << 1,
    X = 1 << 2,
    Y = 1 << 3,
    Start = 1 << 4,
    L = 1 << 5,
    R = 1 << 6,
};

struct ControllerState {
    std::uint16_t buttons = 0;
    float leftX = 0.0F;
    float leftY = 0.0F;
    float rightX = 0.0F;
    float rightY = 0.0F;
    float leftTrigger = 0.0F;
    float rightTrigger = 0.0F;

    [[nodiscard]] bool IsPressed(GameButton button) const;
};

class InputMapper {
public:
    void SetKeyDown(HostKey key, bool isDown);
    void Reset();
    [[nodiscard]] ControllerState ControllerOne() const;

private:
    [[nodiscard]] bool IsDown(HostKey key) const;

    std::unordered_set<HostKey> keysDown_;
};

} // namespace smstrikers::pc
