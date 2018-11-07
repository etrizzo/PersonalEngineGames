#include "OBB2.hpp"

OBB2::OBB2(const Vector2 & centerPoint, const Vector2 & right, const Vector2 & halfDims)
{
	center = centerPoint;
	halfDimensions = halfDims;
	normalizedRight = right.GetNormalized();
}

OBB2::OBB2(const Vector2 & centerPoint, const Vector2 & right, float halfSizeX, float halfSizeY)
{
	center = centerPoint;
	halfDimensions = Vector2(halfSizeX, halfSizeY);
	normalizedRight = right.GetNormalized();
}

bool OBB2::operator==(const OBB2 & compare) const
{
	return (center == compare.center) && (halfDimensions == compare.halfDimensions) && (normalizedRight == compare.normalizedRight);
}

bool OBB2::operator!=(const OBB2 & compare) const
{
	return (center != compare.center) || (halfDimensions != compare.halfDimensions) || (normalizedRight != compare.normalizedRight);
}

Vector2 OBB2::GetUp() const
{
	return Vector2(-normalizedRight.y, normalizedRight.x);
}

Vector2 OBB2::GetLocalMins() const
{
	return center - halfDimensions;
}

Vector2 OBB2::GetLocalMaxs() const
{
	return center + halfDimensions;
}

Vector2 OBB2::GetWorldMins() const
{
	Vector2 halfLocalOffsetI = halfDimensions.x * normalizedRight;
	Vector2 halfLocalOffsetJ = halfDimensions.y * GetUp();
	Vector2 halfLocalOffset = halfLocalOffsetI + halfLocalOffsetJ;
	return center - halfLocalOffset;
}

Vector2 OBB2::GetWorldMaxs() const
{
	Vector2 halfLocalOffsetI = halfDimensions.x * normalizedRight;
	Vector2 halfLocalOffsetJ = halfDimensions.y * GetUp();
	Vector2 halfLocalOffset = halfLocalOffsetI + halfLocalOffsetJ;
	return center + halfLocalOffset;
}

Vector2 OBB2::GetWorldTopLeft() const
{
	Vector2 halfLocalOffsetI = halfDimensions.x * normalizedRight * -1.f;		//negative x
	Vector2 halfLocalOffsetJ = halfDimensions.y * GetUp();			//positive y
	Vector2 halfLocalOffset = halfLocalOffsetI + halfLocalOffsetJ;
	return center + halfLocalOffset;
}

Vector2 OBB2::GetWorldBottomRight() const
{
	Vector2 halfLocalOffsetI = halfDimensions.x * normalizedRight ;		//positive x
	Vector2 halfLocalOffsetJ = halfDimensions.y * GetUp() * -1.f;			//negative y
	Vector2 halfLocalOffset = halfLocalOffsetI + halfLocalOffsetJ;
	return center + halfLocalOffset;
}

Vector2 OBB2::GetScaledRight() const
{
	return halfDimensions.x * normalizedRight * 2.f;
}

Vector2 OBB2::GetScaledUp() const
{
	return  halfDimensions.y * GetUp() * 2.f;
}

bool OBB2::IsPointInside(float x, float y) const
{
	return IsPointInside(Vector2(x, y));
}

bool OBB2::IsPointInside(const Vector2 & point) const
{
	Vector2 offset = point - center;
	Vector2 projectedOffset = GetTransformedIntoBasis(offset, normalizedRight, GetUp());
	Vector2 localPoint = center + projectedOffset;
	Vector2 localMins = GetLocalMins();
	Vector2 localMaxs = GetLocalMaxs();

	if (localPoint.x <= localMaxs.x && localPoint.x >= localMins.x){
		if (localPoint.y <= localMaxs.y && localPoint.y>= localMins.y){
			return true;
		}
	}
	return false;
}

Vector2 OBB2::GetDimensions() const
{
	return halfDimensions * 2.f;
}

float OBB2::GetWidth() const
{
	return halfDimensions.x * 2.f;
}

float OBB2::GetHeight() const
{
	return halfDimensions.y * 2.f;
}

float OBB2::GetAspect() const
{
	return (GetWidth() / GetHeight());
}
