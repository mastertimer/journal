#include "geometry.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

intervali::intervali(interval b) : min{ (i64)b.min }, max{ (i64)b.max }
{
	if (b.max < b.min) return;
	if ((b.min < 0) && (min != b.min)) min--;
	max += (max == b.max) ? b.right_closed : (b.max > 0);
}

intervali& intervali::expand(i64 b)
{
	if (!empty())
	{
		min -= b;
		max += b;
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

recti size2i::move(ixy d) const
{
	return { {d.x, d.x + x}, {d.y, d.y + y} };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool recti::empty() const
{
	return x.empty() || y.empty();
}

bool recti::test(ixy b) const
{
	return x.test(b.x) && y.test(b.y);
}

recti::recti(const rect& r) : x{ r.x }, y{ r.y }
{
}

recti& recti::operator&=(const recti& b)
{
	x &= b.x;
	y &= b.y;
	return *this;
}

recti recti::operator&(const recti& b) const
{
	return { x & b.x, y & b.y };
}

bool recti::operator!=(const recti& b) const
{
	if (b.empty() && empty()) return false;
	return (x.min != b.x.min) || (y.min != b.y.min) || (x.max != b.x.max) || (y.max != b.y.max);
}

bool recti::operator==(const recti& b) const
{
	if (b.empty() && empty()) return true;
	return (x.min == b.x.min) && (y.min == b.y.min) && (x.max == b.x.max) && (y.max == b.y.max);
}

recti recti::expanded(i64 b) const
{
	recti r = *this;
	return r.expand(b);
}

recti& recti::expand(i64 b)
{
	x.expand(b);
	y.expand(b);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

interval& interval::operator&=(const interval& b)
{
	if (b.min > min) min = b.min;
	if (b.max < max)
	{
		max = b.max;
		right_closed = b.right_closed;
	}
	else
		if (b.max == max) right_closed &= b.right_closed;
	return *this;
}

interval& interval::operator|=(const interval& b)
{
	if (b.empty()) return *this;
	if (empty()) return *this = b;
	if (b.min < min) min = b.min;
	if (b.max > max)
	{
		max = b.max;
		right_closed = b.right_closed;
	}
	else if (b.max == max) right_closed |= b.right_closed;
	return *this;
}

bool interval::operator<=(const interval& b) const
{
	if (empty()) return true;
	if (b.empty()) return false;
	if (min < b.min) return false;
	if (max == b.max) return (!right_closed) || b.right_closed;
	return (max <= b.max);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool rect::empty() const
{
	return x.empty() || y.empty();
}

rect& rect::operator&=(const rect& b)
{
	x &= b.x;
	y &= b.y;
	return *this;
}

rect& rect::operator|=(const rect& b)
{
	if (b.empty()) return *this;
	if (empty()) return *this = b;
	x |= b.x;
	y |= b.y;
	return *this;
}

rect::rect(interval x_, interval y_) : x{ x_ }, y{ y_ }
{
}

bool rect::operator<=(const rect& b) const
{
	if (empty()) return true;
	if (b.empty()) return false;
	return (x <= b.x) && (y <= b.y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

transform& transform::operator*=(const transform& b)
{
	offset += b.offset * scale;
	scale *= b.scale;
	return *this;
}
rect transform::operator()(const rect& b) const
{
	if (b.empty()) return b;
	return { {b.x.min * scale + offset.x, b.x.max * scale + offset.x},
			 {b.y.min * scale + offset.y, b.y.max * scale + offset.y} };
}

