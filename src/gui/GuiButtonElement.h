#pragma once

#include <string>
#include "GuiNinePathImageElement.h"

class GuiButtonElement : public GuiNinePathImageElement {

    int inactiveTexX, inactiveTexY;
    int activeTexX, activeTexY;
    std::string text;

public:
    GuiButtonElement(int x, int y, int w, int h, std::string text);

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

    virtual void rebuild(RenderObjectBuilder* builder);

};


