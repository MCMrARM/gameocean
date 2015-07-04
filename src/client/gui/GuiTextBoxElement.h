#pragma once

#include "GuiNinePathImageElement.h"

class GuiTextBoxElement : public GuiNinePathImageElement {

protected:
    static const int CARRET_BLINK_TIME = 500; // in ms

    bool active = false;

    int inactiveTexX, inactiveTexY;
    int activeTexX, activeTexY;
    std::string text;
    int textW;
    Color textColor;
    bool carretVisible;
    long long carretTime = 0;

    inline void buildCarret(RenderObjectBuilder* builder);

public:
    GuiTextBoxElement(int x, int y, int w, int h, std::string text);

    virtual void focus();
    virtual void blur();

    inline void setInactiveTexturePosition(int x, int y) {
        inactiveTexX = x;
        inactiveTexY = y;
    };
    inline void setActiveTexturePosition(int x, int y) {
        activeTexX = x;
        activeTexY = y;
    };

    virtual bool needsUpdate() { return (shouldUpdate | active); };

    virtual void rebuild(RenderObjectBuilder* builder);
    virtual GuiUpdateFlags update(RenderObjectBuilder* builder);

    virtual void setText(std::string newText);

};


