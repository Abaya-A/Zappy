#pragma once

#include <algorithm>

namespace zappy::render {

class IntroAnimation {
public:
    IntroAnimation(float duration = 2.0f, float delay = 0.0f)
        : _duration(duration),
          _delay(delay)
    {
    }

    void restart()
    {
        _elapsed = 0.0f;
    }

    void update(float deltaTime)
    {
        _elapsed += std::max(0.0f, deltaTime);

        if (_elapsed > _delay + _duration)
            _elapsed = _delay + _duration;
    }

    float progress() const
    {
        if (_duration <= 0.0f)
            return 1.0f;

        const float animationTime = _elapsed - _delay;

        if (animationTime <= 0.0f)
            return 0.0f;

        const float linear = std::clamp(animationTime / _duration, 0.0f, 1.0f);

        return easeOutCubic(linear);
    }

    bool finished() const
    {
        return progress() >= 1.0f;
    }

private:
    static float easeOutCubic(float value)
    {
        const float inverse = 1.0f - value;

        return 1.0f - inverse * inverse * inverse;
    }

    float _elapsed = 0.0f;
    float _duration = 2.0f;
    float _delay = 0.0f;
};

}