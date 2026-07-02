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
	root = std::make_unique<ui_element>(this);
}

void ui_scene::add_changed_rect(rect a)
{
	changed_region |= (a & canvas.get_size());
}
