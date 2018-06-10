#include "AABB2.hpp"
#include "Engine/Core/StringUtils.hpp"

const AABB2 AABB2::ZERO_TO_ONE = AABB2(0.f,0.f,1.f,1.f);

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
{
	mins = Vector2(minX, minY);
	maxs = Vector2(maxX, maxY);
}

AABB2::AABB2(const Vector2 & minsXY, const Vector2 & maxsXY)
{
	mins = Vector2(minsXY);
	maxs = Vector2(maxsXY);
}

AABB2::AABB2(const Vector2 & center, float radiusX, float radiusY)
{
	mins = Vector2(center.x - radiusX, center.y - radiusY);
	maxs = Vector2(center.x + radiusX, center.y + radiusY);
}

bool AABB2::operator==(const AABB2 & compare) const
{
	return ((mins == compare.mins) && (maxs == compare.maxs));
}

bool AABB2::operator!=(const AABB2 & compare) const
{
	return ((mins != compare.mins) || (maxs != compare.maxs));
}

void AABB2::StretchToIncludePoint(float x, float y)
{
	if (x < mins.x){
		mins.x = x;
		//float diff = mins.x - x;
		//mins.x-=diff;
		//maxs.x+=diff;
	}
	if (y < mins.y){
		mins.y = y;
		//float diff = mins.y - y;
		//mins.y-=diff;
		//maxs.y+=diff;
	}
	if (x > maxs.x){
		maxs.x = x;
		//float diff = x - maxs.x;
		//maxs.x+=diff;
		//mins.x-=diff;
	}
	if (y > maxs.y){
		maxs.y = y;
		//float diff = y - maxs.y;
		//maxs.y+=diff;
		//mins.y-=diff;
	}
}

void AABB2::StretchToIncludePoint(const Vector2& point)
{
	if (point.x < mins.x){
		float diff = mins.x -point.x;
		mins.x-=diff;
		//maxs.x+=diff;
	}
	if (point.y < mins.y){
		float diff = mins.y - point.y;
		mins.y-=diff;
		//maxs.y+=diff;
	}
	if (point.x > maxs.x){
		float diff = point.x - maxs.x;
		maxs.x+=diff;
		//mins.x-=diff;
	}
	if (point.y > maxs.y){
		float diff = point.y - maxs.y;
		maxs.y+=diff;
		//mins.y-=diff;
	}
}

void AABB2::AddPaddingToSides(float xPaddingRadius, float yPaddingRadius)
{
	mins.x-=xPaddingRadius;
	maxs.x+=xPaddingRadius;
	mins.y-=yPaddingRadius;
	maxs.y+=yPaddingRadius;
}

void AABB2::Translate(const Vector2 & translation)
{
	mins+=translation;
	maxs+=translation;
}

void AABB2::Translate(float translationX, float translationY)
{
	mins.x+=translationX;
	mins.y+=translationY;
	maxs.x+=translationX;
	maxs.y+=translationY;
}

void AABB2::UniformScaleFromCenter(float scale)
{
	float newWidth = GetWidth() * scale;
	float newHeight = GetHeight() * scale;
	Vector2 center = GetCenter();
	Vector2 halfDims = Vector2(newWidth * .5f, newHeight * .5f);
	mins = center - halfDims;
	maxs = center + halfDims;
}

void AABB2::TrimToSquare()
{
	Vector2 center = GetCenter();
	float w = GetWidth();
	float h = GetHeight();

	float halfHeight = h * .5f;
	float halfWidth = w * .5f;
	if (h > w){
		//shrink in y direction
		mins = Vector2(mins.x, center.y - halfWidth);
		maxs = Vector2(maxs.x, center.y + halfWidth);
	} else if (h < w){
		//shrink in x direction
		mins = Vector2(center.x - halfHeight, mins.y);
		maxs = Vector2(center.x + halfHeight, maxs.y);
	}
}

void AABB2::ExpandToSquare()
{
	Vector2 center = GetCenter();
	float w = GetWidth();
	float h = GetHeight();

	float halfHeight = h * .5f;
	float halfWidth = w * .5f;
	if (h > w){
		//expand in x direction
		mins = Vector2(center.x - halfHeight, mins.y);
		maxs = Vector2(center.x + halfHeight, maxs.y);
	} else if (h < w){
		//expand in y direction
		mins = Vector2(mins.x, center.y - halfWidth);
		maxs = Vector2(maxs.y, center.y + halfWidth);
	}
}

void AABB2::SetFromText(const char * text)
{
	Strings floats = Strings();
	Split((std::string) text, ',', floats);
	std::string minsText = floats[0] + "," + floats[1];
	std::string maxsText = floats[2] + "," + floats[3];
	mins.SetFromText(minsText.c_str());
	maxs.SetFromText(maxsText.c_str());
}

void AABB2::SplitAABB2Vertical(float xDivide, AABB2 &outLeft, AABB2 &outRight)
{
	float boxWidth = GetWidth();
	Vector2 splitLineBottom = mins + Vector2(boxWidth * xDivide, 0.f);
	Vector2 splitLineTop = mins + Vector2(boxWidth * xDivide, GetHeight());
	outLeft = AABB2(mins, splitLineTop);
	outRight = AABB2(splitLineBottom, maxs);
}

