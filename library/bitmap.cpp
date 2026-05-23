#include "bitmap.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

picture::picture(size2i s, bool allocate_memory)
{
	drawing_rect = size = s;
	if (!size.empty() && allocate_memory) data = new color[size.count()];
}

picture::~picture()
{
	delete[] data;
}

picture::picture(const picture& copy) : size(copy.size), transparent(copy.transparent), drawing_rect(copy.drawing_rect)
{
	if (size.empty()) return;
	data = new color[size.count()];
	memcpy(data, copy.data, size.count() * sizeof(color));
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
	memcpy(data, copy.data, size.count() * sizeof(color));
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
	auto new_data = (wh.empty()) ? nullptr : new color[wh.count()];
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
	hdc = CreateCompatibleDC(nullptr);
	if (hdc) resize(s);
}

bitmap::~bitmap()
{
	DeleteObject(hbm);
	DeleteDC(hdc);
	data = nullptr;
}

bool bitmap::resize(size2i wh)
{
	if (size == wh) return false;

	HBITMAP new_hbm = nullptr;
	color* new_data = nullptr;

	if (!wh.empty())
	{
		BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), (long)wh.x, -(long)wh.y, 1, 32, BI_RGB };
		new_hbm = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, (void**)&new_data, 0, 0);
		if (!new_hbm) return false;
		SelectObject(hdc, new_hbm);
	}

	DeleteObject(hbm);

	hbm = new_hbm;
	data = new_data;
	size = wh;
	drawing_rect = size;
	transparent = false;

	return true;
}