#include "smstrikers_pc/input_mapper.h"

#include <cmath>
#include <iostream>

int main()
{
    smstrikers::pc::InputMapper input;
    input.SetKeyDown(smstrikers::pc::HostKey::W, true);
    input.SetKeyDown(smstrikers::pc::HostKey::D, true);
    input.SetKeyDown(smstrikers::pc::HostKey::Space, true);
    input.SetKeyDown(smstrikers::pc::HostKey::Q, true);

    const smstrikers::pc::ControllerState state = input.ControllerOne();
    const float expectedAxis = std::sqrt(0.5F);
    if (std::abs(state.leftX - expectedAxis) > 0.0001F || std::abs(state.leftY - expectedAxis) > 0.0001F ||
        !state.IsPressed(smstrikers::pc::GameButton::A) || !state.IsPressed(smstrikers::pc::GameButton::L) ||
        state.leftTrigger != 1.0F)
    {
        std::cerr << "Keyboard input was not mapped to the expected controller state.\n";
        return 1;
    }

    input.SetKeyDown(smstrikers::pc::HostKey::Space, false);
    input.Reset();
    const smstrikers::pc::ControllerState cleared = input.ControllerOne();
    if (cleared.buttons != 0 || cleared.leftX != 0.0F || cleared.leftY != 0.0F)
    {
        std::cerr << "Input reset did not release controller state.\n";
        return 1;
    }
    return 0;
}
