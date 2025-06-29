#pragma once

#include "geometry.h"

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

union color
{
	uint c;
	struct
	{
		uchar b, g, r, a;
	};

	color() = default;
	constexpr color(uint c2) : c(c2) {}

	operator uint() const { return c; }

	color& operator=(uint c2) { c = c2; return *this; }

	bool operator==(const color color_) const { return c == color_.c; }
	bool operator==(const uint color_) const { return c == color_; }
	bool operator!=(const color color_) const { return c != color_.c; }
	bool operator!=(const uint color_) const { return c != color_; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct picture
{
    explicit picture(size2i s, bool allocate_memory = true);
	~picture();

protected:
	color* data = nullptr;
	size2i size;
	bool transparent = false; // есть полупрозрачные пиксели
	rect2i drawing_rect; // разрешенная область для рисования
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct bitmap : public picture
{
	HBITMAP hbm = nullptr;
	HDC hdc = nullptr;
	HFONT hfont = nullptr;
	LOGFONT font = {};
	uint f_c = 0; // цвет шрифта
	uint f_cf = 0; // цвет фона шрифта

    explicit bitmap(size2i s);
	~bitmap();
};