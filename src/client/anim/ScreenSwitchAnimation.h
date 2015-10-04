#pragma once

#include <gui/GuiElement.h>
#include <gui/GuiElementContainer.h>
#include "Animation.h"
class Screen;
class GuiElement;

class ScreenSwitchAnimation : public Animation {
protected:
    std::shared_ptr<Screen> screen1;
    std::shared_ptr<Screen> screen2;
public:
    ScreenSwitchAnimation(std::shared_ptr<Screen> screen1, std::shared_ptr<Screen> screen2, float duration) : Animation(duration), screen1(screen1), screen2(screen2) {
        this->easing = Animation::Easing::SWING;
    }

    virtual void render() { };
};

class FadeScreenSwitchAnimation : public ScreenSwitchAnimation {
public:
    FadeScreenSwitchAnimation(std::shared_ptr<Screen> screen1, std::shared_ptr<Screen> screen2, float duration) : ScreenSwitchAnimation(screen1, screen2, duration) { };

    virtual void updateValue(float val);
};

class MoveElementScreenSwitchAnimation : public ScreenSwitchAnimation {
private:
    float sX, sY, dX, dY, mX, mY;
    float sW, sH, dW, dH, i;
public:
    std::shared_ptr<GuiElement> el1;
    std::shared_ptr<GuiElement> el2;
    GuiElementContainer ctr1, ctr2;

    MoveElementScreenSwitchAnimation(std::shared_ptr<Screen> screen1, std::shared_ptr<Screen> screen2,
                                       std::shared_ptr<GuiElement> el1, std::shared_ptr<GuiElement> el2,
                                       float duration);

    virtual void updateValue(float val);

    virtual void render();
    virtual void finished();
};
