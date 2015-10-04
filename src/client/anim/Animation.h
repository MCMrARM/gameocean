#pragma once

#include <memory>
#include <cmath>
#include "AnimationProperty.h"
#include "utils/Time.h"

class Animation {
public:
    typedef std::function<float(float)> EasingFunc;

    struct Easing {
        static float LINEAR(float val) {
            return val;
        }
        static float SWING(float val) {
            return (0.5 - cos(val * M_PI) / 2);
        }
    };

    long long startTime;
    int duration;
    EasingFunc easing = Easing::LINEAR;

    Animation(float duration) {
        startTime = Time::now();
        this->duration = (int) (duration * 1000);
    }

    virtual bool tick() {
        float t = ((float) (int) (Time::now() - startTime)) / ((float) duration);
        if (t > 1.f) {
            t = 1.f;
        } else {
            t = easing(t);
        }
        updateValue(t);
        if (t == 1.f) {
            finished();
            return true;
        }
        return false;
    }
    virtual void updateValue(float val) = 0;
    virtual void finished() {};
};

class PropertyAnimation : public Animation {

public:
    std::unique_ptr<AnimationProperty> property;

    PropertyAnimation(std::unique_ptr<AnimationProperty> property, float duration, EasingFunc easing) : Animation(duration), property(std::move(property)) {
        this->easing = easing;
    };

    virtual void updateValue(float val) {
        property->set(val);
    }
};