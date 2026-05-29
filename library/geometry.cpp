#include "geometry.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

intervali::intervali(interval b) : min{ (i64)b.min }, max{ (i64)b.max }
{
	if (b.max < b.min) return;
	if ((b.min < 0) && (min != b.min)) min--;
	max += (max == b.max) ? b.right_closed : (b.max > 0);
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

void recti::operator&=(const recti& b)
{
	x &= b.x;
	y &= b.y;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void interval::operator&=(const interval& b)
{
	if (b.min > min) min = b.min;
	if (b.max < max)
	{
		max = b.max;
		right_closed = b.right_closed;
	}
	else
		if (b.max == max) right_closed &= b.right_closed;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool rect::empty() const
{
	return x.empty() || y.empty();
}

void rect::operator&=(const rect& b)
{
	x &= b.x;
	y &= b.y;
}
