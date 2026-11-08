#include "ui_elements.h"
#include "ui_scene.h"

#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ui_element::render(transform tr)
{
	tr *= trans_;
	if ((tr(calc_combined_region()) & scene->changed_region).empty()) return;
	scene->canvas.set_drawing_rect(tr(calc_local_region()) & scene->changed_region);
	draw(tr);
	for (auto& element : children) element->render(tr);
}

void ui_element::set_transform(transform value)
{
	if (trans_ == value) return;
	update_regions({}, region_change::remove);
	trans_ = value;
	update_regions({}, region_change::add);
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
		if (scene && scene->root.get() == this)	scene->add_changed_rect(trans_(*r));
		return;
	}
	parent->update_regions(trans_(*r), change);
}

rect& ui_element::calc_combined_region()
{
	if (combined_region.has_value()) return *combined_region;
	combined_region = calc_local_region();
	for (auto& element : children)
	{
		element->calc_combined_region();
		*combined_region |= element->trans_(*element->combined_region);
	}
	return *combined_region;
}

rect& ui_element::calc_local_region()
{
	if (local_region.has_value()) return *local_region;
	local_region.emplace();
	return *local_region;
}

bool ui_element::mouse_move(xy screen_mouse_pos, transform tr)
{
	tr *= trans_;
	if (!tr(calc_combined_region()).test(screen_mouse_pos)) return false;
	for (auto& element : children) if (element->mouse_move(screen_mouse_pos, tr)) return true;
	return tr(calc_local_region()).test(screen_mouse_pos) && mouse_move2(screen_mouse_pos, tr);
}

bool ui_element::mouse_click(xy screen_mouse_pos, transform tr)
{
	tr *= trans_;
	if (!tr(calc_combined_region()).test(screen_mouse_pos)) return false;
	for (auto& element : children) if (element->mouse_click(screen_mouse_pos, tr)) return true;
	if (tr(calc_local_region()).test(screen_mouse_pos) && mouse_click2(screen_mouse_pos, tr))
	{
		auto previous_target = scene->keyboard_target.lock();
		if (previous_target.get() != this)
		{
			scene->keyboard_target = shared_from_this();
			if (previous_target) previous_target->update_regions({}, region_change::modify);
			update_regions({}, region_change::modify);
		}
		return true;
	}
	return false;
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
	auto bounds = recti(tr(*local_region));
	draw_frame(bounds);

	int sf = (int)(font_size * tr.scale + 0.5);
	if (sf < 1) return;
	i64 top_indent = (bounds.y.length() - sf) / 2;
	update_visible_text(sf, (int)bounds.x.length() - 8);
	draw_visible_text(bounds, sf, top_indent);
	if (scene->keyboard_target.lock().get() == this)
		draw_cursor(bounds, sf, top_indent);
}

void ui_text_edit::draw_frame(recti bounds)
{
	scene->canvas.fill_rectangle(bounds.expanded(-1), black_color);
	color frame_color = scene->keyboard_target.lock().get() == this
		? ui_active_element_frame_color
		: white_color;
	scene->canvas.rectangle(bounds, frame_color);
}

void ui_text_edit::draw_visible_text(recti bounds, int scaled_font_size, i64 top_indent)
{
	if (!visible_text.empty())
		scene->canvas.text({ bounds.x.min + 5, bounds.y.min + top_indent },
			text.substr(visible_text.min, visible_text.length()),
			scaled_font_size, white_color, { 0 });

	if (visible_text.min > 0)
		scene->canvas.vertical_line(bounds.x.min + 2, bounds.y.expanded(-1), { 0xFF30C0F0 });
	if (visible_text.max < (int)text.size())
		scene->canvas.vertical_line(bounds.x.max - 3, bounds.y.expanded(-1), { 0xFF30C0F0 });
}

void ui_text_edit::draw_cursor(recti bounds, int scaled_font_size, i64 top_indent)
{
	auto cursor_text = text.substr(visible_text.min, cursor - visible_text.min);
	i64 cursor_offset = scene->canvas.size_text(cursor_text, scaled_font_size).x;
	scene->canvas.vertical_line(bounds.x.min + 4 + cursor_offset,
		{ bounds.y.min + top_indent, bounds.y.min + scaled_font_size + top_indent }, white_color);
}

