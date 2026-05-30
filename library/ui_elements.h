#pragma once

#include "base.h"

struct ui_scene;

struct ui_element
{
    ui_scene* scene = nullptr;
    ui_element* parent = nullptr;
    std::vector<std::unique_ptr<ui_element>> children;

    virtual void draw(transform tr) = 0;
};
