#include "bitmap.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
std::array<std::wstring, 3> font_names = { L"Cascadia Code", L"Times New Roman", L"Courier New" };

struct blend_replace
{
	color cc;
	blend_replace(color c) : cc(c) {}
	void blend(color& c) const { c = cc; }
	static void mix2(const color cc, color& c) { c = cc; }
};

struct blend_over_opaque
{
	uint kk;
	uint d_b;
	uint d_g;
	uint d_r;

	blend_over_opaque(color c) :
		kk(255 - c.a),
		d_b(c.b* (uint(c.a) + 1)),
		d_g(c.g* (uint(c.a) + 1)),
		d_r(c.r* (uint(c.a) + 1))
	{
	}

	void blend(color& c) const
	{
		c.b = (c.b * kk + d_b) >> 8;
		c.g = (c.g * kk + d_g) >> 8;
		c.r = (c.r * kk + d_r) >> 8;
	}

	static void mix2(const color cc, color& c)
	{
		uint kk = 255 - cc.a;
		uint k2 = uint(cc.a) + 1;
		c.b = (c.b * kk + cc.b * k2) >> 8;
		c.g = (c.g * kk + cc.g * k2) >> 8;
		c.r = (c.r * kk + cc.r * k2) >> 8;
	}

	static void mixmix2(const color cc1, const color cc2, color& c)
	{
		uint k1 = cc1.a;
		uint k2 = cc2.a;
		uint kk = 256 - cc1.a - cc2.a;
		c.b = (c.b * kk + cc1.b * k1 + cc2.b * k2) >> 8;
		c.g = (c.g * kk + cc1.g * k1 + cc2.g * k2) >> 8;
		c.r = (c.r * kk + cc1.r * k1 + cc2.r * k2) >> 8;
	}
};

struct blend_over_alpha
{
	uint kk;
	uint d_b;
	uint d_g;
	uint d_r;

	blend_over_alpha(color c)
	{
		kk = 255 - c.a;
		uint k2 = (uint(c.a) + 1) * 255;
		d_b = c.b * k2;
		d_g = c.g * k2;
		d_r = c.r * k2;
	}

	void blend(color& c) const
	{
		uint kk_ = 255 - c.a;
		uint k2_ = (uint(c.a) + 1) * kk;
		uint znam = 65535 - kk * kk_;
		c.b = (c.b * k2_ + d_b) / znam;
		c.g = (c.g * k2_ + d_g) / znam;
		c.r = (c.r * k2_ + d_r) / znam;
		c.a = 255 - ((kk_ * kk) >> 8);
	}

	static void mix2(const color cc, color& c)
	{
		uint kk = 255 - cc.a;
		uint k2 = (uint(cc.a) + 1) * 255;
		uint kk_ = 255 - c.a;
		uint k2_ = (uint(c.a) + 1) * kk;
		uint znam = 65535 - kk * kk_;
		c.b = (c.b * k2_ + cc.b * k2) / znam;
		c.g = (c.g * k2_ + cc.g * k2) / znam;
		c.r = (c.r * k2_ + cc.r * k2) / znam;
		c.a = 255 - ((kk_ * kk) >> 8);
	}

	static void mixmix2(const color cc1, const color cc2, color& c)
	{
		uint kk = 256 - cc1.a - cc2.a;
		uint k1 = uint(cc1.a) * 255;
		uint k2 = uint(cc2.a) * 255;
		uint kk_ = 255 - c.a;
		uint k0 = (uint(c.a) + 1) * kk;
		uint znam = 65535 - kk * kk_;
		c.b = (c.b * k0 + cc1.b * k1 + cc2.b * k2) / znam;
		c.g = (c.g * k0 + cc1.g * k1 + cc2.g * k2) / znam;
		c.r = (c.r * k0 + cc1.r * k1 + cc2.r * k2) / znam;
		c.a = 255 - ((kk_ * kk) >> 8);
	}
};

struct font_style
{
	long h = 10;
	long id = 0;
	bool bold = false;

	bool operator==(const font_style& o) const = default;
};

