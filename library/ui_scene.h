#pragma once

#include "bitmap.h"

struct ui_scene
{
	bitmap canvas;
	rect2i changed_rect;
};