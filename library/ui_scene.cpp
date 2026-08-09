#include "ui_scene.h"

recti ui_scene::draw(size2i s)
{
	if (canvas.resize(s)) changed_region = s;
	if (changed_region.empty()) return {};
	canvas.set_drawing_rect(changed_region);
	changed_region &= canvas.get_size();
	canvas.clear(background_color);
	if (root) root->render({});
	recti result = changed_region;
	changed_region = rect();
	return result;
}

ui_scene::ui_scene()
{
	root = std::make_shared<ui_element>();
	root->scene = this;
}

void ui_scene::add_changed_rect(rect a)
{
	changed_region |= (a & canvas.get_size());
}

void ui_scene::key_down(u64 key)
{
	if (auto target = keyboard_target.lock())
	{
		target->key_down(key);
	}
}

void ui_scene::key_press(u64 key)
{
	if (auto target = keyboard_target.lock())
	{
		target->key_press(key);
	}
}

void ui_scene::mouse_move(short x, short y)
{
	root->mouse_move({ double(x), double(y) });
}