struct font_hash
{
	size_t operator()(const font_style& f) const noexcept
	{
		return size_t(f.h) * 64 + size_t(f.id) * 2 + f.bold;
	}
};

HFONT get_font_from_cache(i64 h, int id, bool bold)
{
	constexpr double ln_font_step = 0.05; // ~ln(1.05);
	static std::unordered_map<font_style, HFONT, font_hash> fonts;

	if (h < 3 || h > 1000 || id < 0 || id >= font_names.size()) return nullptr;

	font_style fs{.id = id, .bold = bold};
	fs.h = long(exp(i64(log(h) / ln_font_step) * ln_font_step) + 0.5);

	auto f = fonts.find(fs);
	if (f != fonts.end()) return f->second;

	LOGFONT font{
		.lfHeight = -fs.h,							// высота шрифта или символа
		.lfWidth = 0,								// средняя ширина символов в шрифте
		.lfEscapement = 0,							// угол, между вектором наклона и осью X устройства
		.lfOrientation = 0,							// угол, между основной линией каждого символа и осью X устройства
		.lfWeight = fs.bold ? FW_BOLD : FW_NORMAL,	// толщина шрифта в диапазоне от 0 до 1000
		.lfItalic = 0,								// курсивный шрифт
		.lfUnderline = 0,							// подчеркнутый шрифт
		.lfStrikeOut = 0,							// зачеркнутый шрифт
		.lfCharSet = DEFAULT_CHARSET,				// набор символов
		.lfOutPrecision = 0,						// точность вывода
		.lfClipPrecision = 0,						// точность отсечения
		.lfQuality = 0,								// качество вывода
		.lfPitchAndFamily = 0,						// ширина символов и семейство шрифта
	};
	memcpy(font.lfFaceName, font_names[fs.id].c_str(), font_names[fs.id].size() * 2 + 2); // название шрифта

	auto hfont = CreateFontIndirect(&font);
	if (hfont) fonts.emplace(fs, hfont);

	return hfont;
}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

