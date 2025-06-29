#include "bitmap.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

picture::picture(size2i s, bool allocate_memory)
{
	drawing_rect = size = s;
	if (!size.empty() && allocate_memory) data = (color*)(new uint[size.square()]);
}

picture::~picture()
{
	delete[] data;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bitmap::bitmap(size2i s) : picture(s, false)
{
	font.lfHeight         = 13;              // высота шрифта или символа
	font.lfWeight         = 100;             // толщина шрифта в диапазоне от 0 до 1000
	font.lfCharSet        = DEFAULT_CHARSET; // набор символов
	wcsncpy_s(font.lfFaceName, LF_FACESIZE, L"Tahoma", LF_FACESIZE - 1);

	BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), (long)s.x, (long) - s.y, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
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
