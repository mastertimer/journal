#pragma once

#include "geometry.h"

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

union color
{
	uint c;
	struct
	{
		uchar b, g, r, a;
	};

	operator uint() const { return c; }

	color& operator=(uint c2) { c = c2; return *this; }

	bool operator==(color color_) const { return c == color_.c; }
	bool operator==(uint color_) const { return c == color_; }
	bool operator!=(color color_) const { return c != color_.c; }
	bool operator!=(uint color_) const { return c != color_; }
};
static_assert(sizeof(color) == 4);

constexpr color black_color = { 0xFF000000 };
constexpr color white_color = { 0xFFFFFFFF };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct picture
{
	explicit picture(size2i s);
	virtual ~picture();
	picture(const picture& copy);
	picture(picture&& move) noexcept;
	picture& operator=(picture&& move) noexcept;
	picture& operator=(const picture& copy);

	const size2i& get_size() const { return size; }
	virtual bool resize(size2i wh);
	void set_drawing_rect(const recti& q);

	void clear(color c = { 0xFF000000 });
	void fill_rectangle(recti r, color c, bool rep = false);

protected:
	color* data = nullptr;
	size2i size;
	bool transparent = false; // требует альфа-смешивания
	recti drawing_rect; // разрешенная область для рисования

	color& pixel(const i64 x, const i64 y) { return data[y * size.x + x]; }
	const color& pixel(const i64 x, const i64 y) const { return data[y * size.x + x]; }

private:
	template<typename Blender> void vertical_line(i64 x, intervali y, color c);
	template<class Blender>	void fill_rectangle_impl(recti r, color c);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct bitmap : public picture
{
	HDC hdc = nullptr;
	HBITMAP hbm = nullptr;

	bitmap();
	explicit bitmap(size2i s);

	bitmap(const bitmap& copy);
	bitmap(bitmap&& move) noexcept;

	bitmap& operator=(const bitmap& copy);
	bitmap& operator=(bitmap&& move) noexcept;

	~bitmap();

	bool resize(size2i wh) override;
	void set_drawing_rect(const recti& r);

	void set_text_colors(color tc, color bc);

private:
	color tc_cur = { 0 }; // цвет шрифта
	color bc_cur = { 0 }; // цвет фона шрифта
};