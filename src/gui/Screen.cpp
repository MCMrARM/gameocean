#include "Screen.h"
#include "../App.h"

Screen::Screen(App *app) {
    this->x = 0;
    this->y = 0;
    this->width = app->guiWidth;
    this->height = app->guiHeight;
    this->clip = false;
}