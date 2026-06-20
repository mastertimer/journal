#pragma once

#include "bitmap.h"
#include "ui_elements.h"

struct ui_scene
{
	bitmap canvas;
	rect changed_rect;

	std::unique_ptr<ui_element> root;

	color background_color = black_color; // цвет фона

	ui_scene();
	recti draw(size2i s);
};