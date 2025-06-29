#pragma once

#include "geometry.h"

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
    explicit picture(size2i s);

protected:
	color* data = nullptr;
	size2i size;
	bool transparent = false; // есть полупрозрачные пиксели
	rect2i drawing_rect; // разрешенная область для рисования
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct bitmap : public picture
{
    explicit bitmap(size2i s);
};