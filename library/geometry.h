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
	ixy(i64 x_, i64 y_) : x{ x_ }, y{ y_ } {}
}; 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct xy
{
	double x;
	double y;

	xy() = default;
	xy(double x_, double y_) : x{ x_ }, y{ y_ } {}
	xy(const ixy b) : x{ double(b.x) }, y{ double(b.y) } {}

	xy operator-() const { return { -x,  -y }; }

	xy& operator+=(const xy b) { x += b.x; y += b.y; return *this; }

	xy operator*(double b) const { return { x * b, y * b }; }
	xy operator-(const xy b) const { return { x - b.x, y - b.y }; }
	xy operator+(const xy b) const { return { x + b.x, y + b.y }; }
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
	xy center() const { return { x * 0.5, y * 0.5 }; }
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
	double lerp(double t) const { return min + 0.5 + (max - min - 1) * t; }
	double center() const { return min + (max - min) * 0.5; }
	intervali& expand(i64 b);
	intervali expanded(i64 b) const;
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
	bool operator==(const recti&) const;
	bool operator!=(const recti& b) const { return !(*this == b); }

	bool empty() const;
	bool test(ixy b) const;
	recti expanded(i64 b) const; // расширенная область во все стороны на b
	recti& expand(i64 b);
	xy center() const { return { x.center(), y.center() }; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct interval // [...])
{
	double min = 1.0;
	double max = 0.0;
	bool   right_closed = true;

	interval& operator&=(const interval& b);
	interval& operator|=(const interval& b);

	bool operator==(const interval& b) const;
	bool operator<=(const interval& b) const;
	bool touches_boundary(const interval& b) const;

	bool empty() const { return (max < min) || (max == min && !right_closed); }
	bool test(double b) const; // принадлежит ли точка области

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct rect
{
	interval x;
	interval y;

	rect() = default;
	rect(size2i b) : x{ 0.0, double(b.x), false }, y{ 0.0, double(b.y), false } {}
	rect(interval x_, interval y_) : x{ x_ }, y{ y_ } {}

	rect& operator&=(const rect& b);
	rect& operator|=(const rect& b);

	rect operator&(const rect& b) const { return rect(*this) &= b; }

	bool operator==(const rect& b) const;
	bool operator<=(const rect& b) const;
	bool operator!=(const rect& b) const { return !(*this == b); }
	bool operator<(const rect& b) const { return (*this <= b) && !(*this == b); }
	bool touches_boundary(const rect& b) const;

	bool empty() const;
	bool test(xy b) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct transform
{
	double scale = 1.0;
	xy offset = { 0.0, 0.0 };

	rect operator()(const rect& b) const;
	xy operator()(const xy b) const;

	transform& operator*=(const transform& b);

	transform inverse() const;
};