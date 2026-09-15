#include "smstrikers_pc/frame_scheduler.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

int main()
{
    smstrikers::pc::FrameScheduler scheduler(0.1, 3);
    if (scheduler.Advance(0.05) != 0 || std::abs(scheduler.InterpolationAlpha() - 0.5) > 0.000001)
    {
        std::cerr << "Frame scheduler did not preserve a partial fixed step.\n";
        return 1;
    }
    if (scheduler.Advance(0.05) != 1 || std::abs(scheduler.InterpolationAlpha()) > 0.000001)
    {
        std::cerr << "Frame scheduler did not emit a completed fixed step.\n";
        return 1;
    }
    if (scheduler.Advance(10.0) != 3)
    {
        std::cerr << "Frame scheduler did not cap catch-up work.\n";
        return 1;
    }

    try
    {
        (void)scheduler.Advance(-0.1);
    }
    catch (const std::invalid_argument&)
    {
        return 0;
    }
    std::cerr << "Frame scheduler accepted invalid elapsed time.\n";
    return 1;
}
