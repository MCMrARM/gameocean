#pragma once



class GuiElement {

protected:
    int x, y, width, height;

public:
    GuiElement();

    virtual bool isDynamic() { return false; };
    //virtual void build() = 0;

};


