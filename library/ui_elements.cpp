#include "ui_elements.h"
#include "ui_scene.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ui_element::ui_element(ui_scene* scene_) : scene{ scene_ }
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

ui_text::ui_text(ui_scene* scene_) : ui_element(scene_)
{
}

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
	combined_region.reset();
	text = t;
	reset_regions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ui_text_edit::ui_text_edit(ui_scene* scene_) : ui_element(scene_)
{
	local_region = rect(size2i{ 200, 32 });
}

void ui_text_edit::draw(transform tr)
{
	auto oo = recti(tr(*local_region));
	scene->canvas.fill_rectangle(oo.expanded(-1), black_color);
	scene->canvas.rectangle(oo, white_color);
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 1) return;
	int l = (int)text.size();
	if (cursor < 0) cursor = 0;
	if (cursor > l) cursor = l;
	if (first > cursor) first = cursor;
	if (first + len2 > l) len2 = l - first;
	if (first + len2 < cursor) first = cursor - len2;
	int rx_text = (int)(oo.x.length() - 8);
	if ((l > 0) && (rx_text > 3))
	{
		size2i size = scene->canvas.size_text(text.substr(first, len2).c_str(), sf);
		if (size.x > rx_text)
		{
			while (first + len2 > cursor)
			{
				len2--;
				size = scene->canvas.size_text(text.substr(first, len2).c_str(), sf);
				if (size.x <= rx_text) break;
			}
			if (size.x > rx_text)
				while (first < cursor)
				{
					len2--;
					first++;
					size = scene->canvas.size_text(text.substr(first, len2).c_str(), sf);
					if (size.x <= rx_text) break;
				}
		}
		else if (size.x < rx_text)
		{
			while (first + len2 < l)
			{
				size = scene->canvas.size_text(text.substr(first, len2 + 1LL).c_str(), sf);
				if (size.x > rx_text) break;
				len2++;
			}
			if (size.x < rx_text)
				while (first > 0)
				{
					size = scene->canvas.size_text(text.substr(first - 1LL, len2 + 1LL).c_str(), sf);
					if (size.x > rx_text) break;
					len2++;
					first--;
				}
		}
		scene->canvas.text({ oo.x.min + 5, oo.y.min }, text.substr(first, len2).c_str(), sf, white_color, { 0 });
		if (first > 0)        scene->canvas.vertical_line(oo.x.min + 2, oo.y.expand(-1), { 0xFF30C0F0 });
		if (len2 < l - first) scene->canvas.vertical_line(oo.x.max - 2, oo.y.expand(-1), { 0xFF30C0F0 });
	}
	if (scene->keyboard_target == this)
	{
		size2i size = scene->canvas.size_text(text.substr(first, (i64)cursor - first).c_str(), sf);
		scene->canvas.vertical_line( oo.x.min + 4 + size.x, { oo.y.min + 1 , oo.y.min + sf + 1 }, white_color);
	}
}
