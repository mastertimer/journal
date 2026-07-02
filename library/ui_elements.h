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

	ui_element(ui_scene* scene_);
	virtual ~ui_element() {}

    void render(transform tr); // нарисовать дерево
	void reset_regions(std::optional<rect> a = std::nullopt, bool check_boundary = false);
	rect calc_combined_region(); // вычислить полную область
	virtual rect calc_local_region(); // вычислить локальную область

	void add_child(std::unique_ptr<ui_element> element);

protected:
	ui_scene* scene = nullptr;
	std::optional<rect> local_region;
	std::optional<rect> combined_region; // local_region + children.local_region

	virtual void draw(transform tr);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class text_align
{
	left,
	center,
	right
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_text : public ui_element
{
	ui_text(ui_scene* scene_);

	void draw(transform tr) override;
	rect calc_local_region() override;

	void set_text( std::wstring_view t );

private:
	std::wstring text;
	int font_size = 10;
	color text_color = white_color;
	text_align align = text_align::left;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_text_edit : public ui_element
{
	ui_text_edit(ui_scene* scene_);

	void draw(transform tr) override;

private:
	std::wstring text;
	int font_size = 10;
	//int cursor_position = 0; // абсолютное положение курсора
	//int text_offset = 0; // номер первого символа
	//int visible_text_length = 0; // длина куска
	int cursor = 0; // абсолютное положение курсора
	int first = 0; // номер первого символа
	int len2 = 0; // длина куска
};

struct ui_labeled_text_edit;