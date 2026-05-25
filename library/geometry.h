#pragma once

#include "base.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct size2i // [0...x), [0...y)
{
	i64 x = 0;
	i64 y = 0;

	bool empty()  const { return (x <= 0) || (y <= 0); }
	i64  count() const { return empty() ? 0 : x * y; }
	bool operator==(const size2i s) const { return ((x == s.x) && (y == s.y)) || (empty() && s.empty()); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rangei // [...)
{
	i64 min = 0;
	i64 max = 0;

	bool empty()  const { return (max <= min); }
	i64  length() const { return (min < max) ? (max - min) : 0; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct rect2i
{
	rangei x, y; // [...) [...)

	rect2i() = default;
	rect2i(size2i size) : x{ 0LL, size.x }, y{ 0LL, size.y } {}

	bool empty() const;
};
