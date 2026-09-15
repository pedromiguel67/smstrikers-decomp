#include "smstrikers_pc/input_mapper.h"

#include <algorithm>
#include <cmath>

namespace smstrikers::pc {
namespace {

float Axis(bool negative, bool positive)
{
    return static_cast<float>(positive) - static_cast<float>(negative);
}

void Normalize(float& x, float& y)
{
    const float magnitude = std::sqrt((x * x) + (y * y));
    if (magnitude > 1.0F)
    {
        x /= magnitude;
        y /= magnitude;
    }
}

} // namespace

bool ControllerState::IsPressed(GameButton button) const
{
    return (buttons & static_cast<std::uint16_t>(button)) != 0;
}

void InputMapper::SetKeyDown(HostKey key, bool isDown)
{
    if (isDown)
    {
        keysDown_.insert(key);
    }
    else
    {
        keysDown_.erase(key);
    }
}

void InputMapper::Reset()
{
    keysDown_.clear();
}

ControllerState InputMapper::ControllerOne() const
{
    ControllerState state;
    state.leftX = Axis(IsDown(HostKey::A), IsDown(HostKey::D));
    state.leftY = Axis(IsDown(HostKey::S), IsDown(HostKey::W));
    state.rightX = Axis(IsDown(HostKey::Left), IsDown(HostKey::Right));
    state.rightY = Axis(IsDown(HostKey::Down), IsDown(HostKey::Up));
    Normalize(state.leftX, state.leftY);
    Normalize(state.rightX, state.rightY);

    if (IsDown(HostKey::Space))
    {
        state.buttons |= static_cast<std::uint16_t>(GameButton::A);
    }
    if (IsDown(HostKey::J))
    {
        state.buttons |= static_cast<std::uint16_t>(GameButton::B);
    }
    if (IsDown(HostKey::K))
    {
        state.buttons |= static_cast<std::uint16_t>(GameButton::X);
    }
    if (IsDown(HostKey::L))
    {
        state.buttons |= static_cast<std::uint16_t>(GameButton::Y);
    }
    if (IsDown(HostKey::Enter))
    {
        state.buttons |= static_cast<std::uint16_t>(GameButton::Start);
    }
    if (IsDown(HostKey::Q))
    {
        state.buttons |= static_cast<std::uint16_t>(GameButton::L);
        state.leftTrigger = 1.0F;
    }
    if (IsDown(HostKey::E))
    {
        state.buttons |= static_cast<std::uint16_t>(GameButton::R);
        state.rightTrigger = 1.0F;
    }
    return state;
}

bool InputMapper::IsDown(HostKey key) const
{
    return keysDown_.contains(key);
}

} // namespace smstrikers::pc
