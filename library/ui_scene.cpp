#include "ui_scene.h"

recti ui_scene::draw(size2i s)
{
	if (canvas.resize(s)) changed_rect = s;
	if (changed_rect.empty()) return {};
	canvas.set_drawing_rect(changed_rect);
	changed_rect &= canvas.get_size();
	canvas.clear(background_color);
	if (root) root->render({});
	recti result = changed_rect;
	changed_rect = rect();
	return result;
}

ui_scene::ui_scene()
{
	root = std::make_unique<ui_element>();
	root->scene = this;
}

void ui_scene::add_changed_rect(rect a)
{
	changed_rect |= (a & canvas.get_size());
}
