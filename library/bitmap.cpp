#include "bitmap.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

picture::picture(size2i s, bool allocate_memory)
{
	drawing_rect = size = s;
	if (!size.empty() && allocate_memory) data = new color[size.square()];
}

picture::~picture()
{
	delete[] data;
}

picture::picture(const picture& copy) : size(copy.size), transparent(copy.transparent), drawing_rect(copy.drawing_rect)
{
	if (size.empty()) return;
	data = new color[size.square()];
	memcpy(data, copy.data, size.square() * sizeof(color));
}

picture::picture(picture&& move) noexcept : data(move.data), size(move.size), transparent(move.transparent),
drawing_rect(move.drawing_rect)
{
	move.data = nullptr;
	move.drawing_rect = move.size = { 0,0 };
}

picture& picture::operator=(const picture& copy)
{
	if (&copy == this) return *this;
	resize(copy.size);
	transparent = copy.transparent;
	memcpy(data, copy.data, size.square() * sizeof(color));
	return *this;
}

picture& picture::operator=(picture&& move) noexcept
{
	if (&move == this) return *this;
	delete[] data;
	data = move.data;
	size = move.size;
	transparent = move.transparent;
	drawing_rect = move.drawing_rect;
	move.data = nullptr;
	move.drawing_rect = move.size = { 0,0 };
	return *this;
}

bool picture::resize(size2i wh)
{
	if (size == wh) return false;
	auto new_data = (wh.empty()) ? nullptr : new color[wh.square()];
	delete[] data;
	data = new_data;
	size = wh;
	drawing_rect = size;
	transparent = false;
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

bitmap::bitmap(size2i s) : picture(s, false)
{
	BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), (long)s.x, -(long)s.y, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	if (hbm = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, (void**)(&data), 0, 0); !hbm) return;
	if (hdc = CreateCompatibleDC(nullptr); !hdc)
	{
		DeleteObject(hbm);
		hbm = nullptr;
		data = nullptr;
		return;
	}
	SelectObject(hdc, hbm);
}

bitmap::~bitmap()
{
	DeleteDC(hdc);
	DeleteObject(hbm);
	data = nullptr;
}
