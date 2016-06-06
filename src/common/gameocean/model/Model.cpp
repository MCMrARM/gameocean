#include "Model.h"

std::map<std::string, Model*> Model::models;

Model *Model::getModel(std::string name) {
    if (models.count(name) > 0)
        return models.at(name);
    Model *model = new Model();
    models[name] = model;
}