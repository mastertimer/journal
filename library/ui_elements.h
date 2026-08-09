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

	void set_text(std::wstring_view t);

private:
	constexpr static int vertical_indentation = 2;
	std::wstring text;
	int font_size = default_font_size;
	int cursor = 0; // абсолютное положение курсора
	int first = 0; // номер первого символа
	int len2 = 0; // длина куска
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

struct ui_value_edit : public ui_element
{
	ui_value_edit();

	void draw(transform tr) override;

private:
	constexpr static int vertical_indentation = 2;
	int font_size = default_font_size;
};