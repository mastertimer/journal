#pragma once

#include "bitmap.h"

struct ui_scene;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum class region_change
{
	add,
	remove,
	modify
};

struct ui_element
{
	friend ui_scene;

	transform trans;
    ui_element* parent = nullptr;
    std::vector<std::shared_ptr<ui_element>> children;
    std::unique_ptr<bitmap> render_cache;

	ui_element(ui_scene* scene_);
	virtual ~ui_element() {}

    void render(transform tr); // нарисовать дерево
	void update_regions(std::optional<rect> r, region_change change);
	rect& calc_combined_region(); // вычислить полную область
	virtual rect& calc_local_region(); // вычислить локальную область

	void add_child(std::shared_ptr<ui_element> element);

	virtual void key_down(u64 key) {}
	virtual void key_press(u64 key) {}

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
	rect& calc_local_region() override;

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
	void key_down(u64 key) override;
	void key_press(u64 key) override;

	void set_text(std::wstring_view t);

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