void ui_text_edit::update_visible_text(int scaled_font_size, int available_width)
{
	int text_length = (int)text.size();
	cursor = std::clamp(cursor, 0, text_length);
	visible_text.min = std::clamp((int)visible_text.min, 0, cursor);
	visible_text.max = std::clamp((int)visible_text.max, cursor, text_length);
	if (available_width <= 3)
	{
		visible_text = { cursor, cursor };
		return;
	}

	auto width = [&](int begin, int end)
	{
		return scene->canvas.size_text(text.substr(begin, end - begin), scaled_font_size).x;
	};
	while (visible_text.max > visible_text.min && width((int)visible_text.min, (int)visible_text.max) > available_width)
	{
		if (visible_text.max > cursor) --visible_text.max;
		else ++visible_text.min;
	}
	while (visible_text.max < text_length && width((int)visible_text.min, (int)visible_text.max + 1) <= available_width)
		++visible_text.max;
	while (visible_text.min > 0 && width((int)visible_text.min - 1, (int)visible_text.max + 1) <= available_width)
		--visible_text.min, ++visible_text.max;
}

void ui_text_edit::set_text(std::wstring_view t)
{
	if (text == t) return;
	text = t;
	cursor = std::min(cursor, (int)text.size());
	visible_text.min = std::min((int)visible_text.min, cursor);
	visible_text.max = cursor;
	update_regions(std::nullopt, region_change::modify);
}

