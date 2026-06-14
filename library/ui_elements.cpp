#include "ui_elements.h"
#include "ui_scene.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_element::draw(transform tr)
{
}

void ui_element::render(transform tr)
{
	tr *= trans;
	if ((tr(calc_rect()) & scene->changed_rect).empty()) return;
	scene->canvas.set_drawing_rect(tr(local_rect) & scene->changed_rect);
	draw(tr);
	for (auto& element : children) element->render(tr);
}

rect ui_element::calc_rect()
{
	if (full_rect) return *full_rect;
	full_rect = local_rect;
	for (auto& element : children)
	{
		element->calc_rect();
		*full_rect |= element->trans(*element->full_rect);
	}
	return *full_rect;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_text::draw(transform tr)
{
	int sf = (int)(font_size * tr.scale + 0.5);
	scene->canvas.text(tr.offset, text, sf, text_color);
}
