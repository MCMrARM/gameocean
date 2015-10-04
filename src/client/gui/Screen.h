#pragma once

#include "GuiElementContainer.h"
class App;
class Animation;

class Screen : public GuiElementContainer {

public:
    std::vector<std::unique_ptr<Animation>> animations;

    Screen(App* app);

    virtual void render();

};
