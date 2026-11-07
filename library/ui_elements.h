#pragma once

#include "bitmap.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr int default_font_size = 22;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	transform trans;
    ui_element* parent = nullptr;
    std::vector<std::shared_ptr<ui_element>> children;
    std::unique_ptr<bitmap> render_cache;
	ui_scene* scene = nullptr;

	virtual ~ui_element() {}

    void render(transform tr); // нарисовать дерево
	void update_regions(std::optional<rect> r, region_change change);
	rect& calc_combined_region(); // вычислить полную область
	virtual rect& calc_local_region(); // вычислить локальную область

	void add_child(std::shared_ptr<ui_element> element);

	virtual void key_down(u64 key) {}
	virtual void key_press(u64 key) {}
	bool mouse_move(xy mouse_pos); // перемещение мышки
	virtual bool mouse_move2(xy r) { return true;  } // перемещение мышки действие
	bool mouse_click(xy mouse_pos); // щелчок мышью
	virtual bool mouse_click2(xy r) { return true; } // действие по щелчку мышью

protected:
	std::optional<rect> local_region;
	std::optional<rect> combined_region; // local_region + children.local_region

	virtual void draw(transform tr) {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class horizontal_align
{
	left,
	center,
	right
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_text : public ui_element
{
	void draw(transform tr) override;
	rect& calc_local_region() override;

	void set_text( std::wstring_view t );

private:
	std::wstring text;
	int font_size = default_font_size;
	color text_color = white_color;
	horizontal_align align = horizontal_align::left;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_text_edit : public ui_element
{
	ui_text_edit();

	void draw(transform tr) override;
	void key_down(u64 key) override;
	void key_press(u64 key) override;
	bool mouse_click2(xy r) override;

	void set_text(std::wstring_view t);

private:
	constexpr static int vertical_indentation = 2;
	std::wstring text;
	int font_size = default_font_size;
	int cursor = 0; // абсолютное положение курсора
	int visible_text_begin = 0; // индекс первого отображаемого символа
	int visible_text_end = 0; // индекс после последнего отображаемого символа

	void update_visible_text(int scaled_font_size, int available_width);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_button : public ui_element
{
	ui_button();

	void draw(transform tr) override;
	void set_caption(std::wstring_view c);

private:
	constexpr static int vertical_indentation = 2;
	std::wstring caption;
	int font_size = default_font_size;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr int ui_list_scroll_bar_indent = 2;
constexpr color ui_list_line_color = { 0xFFFFFFFF };
constexpr color ui_list_text_color = { 0xFFFFFFFF };
constexpr color ui_list_scroll_bar_color = { 0xFFFFFFFF };

struct ui_list : public ui_element
{
	ui_list();

	void draw(transform tr) override;

private:
	std::vector<std::wstring> items;
	constexpr static int vertical_indentation = 2;
	int font_size = default_font_size;
	std::optional<int> selected_item;
	int visible_items_count = 0;
	int element_height = 0;
	int first_item = 0; // номер первого отображаемого элемента
	bool scroll_bar = false;
	int scroll_bar_width = 8;
};