void AABB2::SplitAABB2Horizontal(float yDivide, AABB2 &outTop, AABB2 &outBottom)
{
	float boxHeight = GetHeight();
	Vector2 splitLineLeft = mins + Vector2(0.f, boxHeight * yDivide);
	Vector2 splitLineRight = mins + Vector2(GetWidth(), boxHeight * yDivide);

	outBottom = AABB2(mins, splitLineRight);
	outTop = AABB2(splitLineLeft, maxs);
}

AABB2 AABB2::GetPercentageBox(float xPercentMin, float yPercentMin, float xPercentMax, float yPercentMax)
{
	return GetPercentageBox(Vector2(xPercentMin, yPercentMin), Vector2(xPercentMax, yPercentMax));
}

AABB2 AABB2::GetPercentageBox(Vector2 minsPercentages, Vector2 maxsPercentages)
{
	Vector2 dims = GetDimensions();
	Vector2 minsOffset = Vector2(dims.x * minsPercentages.x, dims.y * minsPercentages.y);
	Vector2 maxsOffset = Vector2(dims.x * maxsPercentages.x, dims.y * maxsPercentages.y);
	AABB2 newBox = AABB2(mins + minsOffset, mins + maxsOffset );
	return newBox;
}

Vector2 AABB2::GetPercentageOfPoint(Vector2 point)
{
	float width = GetWidth();
	float height = GetHeight();
	Vector2 offset = point - mins;
	if (width != 0 && height != 0){
		return Vector2 (offset.x / width, offset.y / height);
	}
	return Vector2::ZERO;
}

bool AABB2::IsPointInside(float x, float y) const
{
	if (x <= maxs.x && x >= mins.x){
		if (y <= maxs.y && y>= mins.y){
			return true;
		}
	}
	return false;
}

bool AABB2::IsPointInside(const Vector2 & point) const
{
	if (point.x <= maxs.x && point.x >= mins.x){
		if (point.y <= maxs.y && point.y>= mins.y){
			return true;
		}
	}
	return false;
}

Vector2 AABB2::GetDimensions() const
{
	return Vector2(maxs.x - mins.x, maxs.y-mins.y);
}

Vector2 AABB2::GetCenter() const
{
	return Vector2((maxs.x+mins.x)/2.f, (maxs.y+mins.y)/2.f);
}

float AABB2::GetWidth() const
{
	return maxs.x - mins.x;
}

float AABB2::GetHeight() const
{
	return maxs.y - mins.y;
}

Vector2 AABB2::GetRandomPointInBox() const
{
	float x = GetRandomFloatInRange(mins.x, maxs.x);
	float y = GetRandomFloatInRange(mins.y, maxs.y);
	return Vector2(x,y);
}

float AABB2::GetAspect() const
{
	return (GetWidth() / GetHeight());
}

void AABB2::operator+=(const Vector2 & translation)
{
	maxs+=translation;
	mins+=translation;
}

void AABB2::operator-=(const Vector2 & antiTranslation)
{
	maxs-=antiTranslation;
	mins-=antiTranslation;
}

AABB2 AABB2::operator+(const Vector2 & translation) const
{
	return AABB2(mins+translation, maxs+translation );
}

AABB2 AABB2::operator-(const Vector2 & antiTranslation) const
{
	return AABB2(mins - antiTranslation, maxs-antiTranslation);
}

bool DoAABBsOverlap(const AABB2 & a, const AABB2 & b)
{
	if (GetDistance(a.mins, b.mins) < GetDistance (a.mins, a.maxs)){
		return true;
	}
	if (GetDistance(b.mins, a.mins) < GetDistance (b.mins, b.maxs)){
		return true;
	}
	return false;
}

AABB2 Interpolate(const AABB2 & start, const AABB2 & end, float fractionTowardEnd)
{
	Vector2 mins = Interpolate(start.mins, end.mins, fractionTowardEnd);
	Vector2 maxs = Interpolate(start.maxs, end.maxs, fractionTowardEnd);
	return AABB2(mins, maxs);
}

Plane::Plane(const Vector3& center, const Vector3& up, const Vector3& right, const Vector2 & size)
{
	m_center = center;
	m_up = up;
	m_right = right;
	m_size = size;

}

Plane::Plane(Vector3 nearBottomLeft, Vector3 nearBottomRight, Vector3 farTopRight, Vector3 farTopLeft)
{
	m_nearBottomLeft	= nearBottomLeft;
	m_nearBottomRight	= nearBottomRight;
	m_farTopRight		= farTopRight;
	m_farTopLeft		= farTopLeft;
}

Vector3 Plane::GetCenter() const
{
	//this should be fine right?
	return m_center;
}

AABB2 Plane::GetAABB2() const
{
	Vector3 nearBottomLeft	= m_center - (m_right * m_size.x) - (m_up * m_size.y);
	Vector3 farTopRight		= m_center + (m_right * m_size.x) + (m_up * m_size.y);
	return AABB2(nearBottomLeft.XY(), farTopRight.XY());
}
