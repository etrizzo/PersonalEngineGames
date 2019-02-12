#include "Engine\Math\IntVector2.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <string>

const IntVector2 IntVector2::NORTH = IntVector2(0, 1);
const IntVector2 IntVector2::SOUTH = IntVector2(0,-1);
const IntVector2 IntVector2::EAST = IntVector2( 1,0);
const IntVector2 IntVector2::WEST = IntVector2(-1,0);

const IntVector2 IntVector2::INVALID_INDEX = IntVector2(-1,-1);

const IntVector2 IntVector2::AWAY_LEFT		= IntVector2(-1, 1);
const IntVector2 IntVector2::AWAY_RIGHT		= IntVector2( 1, 1);
const IntVector2 IntVector2::TOWARD_LEFT	= IntVector2(-1,-1);
const IntVector2 IntVector2::TOWARD_RIGHT	= IntVector2( 1,-1);

//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2( const IntVector2& copy )
	: x( 99 )
	, y( 99 )
{
	x = copy.x;
	y = copy.y;
}



//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2( int initialX, int initialY )
	: x( 99 )
	, y( 99 )
{
	x = initialX;
	y = initialY;
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator + ( const IntVector2& vecToAdd ) const
{
	return IntVector2( x + vecToAdd.x, y + vecToAdd.y ); 
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator-( const IntVector2& vecToSubtract ) const
{
	return IntVector2( x - vecToSubtract.x, y - vecToSubtract.y); 
}


bool IntVector2::operator<(const IntVector2& compare) const
{
	if (y < compare.y){
		return true;
	}
	if (y == compare.y){
		if (x < compare.x){
			return true;
		} else {
			return false;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator*( int uniformScale ) const
{
	return IntVector2( x*uniformScale, y*uniformScale ); 
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator/( int inverseScale ) const
{
	return IntVector2( x * (1 /inverseScale), y * (1 / inverseScale) ); 
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator+=( const IntVector2& vecToAdd )
{
	x = x + vecToAdd.x; 
	y = y + vecToAdd.y; 
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator-=( const IntVector2& vecToSubtract )
{
	x = x - vecToSubtract.x; 
	y = y - vecToSubtract.y; 
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator*=( const int uniformScale )
{
	x = x * uniformScale;
	y = y * uniformScale;
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator/=( const int uniformDivisor )
{
	x = x * (1/uniformDivisor);
	y = y * (1/uniformDivisor);
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator=( const IntVector2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const IntVector2 operator*( int uniformScale, const IntVector2& vecToScale )
{
	return (vecToScale * uniformScale); // #MP1Fixme
}

const IntVector2 Interpolate(const IntVector2 & start, const IntVector2 & end, float fractionTowardEnd)
{
	int x = Interpolate(start.x, end.x, fractionTowardEnd);
	int y = Interpolate(start.y, end.y, fractionTowardEnd);
	return IntVector2(x, y);
}


//-----------------------------------------------------------------------------------------------
bool IntVector2::operator==( const IntVector2& compare ) const
{
	if ((x == compare.x) && (y == compare.y)){
		return true;
	}
	return false; 
}


//-----------------------------------------------------------------------------------------------
bool IntVector2::operator!=( const IntVector2& compare ) const
{
	if ((x != compare.x) || (y != compare.y)){
		return true;
	}
	return false; 
}

 Vector2 IntVector2::GetVector2() const
{
	return Vector2((float) x, (float) y);
}

 void IntVector2::SetFromText(const char * text)
 {
	 std::string stringText = (std::string) text;
	 unsigned int commaIndex = (unsigned int) stringText.find(',');
	 if (commaIndex != std::string::npos){
		 std::string xString = stringText.substr(0, commaIndex);
		 std::string yString = stringText.substr(commaIndex+1);
		 x = atoi(xString.c_str());
		 y = atoi(yString.c_str());
	 }
 }
