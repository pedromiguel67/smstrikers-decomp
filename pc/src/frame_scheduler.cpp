#include "smstrikers_pc/frame_scheduler.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace smstrikers::pc {

FrameScheduler::FrameScheduler(double fixedStepSeconds, std::size_t maximumCatchUpSteps)
    : fixedStepSeconds_(fixedStepSeconds)
    , maximumCatchUpSteps_(maximumCatchUpSteps)
{
    if (!std::isfinite(fixedStepSeconds_) || fixedStepSeconds_ <= 0.0 || maximumCatchUpSteps_ == 0)
    {
        throw std::invalid_argument("Frame scheduler requires a positive finite fixed step and catch-up limit.");
    }
}

std::size_t FrameScheduler::Advance(double elapsedSeconds)
{
    if (!std::isfinite(elapsedSeconds) || elapsedSeconds < 0.0)
    {
        throw std::invalid_argument("Frame scheduler elapsed time must be finite and non-negative.");
    }

    const double maximumAccumulatedSeconds = fixedStepSeconds_ * static_cast<double>(maximumCatchUpSteps_);
    accumulatedSeconds_ = std::min(accumulatedSeconds_ + elapsedSeconds, maximumAccumulatedSeconds);

    const double roundingTolerance = fixedStepSeconds_ * 0.000000001;
    const std::size_t steps = static_cast<std::size_t>(std::floor((accumulatedSeconds_ + roundingTolerance) / fixedStepSeconds_));
    accumulatedSeconds_ = std::max(0.0, accumulatedSeconds_ - static_cast<double>(steps) * fixedStepSeconds_);
    return steps;
}

double FrameScheduler::FixedStepSeconds() const
{
    return fixedStepSeconds_;
}

double FrameScheduler::InterpolationAlpha() const
{
    return accumulatedSeconds_ / fixedStepSeconds_;
}

} // namespace smstrikers::pc