bool ui_text_edit::mouse_click2(xy screen_mouse_pos, transform tr)
{
	int sf = (int)(font_size * tr.scale + 0.5);
	auto bounds = recti(tr(*local_region));
	int x = (int)screen_mouse_pos.x - (int)bounds.x.min - 5;
	int position = (int)visible_text.min;
	if (x > 0)
	{
		auto visible = std::wstring_view(text).substr(visible_text.min, visible_text.length());
		auto measurement = scene->canvas.measure_text_fit(visible, sf, (int)bounds.x.length() - 8);
		int count = std::min(measurement.fitting_characters, (int)measurement.prefix_widths.size());
		for (int i = 0; i < count; ++i)
		{
			int left = i == 0 ? 0 : measurement.prefix_widths[i - 1];
			int right = measurement.prefix_widths[i];
			if (x < left + (right - left + 1) / 2) break;
			position = (int)visible_text.min + i + 1;
		}
	}
	cursor = position;
	update_regions({}, region_change::modify);
	return true;
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

ui_list::ui_list() : ui_element()
{
	local_region = rect(size2i{ 200, (font_size + vertical_indentation * 2) * 5 + 2 }); // 5 строк

	items.push_back(L"Элемент 1");
	items.push_back(L"Элемент 2");
	items.push_back(L"Элемент 3");
	items.push_back(L"Элемент 4 В лесу родилась елечка");
	items.push_back(L"Элемент 5");
	items.push_back(L"Элемент 6");
	selected_item = 3;
}

void ui_list::draw(transform tr)
{
	auto bounds = recti(tr(*local_region));
	draw_frame(bounds);

	int scaled_font_size = (int)(font_size * tr.scale + 0.5);
	if (!prepare_layout(bounds, scaled_font_size)) return;

	draw_scroll_bar(bounds, scaled_font_size);
	draw_items(bounds, scaled_font_size);
	draw_selected_item(bounds, scaled_font_size);
}

void ui_list::draw_frame(recti bounds)
{
	scene->canvas.fill_rectangle(bounds.expanded(-1), black_color);
	color frame_color = scene->keyboard_target.lock().get() == this
		? ui_active_element_frame_color
		: white_color;
	scene->canvas.rectangle(bounds, frame_color);
}

bool ui_list::prepare_layout(recti bounds, int scaled_font_size)
{
	if (items.empty() || scaled_font_size < 1)
	{
		selected_item.reset();
		visible_items_count = 0;
		return false;
	}

	element_height = scaled_font_size + vertical_indentation * 2;
	visible_items_count = (int)((bounds.y.length() - 2) / element_height);
	if (visible_items_count < 1)
	{
		selected_item.reset();
		return false;
	}

	int item_count = (int)items.size();
	first_item = std::clamp(first_item, 0, std::max(0, item_count - visible_items_count));
	if (selected_item && (*selected_item < first_item || *selected_item >= first_item + visible_items_count))
		selected_item.reset();
	return true;
}

void ui_list::draw_scroll_bar(recti bounds, int scaled_font_size)
{
	int item_count = (int)items.size();
	if (item_count <= visible_items_count) return;

	int bar_width = scaled_font_size;
	int bar_x = (int)bounds.x.max - bar_width - 3;
	scene->canvas.vertical_line(bar_x, bounds.y, ui_list_line_color);
	scene->canvas.rectangle(scroll_thumb_bounds(bounds, scaled_font_size), ui_list_scroll_bar_color);
}


recti ui_list::scroll_thumb_bounds(recti bounds, int scaled_font_size) const
{
	int item_count = (int)items.size();
	auto track = bounds.y.expanded(-1).expanded(-ui_list_scroll_bar_indent);
	int thumb_height = std::max(1, (int)(track.length() * visible_items_count / item_count));
	int scroll_range = item_count - visible_items_count;
	int thumb_offset = (int)((track.length() - thumb_height) * first_item / scroll_range);
	int bar_x = (int)bounds.x.max - scaled_font_size - 3;
	return recti(
		{ bar_x + ui_list_scroll_bar_indent + 1, (int)bounds.x.max - ui_list_scroll_bar_indent - 1 },
		{ track.min + thumb_offset, track.min + thumb_offset + thumb_height });
}

void ui_list::draw_items(recti bounds, int scaled_font_size)
{
	if ((int)items.size() > visible_items_count)
		bounds.x.max -= scaled_font_size + 3;

	int last_item = std::min((int)items.size(), first_item + visible_items_count);
	int text_width = (int)bounds.x.length() - 6;
	for (int item = first_item; item < last_item; ++item)
	{
		int row = item - first_item;
		std::wstring_view text = items[item];
		auto fit = scene->canvas.measure_text_fit(text, scaled_font_size, text_width);
		int fitting_characters = std::clamp(fit.fitting_characters, 0, (int)text.size());
		text = text.substr(0, fitting_characters);
		scene->canvas.text(bounds.top_left() + ixy(4, element_height * row + 1),
			text, scaled_font_size, ui_list_text_color, { 0 });
	}
}

void ui_list::draw_selected_item(recti bounds, int scaled_font_size)
{
	if (!selected_item) return;
	if ((int)items.size() > visible_items_count)
		bounds.x.max -= scaled_font_size + 3;

	int row = *selected_item - first_item;
	intervali item_x{ bounds.x.min + 2, bounds.x.max - 1 };
	intervali item_y{ bounds.y.min + 1 + element_height * row,
		bounds.y.min + 1 + element_height * (row + 1) };
	scene->canvas.rectangle({ item_x, item_y }, ui_list_selected_item_color);
}

bool ui_list::mouse_click2(xy screen_mouse_pos, transform tr)
{
	int scaled_font_size = (int)(font_size * tr.scale + 0.5);
	auto bounds = recti(tr(*local_region));
	if (!prepare_layout(bounds, scaled_font_size)) return true;

	int item_count = (int)items.size();
	if (item_count > visible_items_count)
	{
		int bar_x = (int)bounds.x.max - scaled_font_size - 3;
		if (screen_mouse_pos.x >= bar_x)
		{
			auto thumb = scroll_thumb_bounds(bounds, scaled_font_size);
			if (screen_mouse_pos.y >= thumb.y.min && screen_mouse_pos.y < thumb.y.max)
			{
				dragging_scroll_bar = true;
				scroll_drag_offset = (int)screen_mouse_pos.y - (int)thumb.y.min;
				scene->mouse_capture = shared_from_this();
				scene->mouse_capture_transform = tr;
			}
			return true;
		}
	}

	double row_offset = screen_mouse_pos.y - bounds.y.min - 1;
	if (row_offset < 0) return true;
	int row = (int)(row_offset / element_height);
	if (row < 0 || row >= visible_items_count) return true;
	int item = first_item + row;
	if (item >= 0 && item < item_count && selected_item != item)
	{
		selected_item = item;
		update_regions({}, region_change::modify);
	}
	return true;
}

bool ui_list::mouse_move2(xy screen_mouse_pos, transform tr)
{
	if (!dragging_scroll_bar) return true;

	auto bounds = recti(tr(*local_region));
	int scaled_font_size = (int)(font_size * tr.scale + 0.5);
	if (!prepare_layout(bounds, scaled_font_size)) return true;

	int item_count = (int)items.size();
	int scroll_range = item_count - visible_items_count;
	if (scroll_range <= 0) return true;

	auto thumb = scroll_thumb_bounds(bounds, scaled_font_size);
	auto track = bounds.y.expanded(-1).expanded(-ui_list_scroll_bar_indent);
	int thumb_travel = (int)(track.length() - thumb.y.length());
	if (thumb_travel <= 0) return true;

	int thumb_top = (int)screen_mouse_pos.y - scroll_drag_offset;
	thumb_top = std::clamp(thumb_top, (int)track.min, (int)(track.max - thumb.y.length()));
	int new_first_item = (int)(((thumb_top - track.min) * scroll_range + thumb_travel / 2) / thumb_travel);
	new_first_item = std::clamp(new_first_item, 0, scroll_range);
	if (new_first_item != first_item)
	{
		first_item = new_first_item;
		if (selected_item && (*selected_item < first_item || *selected_item >= first_item + visible_items_count))
			selected_item.reset();
		update_regions({}, region_change::modify);
	}
	return true;
}

void ui_list::mouse_up2(xy, transform)
{
	dragging_scroll_bar = false;
}
