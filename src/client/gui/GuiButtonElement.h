#pragma once

#include <string>
#include <functional>
#include "GuiNinePathImageElement.h"

class GuiButtonElement : public GuiNinePathImageElement {
    typedef std::function<void(GuiButtonElement*)> ClickCallback;
protected:
    int inactiveTexX, inactiveTexY;
    int activeTexX, activeTexY;
    std::string text;
    bool updatedText;
    int textW;
    Color textColor, inactiveColor, activeColor;
    ClickCallback clickCallback;

public:
    GuiButtonElement(float x, float y, int w, int h, std::string text);

    void setClickCallback(ClickCallback callback) {
        this->clickCallback = callback;
    }

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
    virtual void click() {
        if (clickCallback)
            clickCallback(this);
    };

    virtual void rebuild(RenderObjectBuilder& builder);

    virtual void setText(std::string newText);

};


