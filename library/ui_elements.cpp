#include "ui_elements.h"
#include "ui_scene.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ui_element::ui_element()
{
}

void ui_element::draw(transform tr)
{
}

void ui_element::render(transform tr)
{
	tr *= trans;
	if ((tr(calc_full_rect()) & scene->changed_rect).empty()) return;
	scene->canvas.set_drawing_rect(tr(calc_local_rect()) & scene->changed_rect);
	draw(tr);
	for (auto& element : children) element->render(tr);
}

void ui_element::add_child(std::unique_ptr<ui_element> element)
{
	auto el = element.get();
	children.push_back(std::move(element));
	el->parent = this;
	el->scene = scene;
	el->add_area();
}

void ui_element::add_area(std::optional<rect> a)
{
	if (!a)
		a = calc_full_rect();
	else
		if (full_rect.has_value() && !(*a <= *full_rect)) full_rect.reset();
	if (!parent)
	{
		if (scene->root.get() == this) scene->add_changed_rect(trans(*a));
		return;
	}
	parent->add_area(trans(*a));
}

rect ui_element::calc_full_rect()
{
	if (full_rect.has_value()) return *full_rect;
	full_rect = calc_local_rect();
	for (auto& element : children)
	{
		element->calc_full_rect();
		*full_rect |= element->trans(*element->full_rect);
	}
	return *full_rect;
}

rect ui_element::calc_local_rect()
{
	if (local_rect.has_value()) return *local_rect;
	local_rect.emplace();
	return *local_rect;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_text::draw(transform tr)
{
	int sf = (int)(font_size * tr.scale + 0.5);
	scene->canvas.text(tr.offset, text, sf, text_color);
}

rect ui_text::calc_local_rect()
{
	if (local_rect.has_value()) return *local_rect;
	local_rect = scene->canvas.size_text(text, font_size);
	return *local_rect;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ui_text_edit::ui_text_edit()
{
	local_rect = rect(size2i{ 200, 32 });
}

void ui_text_edit::draw(transform tr)
{
	auto r = recti(tr(*local_rect));
	scene->canvas.fill_rectangle(r.expanded(-1), black_color);
	scene->canvas.rectangle(r, white_color);
}
