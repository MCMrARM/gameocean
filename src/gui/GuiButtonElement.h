#pragma once

#include "GuiNinePathImageElement.h"

class GuiButtonElement : public GuiNinePathImageElement {

    int inactiveTexX, inactiveTexY;
    int activeTexX, activeTexY;

public:
    GuiButtonElement(int x, int y, int w, int h);

    inline void setInactiveTexturePosition(int x, int y) {
        inactiveTexX = x;
        inactiveTexY = y;
    };
    inline void setActiveTexturePosition(int x, int y) {
        activeTexX = x;
        activeTexY = y;
    };

    virtual bool onMousePress(MousePressEvent& event);
    virtual void onMouseRelease(MouseReleaseEvent& event);

};


