#pragma once

#include <cstddef>

namespace smstrikers::pc {

class FrameScheduler {
public:
    explicit FrameScheduler(double fixedStepSeconds = 1.0 / 60.0, std::size_t maximumCatchUpSteps = 5);

    [[nodiscard]] std::size_t Advance(double elapsedSeconds);
    [[nodiscard]] double FixedStepSeconds() const;
    [[nodiscard]] double InterpolationAlpha() const;

private:
    double fixedStepSeconds_;
    double accumulatedSeconds_ = 0.0;
    std::size_t maximumCatchUpSteps_;
};

} // namespace smstrikers::pc
