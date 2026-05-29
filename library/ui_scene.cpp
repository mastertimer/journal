#include "ui_scene.h"

recti ui_scene::draw(size2i s)
{
	if (canvas.resize(s)) changed_rect = s;
	if (changed_rect.empty()) return {};
	canvas.set_drawing_rect(changed_rect);
	changed_rect &= canvas.get_size();
	canvas.clear(background_color);
//	n_ko->ris(_trans());
	recti result = changed_rect;
	changed_rect = rect();
	return result;
}
