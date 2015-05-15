#pragma once

#include "../input/MouseEvent.h"

class RenderObjectBuilder;

struct GuiUpdateFlags {
    bool updateVertex = false;
    bool updateTextureUV = false;
    bool updateTextureId = false;
    bool updateColor = false;
};

class GuiElement {

protected:
    int x, y, width, height;
    int vertexCount;
    bool shouldUpdate = false;
    bool shouldRebuild = true;
    bool posUpdate = false;

public:
    int builderOffset = -1;

    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;

    GuiElement() {};
    GuiElement(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {};

    virtual bool isDynamic() { return false; };

    virtual bool needsUpdate() { return shouldUpdate; };
    virtual bool hasVertexCountUpdate() { return shouldRebuild; };
    virtual int getVertexCount() { return vertexCount; };
    virtual void rebuild(RenderObjectBuilder* builder) = 0;
    virtual GuiUpdateFlags update(RenderObjectBuilder* builder) = 0;
    inline void requireUpdate() { this->shouldUpdate = true; };
    inline void requireRebuild() { this->shouldRebuild = true; };

    virtual void setPosition(int x, int y);
    virtual void setSize(int w, int h);
    inline int getX() { return x; };
    inline int getY() { return y; };
    void setPositionAndSize(int x, int y, int w, int h) { setPosition(x, y); setSize(w, h); };

    virtual bool isPointInside(int x, int y);
    virtual bool supportsMultitouch() { return false; }
    virtual bool onMousePress(MousePressEvent& event) { return true; };
    virtual void onMouseMove(MouseMoveEvent& event) { };
    virtual void onMouseRelease(MouseReleaseEvent& event);
    virtual void click() { };

};


