#pragma once

#include "bitmap.h"
#include "journal_data.h"
#include "ui_elements.h"

struct ui_scene
{
	journal_data data;
	bitmap canvas;
	rect changed_region;

	std::shared_ptr<ui_element> root;
	std::weak_ptr<ui_element> keyboard_target;
	std::weak_ptr<ui_element> mouse_target;
	std::weak_ptr<ui_element> mouse_capture;
	transform mouse_capture_transform;

	color background_color = black_color; // цвет фона

	ui_scene();
	recti draw(size2i s);
	void add_changed_rect(rect a);
	void all_update();

	void key_down(u64 key);
	void key_press(u64 key);
	void mouse_move(short x, short y, bool left_button_down = false);
	void mouse_click(short x, short y);
	void mouse_release(short x, short y);
};
