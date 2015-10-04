#pragma once

#include "AnimationProperty.h"
#include "../gui/GuiElement.h"

class GuiAnimationProperty : public AnimationProperty {
protected:
    GuiElement& el;
public:
    GuiAnimationProperty(GuiElement& el) : el(el) { };
};
class GuiPosAnimationProperty : public GuiAnimationProperty {
private:
    float fromX, toX;
    float fromY, toY;
public:
    GuiPosAnimationProperty(GuiElement& el, float toX, float toY) : GuiAnimationProperty(el), toX(toX), toY(toY) {
        fromX = el.getX();
        fromY = el.getY();
    };
    virtual void set(float animStep) {
        el.setPosition(fromX + (toX - fromX) * animStep, fromY + (toY - fromY) * animStep);
    }
};