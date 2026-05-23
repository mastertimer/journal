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

	operator uint() const { return c; }

	color& operator=(uint c2) { c = c2; return *this; }

	bool operator==(const color color_) const { return c == color_.c; }
	bool operator==(const uint color_) const { return c == color_; }
	bool operator!=(const color color_) const { return c != color_.c; }
	bool operator!=(const uint color_) const { return c != color_; }
};

static_assert(sizeof(color) == 4);

////////////////////////////////////////////////////////////////////////////////////////////////////

struct picture
{
	explicit picture(size2i s, bool allocate_memory = true);
	virtual ~picture();
	picture(const picture& copy);
	picture(picture&& move) noexcept;
	picture& operator=(picture&& move) noexcept;
	picture& operator=(const picture& copy);

	virtual bool resize(size2i wh);

protected:
	color* data = nullptr;
	size2i size;
	bool transparent = false; // есть полупрозрачные пиксели
	rect2i drawing_rect; // разрешенная область для рисования
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct bitmap : public picture
{
	HDC hdc = nullptr;
	HBITMAP hbm = nullptr;

    explicit bitmap(size2i s);
	bitmap(const bitmap&) = delete;
	bitmap& operator=(const bitmap&) = delete;

	~bitmap();

	bool resize(size2i wh) override;
};