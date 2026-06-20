#pragma once

#include "bitmap.h"

struct ui_scene;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_element
{
	friend ui_scene;

	rect local_rect;
	transform trans;
    ui_element* parent = nullptr;
    std::vector<std::unique_ptr<ui_element>> children;
    std::unique_ptr<bitmap> render_cache;

	ui_element();
	virtual ~ui_element() {}

    void render(transform tr); // нарисовать дерево
	rect calc_rect(); // вычислить область

	void add_child(std::unique_ptr<ui_element> element);

protected:
	ui_scene* scene = nullptr;
	std::optional<rect> full_rect; // local_rect + children.local_rect

	virtual void draw(transform tr);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_text : public ui_element
{
	std::wstring text;
	int font_size = 10;
	color text_color = white_color;
	void draw(transform tr) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_text_edit;
struct ui_labeled_text_edit;