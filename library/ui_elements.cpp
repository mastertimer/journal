#include "ui_elements.h"
#include "ui_scene.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_element::render(transform tr)
{
	tr *= trans;
	if ((tr(calc_combined_region()) & scene->changed_region).empty()) return;
	scene->canvas.set_drawing_rect(tr(calc_local_region()) & scene->changed_region);
	draw(tr);
	for (auto& element : children) element->render(tr);
}

void ui_element::add_child(std::shared_ptr<ui_element> element)
{
	children.push_back(element);
	element->parent = this;
	element->scene = scene;
	element->update_regions(std::nullopt, region_change::add);
}

void ui_element::update_regions(std::optional<rect> r, region_change change)
{
	if (!scene) return;
	if (!r)
	{
		if (change == region_change::modify)
		{
			if (!local_region) { update_regions({}, region_change::add); return; }
			r = local_region;
		}
		else
			r = calc_combined_region();
	}
	else if ( combined_region.has_value() && change != region_change::modify &&
		      (!(*r <= *combined_region) || (change == region_change::remove && r->touches_boundary(*combined_region)) ))
			combined_region.reset();
	if (!parent)
	{
		if (scene && scene->root.get() == this)	scene->add_changed_rect(trans(*r));
		return;
	}
	parent->update_regions(trans(*r), change);
}

rect& ui_element::calc_combined_region()
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

rect& ui_element::calc_local_region()
{
	if (local_region.has_value()) return *local_region;
	local_region.emplace();
	return *local_region;
}

bool ui_element::mouse_move(xy mouse_pos)
{
	mouse_pos = trans.inverse()(mouse_pos);
	if (!calc_combined_region().test(mouse_pos)) return false;
	for (auto& element : children) if (element->mouse_move(mouse_pos)) return true;
	return local_region->test(mouse_pos) && mouse_move2(mouse_pos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_text::draw(transform tr)
{
	int sf = (int)(font_size * tr.scale + 0.5);
	scene->canvas.text(tr.offset, text, sf, text_color);
}

rect& ui_text::calc_local_region()
{
	if (local_region.has_value()) return *local_region;
	local_region = scene->canvas.size_text(text, font_size);
	return *local_region;
}

void ui_text::set_text(std::wstring_view t)
{
	if (text == t) return;
	update_regions(std::nullopt, region_change::remove);
	local_region.reset();
	combined_region.reset();
	text = t;
	update_regions(std::nullopt, region_change::add);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ui_text_edit::ui_text_edit()
{
	local_region = rect(size2i{ 200, font_size + vertical_indentation * 2 + 2 });
}

void ui_text_edit::draw(transform tr)
{
	auto oo = recti(tr(*local_region));
	scene->canvas.fill_rectangle(oo.expanded(-1), black_color);
	scene->canvas.rectangle(oo, white_color);
	int sf = (int)(font_size * tr.scale + 0.5);
	if (sf < 1) return;
	i64 top_indent = (oo.y.length() - sf) / 2;
	int l = (int)text.size();
	if (cursor < 0) cursor = 0;
	if (cursor > l) cursor = l;
	if (first > cursor) first = cursor;
	if (first + len2 > l) len2 = l - first;
	if (first + len2 < cursor) first = cursor - len2;
	int rx_text = (int)(oo.x.length() - 8);
	if ((l > 0) && (rx_text > 3))
	{
		size2i size = scene->canvas.size_text(text.substr(first, len2), sf);
		if (size.x > rx_text)
		{
			while (first + len2 > cursor)
			{
				len2--;
				size = scene->canvas.size_text(text.substr(first, len2), sf);
				if (size.x <= rx_text) break;
			}
			if (size.x > rx_text)
				while (first < cursor)
				{
					len2--;
					first++;
					size = scene->canvas.size_text(text.substr(first, len2), sf);
					if (size.x <= rx_text) break;
				}
		}
		else if (size.x < rx_text)
		{
			while (first + len2 < l)
			{
				size = scene->canvas.size_text(text.substr(first, len2 + 1LL), sf);
				if (size.x > rx_text) break;
				len2++;
			}
			if (size.x < rx_text)
				while (first > 0)
				{
					size = scene->canvas.size_text(text.substr(first - 1LL, len2 + 1LL), sf);
					if (size.x > rx_text) break;
					len2++;
					first--;
				}
		}
		scene->canvas.text({ oo.x.min + 5, oo.y.min + top_indent }, text.substr(first, len2), sf, white_color, { 0 });
		if (first > 0)        scene->canvas.vertical_line(oo.x.min + 2, oo.y.expanded(-1), { 0xFF30C0F0 });
		if (len2 < l - first) scene->canvas.vertical_line(oo.x.max - 3, oo.y.expanded(-1), { 0xFF30C0F0 });
	}
	if (scene->keyboard_target.lock().get() == this)
	{
		size2i size = scene->canvas.size_text(text.substr(first, (i64)cursor - first), sf);
		scene->canvas.vertical_line( oo.x.min + 4 + size.x, { oo.y.min + top_indent , oo.y.min + sf + top_indent }, white_color);
	}
}

void ui_text_edit::set_text(std::wstring_view t)
{
	if (text == t) return;
	text = t;
	update_regions(std::nullopt, region_change::modify);
}

void ui_text_edit::key_down(u64 key)
{
	switch (key)
	{
	case 8: // backspace
		if (cursor <= 0) return;
		text.erase(cursor - 1, 1);
		cursor--;
		break;
	case 37: // left
		if (cursor <= 0) return;
		cursor--;
		break;
	case 39: // right
		if (cursor >= text.size()) return;
		cursor++;
		break;
	case 46: // delete
		if (cursor >= text.size()) return;
		text.erase(cursor, 1);
		break;
	default:
		return;
	}
	update_regions({}, region_change::modify);
}

void ui_text_edit::key_press(u64 key)
{
	if (key < 32) return;
	text.insert(cursor, 1, wchar_t(key));
	cursor++;
	update_regions({}, region_change::modify);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ui_button::ui_button() : ui_element()
{
	local_region = rect(size2i{ 200, font_size + vertical_indentation * 2 + 2 });
}

void ui_button::draw(transform tr)
{
	auto oo = recti(tr(*local_region));
	scene->canvas.fill_rectangle(oo.expanded(-1), black_color);
	scene->canvas.rectangle(oo, white_color);
	int sf = (int)(font_size * tr.scale + 0.5);
	auto size = scene->canvas.size_text(caption, font_size);

	scene->canvas.text(oo.center() - size.center(), caption, sf, white_color, { 0 });
}

void ui_button::set_caption(std::wstring_view t)
{
	if (caption == t) return;
	caption = t;
	update_regions(std::nullopt, region_change::modify);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ui_value_edit::ui_value_edit() : ui_element()
{
	local_region = rect(size2i{ 200, font_size * 2 + vertical_indentation * 4 + 3 });
}

void ui_value_edit::draw(transform tr)
{
	auto oo = recti(tr(*local_region));
	scene->canvas.fill_rectangle(oo.expanded(-1), black_color);
	scene->canvas.rectangle(oo, white_color);
	scene->canvas.horizontal_line(oo.x.expanded(-1), floori(oo.y.center()), white_color);
}