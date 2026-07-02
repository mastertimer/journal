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
	if ((tr(calc_combined_region()) & scene->changed_region).empty()) return;
	scene->canvas.set_drawing_rect(tr(calc_local_region()) & scene->changed_region);
	draw(tr);
	for (auto& element : children) element->render(tr);
}

void ui_element::add_child(std::unique_ptr<ui_element> element)
{
	auto el = element.get();
	children.push_back(std::move(element));
	el->parent = this;
	el->scene = scene;
	el->reset_regions();
}

void ui_element::reset_regions(std::optional<rect> a, bool check_boundary)
{
	if (!a)	a = calc_combined_region();
	else if (combined_region.has_value())
	{
		bool reset = !(*a <= *combined_region);
		if (check_boundary)	reset |= a->touches_boundary(*combined_region);
		if (reset) combined_region.reset();
	}
	if (!parent)
	{
		if (scene && scene->root.get() == this) scene->add_changed_rect(trans(*a));
		return;
	}
	parent->reset_regions(trans(*a), check_boundary);
}

rect ui_element::calc_combined_region()
{
	if (combined_region.has_value()) return *combined_region;
	combined_region = calc_local_region();
	for (auto& element : children)
	{
		element->calc_combined_region();
		*combined_region |= element->trans(*element->combined_region);
	}
	return *combined_region;
}

rect ui_element::calc_local_region()
{
	if (local_region.has_value()) return *local_region;
	local_region.emplace();
	return *local_region;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_text::draw(transform tr)
{
	int sf = (int)(font_size * tr.scale + 0.5);
	scene->canvas.text(tr.offset, text, sf, text_color);
}

rect ui_text::calc_local_region()
{
	if (local_region.has_value()) return *local_region;
	local_region = scene->canvas.size_text(text, font_size);
	return *local_region;
}

void ui_text::set_text(std::wstring_view t)
{
	if (text == t) return;
	reset_regions(std::nullopt, true);
	local_region.reset();
	text = t;
	reset_regions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ui_text_edit::ui_text_edit()
{
	local_region = rect(size2i{ 200, 32 });
}

void ui_text_edit::draw(transform tr)
{
	auto r = recti(tr(*local_region));
	scene->canvas.fill_rectangle(r.expanded(-1), black_color);
	scene->canvas.rectangle(r, white_color);
}
