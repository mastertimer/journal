#pragma once

#include "bitmap.h"
#include "ui_elements.h"

struct ui_scene
{
	bitmap canvas;
	rect changed_region;

	std::shared_ptr<ui_element> root;
	std::weak_ptr<ui_element> keyboard_target;

	color background_color = black_color; // цвет фона

	ui_scene();
	recti draw(size2i s);
	void add_changed_rect(rect a);
};