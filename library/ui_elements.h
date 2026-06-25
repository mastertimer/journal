#pragma once

#include "bitmap.h"

struct ui_scene;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_element
{
	friend ui_scene;

	transform trans;
    ui_element* parent = nullptr;
    std::vector<std::unique_ptr<ui_element>> children;
    std::unique_ptr<bitmap> render_cache;

	ui_element();
	virtual ~ui_element() {}

    void render(transform tr); // нарисовать дерево
	void add_area(std::optional<rect> a = std::nullopt); // эта область добавлена
	rect calc_full_rect(); // вычислить полную область
	virtual rect calc_local_rect(); // вычислить локальную область

	void add_child(std::unique_ptr<ui_element> element);

protected:
	ui_scene* scene = nullptr;
	std::optional<rect> local_rect;
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
	rect calc_local_rect() override;

private:
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_text_edit : public ui_element
{
	ui_text_edit();

	void draw(transform tr) override;

private:
	std::wstring text;
	int font_size = 10;
	int cursor_position = 0; // абсолютное положение курсора
	int text_offset = 0; // номер первого символа
	int visible_text_length = 0; // длина куска
};

struct ui_labeled_text_edit;