picture::picture(size2i s)
{
	drawing_rect = size = s;
	if (!size.empty()) data = new color[size.count()];
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

void picture::set_drawing_rect(const recti& q)
{
	drawing_rect = q & size;
}

void picture::clear(color c)
{
	if (drawing_rect != size)
		fill_rectangle(size, c, true);
	else
	{
		transparent = c.a != 0xff;
		std::fill_n(data, size.count(), c);
	}
}

template<class Blender> void picture::vertical_line(i64 x, intervali y, color c)
{
	if (c.a == 0) return;
	if (!drawing_rect.x.test(x)) return;
	y &= drawing_rect.y;
	if (y.empty()) return;
	if (std::is_same<blend_replace, Blender>::value) transparent |= c.a != 0xff;
	Blender cmix(c);
	color* cс_max = &pixel(x, y.max);
	for (auto cc = &pixel(x, y.min); cc < cс_max; cc += size.x) cmix.blend(*cc);
}

template<class Blender> void picture::fill_rectangle_impl(recti r, color c)
{
	if (r.x.length() == 1)
	{
		vertical_line<Blender>(r.x.min, r.y, c);
		return;
	}
	Blender cmix(c);
	for (i64 y = r.y.min; y < r.y.max; y++)
	{
		color* cс_max = &pixel(r.x.max, y);
		for (auto cc = &pixel(r.x.min, y); cc < cс_max; cc++) cmix.blend(*cc);
	}
}

void picture::fill_rectangle(recti r, color c, bool rep)
{
	if (c.a == 0 && !rep) return;
	r &= drawing_rect;
	if (r.empty()) return;
	if (rep || c.a == 0xff)
	{
		transparent |= c.a != 0xff;
		fill_rectangle_impl<blend_replace>(r, c);
	}
	else if (transparent)
		fill_rectangle_impl<blend_over_alpha>(r, c);
	else
		fill_rectangle_impl<blend_over_opaque>(r, c);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bitmap::bitmap(size2i s) : picture({0,0}), hdc(CreateCompatibleDC(nullptr))
{
	if (hdc) resize(s);
}

bitmap::bitmap() : picture({ 0,0 }), hdc(CreateCompatibleDC(nullptr))
{
}

bitmap::~bitmap()
{
	DeleteDC(hdc);
	DeleteObject(hbm);
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
		if (!SelectObject(hdc, new_hbm))
		{
			DeleteObject(new_hbm);
			return false;
		}
	}

	DeleteObject(hbm);

	hbm = new_hbm;
	data = new_data;
	size = wh;
	transparent = false;
	set_drawing_rect(size);

	return true;
}

bitmap::bitmap(const bitmap& copy) : picture({0,0})
{
	hdc = CreateCompatibleDC(nullptr);

	if (!hdc) return;
	resize(copy.size);

	transparent = copy.transparent;
	set_drawing_rect(copy.drawing_rect);

	memcpy(data, copy.data, size.count() * sizeof(color));
}

bitmap::bitmap(bitmap&& move) noexcept :picture({0,0}), hdc(move.hdc), hbm(move.hbm)
{
	data = move.data;
	size = move.size;
	transparent = move.transparent;
	drawing_rect = move.drawing_rect;

	move.hdc = nullptr;
	move.hbm = nullptr;
	move.data = nullptr;
	move.drawing_rect = move.size = { 0,0 };
}

bitmap& bitmap::operator=(const bitmap& copy)
{
	if (&copy == this) return *this;

	if (!hdc)
	{
		hdc = CreateCompatibleDC(nullptr);
		if (!hdc) return *this;
	}

	resize(copy.size);

	transparent = copy.transparent;
	set_drawing_rect(copy.drawing_rect);

	if (data && copy.data && size == copy.size) memcpy(data, copy.data, size.count() * sizeof(color));

	return *this;
}

bitmap& bitmap::operator=(bitmap&& move) noexcept
{
	if (&move == this) return *this;

	DeleteDC(hdc);
	DeleteObject(hbm);

	hdc = move.hdc;
	hbm = move.hbm;

	data = move.data;
	size = move.size;
	transparent = move.transparent;
	drawing_rect = move.drawing_rect;

	move.hdc = nullptr;
	move.hbm = nullptr;
	move.data = nullptr;
	move.drawing_rect = move.size = { 0,0 };

	return *this;
}

void bitmap::set_drawing_rect(const recti& r)
{
	picture::set_drawing_rect(r);

	if (drawing_rect == size)
		SelectClipRgn(hdc, nullptr);
	else
	{
		HRGN rgn = CreateRectRgn((int)drawing_rect.x.min, (int)drawing_rect.y.min,
	                             (int)drawing_rect.x.max, (int)drawing_rect.y.max);
		SelectClipRgn(hdc, rgn);
		DeleteObject(rgn);
	}
}

void bitmap::text(ixy p, std::wstring_view s, int h, color tc, color bc, bool bold, int font_id)
{
	if ( tc.a == 0 || p.y > drawing_rect.y.max || p.x > drawing_rect.x.max || p.y + h < drawing_rect.y.min) return;
	auto text_area = size_text(s, h, bold, font_id).move(p) & drawing_rect; // шрифт устанавливается в функции size_text
	if (text_area.empty()) return;

	tc = { .b = tc.r, .g = tc.g, .r = tc.b, .a = 0 };
	bc = { .b = bc.r, .g = bc.g, .r = bc.b, .a = bc.a };
	SetTextColor(hdc, tc);
	if (bc.a == 0xff) SetBkColor(hdc, bc.c & 0xffffff);
	SetBkMode(hdc, (bc.a == 0xff) ? OPAQUE : TRANSPARENT);

	TextOutW(hdc, (int)p.x, (int)p.y, s.data(), (int)s.size());
}

size2i bitmap::size_text(std::wstring_view s, int h, bool bold, int font_id)
{
	if (s.empty()) return {};
	auto hfont = get_font_from_cache(h, font_id, bold);
	if (!hfont) return {};
	SelectObject(hdc, hfont);
	SIZE a;
	GetTextExtentPoint32W(hdc, s.data(), (int)s.size(), &a);
	return { a.cx, a.cy };
}
