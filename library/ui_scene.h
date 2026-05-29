#pragma once

#include "bitmap.h"

struct ui_scene
{
	bitmap canvas;
	rect changed_rect;

	color background_color = black_color; // цвет фона

	recti draw(size2i s);
};