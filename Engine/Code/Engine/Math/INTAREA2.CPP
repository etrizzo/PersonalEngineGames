#include "AreaMask.hpp"

bool AreaMask::IsOnEdge(int x, int y) const
{
	return GetDistanceFromEdge(x,y) <= 1.f;
}

AreaMask * AreaMask::GetSubArea(FloatRange centerRange, FloatRange sizeRange) const
{
	return nullptr;
}
