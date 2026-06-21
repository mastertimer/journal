#pragma once

#include "base.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct interval;
struct rect;
struct recti;
struct xy;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline i64 floori(double x)
{
	i64 res = (i64)x;
	return res - (x < 0 && x != res);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ixy // индекс, номер
{
	i64 x;
	i64 y;

	ixy() = default;
	inline ixy(xy b);
}; 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct xy
{
	double x;
	double y;

	xy& operator+=(const xy b) { x += b.x; y += b.y; return *this; }

	xy operator*(double b) const { return { x * b, y * b }; }
};

ixy::ixy(xy b) : x{ floori(b.x) }, y{ floori(b.y) } {};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct size2i // [0...x), [0...y)
{
	i64 x = 0;
	i64 y = 0;

	bool empty()  const { return (x <= 0) || (y <= 0); }
	i64  count() const { return empty() ? 0 : x * y; }
	bool operator==(const size2i s) const { return ((x == s.x) && (y == s.y)) || (empty() && s.empty()); }
	recti move(ixy d) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct intervali // [...)
{
	i64 min = 0;
	i64 max = 0;

	intervali() = default;
	intervali(i64 min_, i64 max_) : min(min_), max(max_) {}
	intervali(interval b);

	intervali& operator&=(const intervali b)
	{
		if (b.min > min) min = b.min;
		if (b.max < max) max = b.max;
		return *this;
	}
	intervali operator&(const intervali b) const { return intervali(*this) &= b; }

	bool empty()  const { return (max <= min); }
	i64  length() const { return (min < max) ? (max - min) : 0; }
	bool test(i64 x) const { return (x >= min) && (x < max); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct recti
{
	intervali x, y; // [...) [...)

	recti() = default;
	recti(size2i size) : x{ 0LL, size.x }, y{ 0LL, size.y } {}
	recti(intervali x_, intervali y_) : x{ x_ }, y{ y_ } {}
	recti(const rect& r);

	recti& operator&=(const recti&);
	recti operator&(const recti&) const;
	bool operator!=(const recti&) const;
	bool operator==(const recti&) const;

	bool empty() const;
	bool test(ixy b) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct interval // [...])
{
	double min = 1.0;
	double max = 0.0;
	bool   right_closed = true;

	interval& operator&=(const interval& b);
	interval& operator|=(const interval& b);

	bool operator<=(const interval& b) const;

	bool empty() const { return (max < min) || (max == min && !right_closed); }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct rect
{
	interval x;
	interval y;

	rect() = default;
	rect(size2i b) : x{ 0.0, double(b.x), false }, y{ 0.0, double(b.y), false } {}
	rect(interval x_, interval y_);

	rect& operator&=(const rect& b);
	rect& operator|=(const rect& b);

	rect operator&(rect b) const { b &= *this; return b; }

	bool operator<=(const rect& b) const;

	bool empty() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct transform
{
	double scale = 1.0;
	xy offset = { 0.0, 0.0 };

	rect operator()(const rect& b) const;

	transform& operator*=(const transform& b);
};