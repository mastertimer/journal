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

picture::picture(const picture& copy) : size(copy.size), transparent(copy.transparent), drawing_rect(copy.size)
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
	size = wh;
	delete[] data;
	data = (size.empty()) ? nullptr : new color[size.square()];
	drawing_rect = size;
	transparent = false;
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

bitmap::bitmap(size2i s) : picture(s, false)
{
	font.lfHeight         = 13;              // высота шрифта или символа
	font.lfWeight         = 100;             // толщина шрифта в диапазоне от 0 до 1000
	font.lfCharSet        = DEFAULT_CHARSET; // набор символов
	wcsncpy_s(font.lfFaceName, LF_FACESIZE, L"Tahoma", LF_FACESIZE - 1);

	BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), (long)s.x, -(long)s.y, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	if (hbm = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, (void**)(&data), 0, 0); !hbm) return;
	if (hdc = CreateCompatibleDC(nullptr); !hdc) return;
	SelectObject(hdc, hbm);
	f_c = GetTextColor(hdc);
	f_cf = GetBkColor(hdc);
}

bitmap::~bitmap()
{
	DeleteObject(hfont);
	DeleteObject(hbm);
	DeleteDC(hdc);
	data = nullptr;
}
