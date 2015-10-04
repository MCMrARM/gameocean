#include "ScreenSwitchAnimation.h"
#include "opengl.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <render/Shader.h>

#include "../gui/Screen.h"

void FadeScreenSwitchAnimation::updateValue(float val) {
    screen1->color.a = 1.f - val;
    screen2->color.a = val;
}

MoveElementScreenSwitchAnimation::MoveElementScreenSwitchAnimation(std::shared_ptr<Screen> screen1,
                                                                       std::shared_ptr<Screen> screen2,
                                                                       std::shared_ptr<GuiElement> el1,
                                                                       std::shared_ptr<GuiElement> el2,
                                                                       float duration) : ScreenSwitchAnimation(screen1, screen2, duration), el1(el1), el2(el2) {
    screen1->removeElement(el1);
    screen2->removeElement(el2);
    sX = el1->getX();
    sY = el1->getY();
    dX = el2->getX();
    dY = el2->getY();
    sW = el1->getWidth();
    sH = el1->getHeight();
    dW = el2->getWidth();
    dH = el2->getHeight();
    el1->setPosition(0, 0);
    el2->setPosition(0, 0);
    ctr1.addElement(el1);
    ctr2.addElement(el2);
    ctr1.setSize(el1->getWidth(), el1->getHeight());
    ctr2.setSize(el2->getWidth(), el2->getHeight());
    ctr1.clip = false;
    ctr2.clip = false;
    mX = sX;
    mY = sY;
}

void MoveElementScreenSwitchAnimation::updateValue(float val) {
    screen1->color.a = 1.f - val;
    screen2->color.a = val;
    ctr2.color.a = val;
    mX = sX + (dX - sX) * val;
    mY = sY + (dY - sY) * val;
    //float mW = sW + (dW - sW) * val;
    //float mH = sW + (dW - sW) * val;
    i = val;
}

void MoveElementScreenSwitchAnimation::render() {
    glm::mat4 view = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(mX, mY, -1.0f)), glm::vec3((dW / sW) * i + 1.f * (1.f - i), (dH / sH) * i + 1.f * (1.f - i), 1.f));
    glUniformMatrix4fv(Shader::current->viewMatrixUniform(), 1, false, glm::value_ptr(view));
    ctr1.render();
    view = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(mX, mY, -1.0f)), glm::vec3((sW / dW) * (1.f - i) + i, (sH / dH) * (1.f - i) + i, 1.f));
    glUniformMatrix4fv(Shader::current->viewMatrixUniform(), 1, false, glm::value_ptr(view));
    ctr2.render();
    view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glUniformMatrix4fv(Shader::current->viewMatrixUniform(), 1, false, glm::value_ptr(view));
}

void MoveElementScreenSwitchAnimation::finished() {
    ctr1.removeElement(el1);
    ctr2.removeElement(el2);
    el1->setPosition(sX, sY);
    el2->setPosition(dX, dY);
    screen1->addElement(el1);
    screen2->addElement(el2);
}