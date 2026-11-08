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

void ui_scene::all_update()
{
	if (!root) return;
	auto visit = [&](auto&& self, const std::shared_ptr<ui_element>& element) -> void
	{
		if (element->update) element->update();
		for (const auto& child : element->children) self(self, child);
	};
	visit(visit, root);
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

void ui_scene::mouse_move(short x, short y, bool left_button_down)
{
	xy screen_mouse_pos{ double(x), double(y) };
	if (auto captured = mouse_capture.lock())
	{
		if (left_button_down)
		{
			captured->mouse_move2(screen_mouse_pos, mouse_capture_transform);
			return;
		}
		captured->mouse_up2(screen_mouse_pos, mouse_capture_transform);
		mouse_capture.reset();
	}
	auto previous_target = mouse_target.lock();
	mouse_target.reset();
	root->mouse_move(screen_mouse_pos);
	auto current_target = mouse_target.lock();
	if (previous_target && previous_target != current_target)
		previous_target->mouse_leave2();
}

void ui_scene::mouse_click(short x, short y)
{
	root->mouse_click({ double(x), double(y) });
}

void ui_scene::mouse_release(short x, short y)
{
	if (auto captured = mouse_capture.lock())
		captured->mouse_up2({ double(x), double(y) }, mouse_capture_transform);
	mouse_capture.reset();
}
