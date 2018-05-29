#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <iostream>


const IntVector3 IntVector3::ZERO	= IntVector3(0,0,0);
const IntVector3 IntVector3::ONE	= IntVector3(1,1,1);
const IntVector3 IntVector3::UP		= IntVector3(0,1,0);

//-----------------------------------------------------------------------------------------------
IntVector3::IntVector3( const IntVector3& copy )
	: x( 99 )
	, y( 99 )
	, z( 99 )
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}



//-----------------------------------------------------------------------------------------------
IntVector3::IntVector3( int initialX, int initialY , int initialZ)
	: x( 99 )
	, y( 99 )
	, z( 99 )
{
	x = initialX;
	y = initialY;
	z = initialZ;
}


//-----------------------------------------------------------------------------------------------
const IntVector3 IntVector3::operator + ( const IntVector3& vecToAdd ) const
{
	return IntVector3( x + vecToAdd.x, y + vecToAdd.y , z + vecToAdd.z); 
}


//-----------------------------------------------------------------------------------------------
const IntVector3 IntVector3::operator-( const IntVector3& vecToSubtract ) const
{
	return IntVector3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z); 
}


//-----------------------------------------------------------------------------------------------
const IntVector3 IntVector3::operator*( int uniformScale ) const
{
	return IntVector3( x*uniformScale, y*uniformScale , z*uniformScale); 
}




//-----------------------------------------------------------------------------------------------
void IntVector3::operator+=( const IntVector3& vecToAdd )
{
	x = x + vecToAdd.x; 
	y = y + vecToAdd.y; 
	z = z + vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void IntVector3::operator-=( const IntVector3& vecToSubtract )
{
	x = x - vecToSubtract.x; 
	y = y - vecToSubtract.y; 
	z = z - vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void IntVector3::operator*=( const int uniformScale )
{
	x = x * uniformScale;
	y = y * uniformScale;
	z = z * uniformScale;
}



//-----------------------------------------------------------------------------------------------
void IntVector3::operator=( const IntVector3& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
const IntVector3 operator*( int uniformScale, const IntVector3& vecToScale )
{
	return (vecToScale * uniformScale); 
}


//-----------------------------------------------------------------------------------------------
bool IntVector3::operator==( const IntVector3& compare ) const
{
	if ((x == compare.x) && (y == compare.y) && (z == compare.z)){
		return true;
	}
	return false; 
}


//-----------------------------------------------------------------------------------------------
bool IntVector3::operator!=( const IntVector3& compare ) const
{
	if ((x != compare.x) || (y != compare.y) || (z != compare.z)){
		return true;
	}
	return false; 
}

float IntVector3::GetLength() const
{
	return sqrtf((float) ( x * x) + (float) (y * y) + (float) (z * z));
}


float IntVector3::GetLengthSquared() const
{
	return ((float)(x*x) + (float)(y*y) + (float)(z * z));
}


Vector3 IntVector3::GetNormalized() const
{
	return Vector3((float) x,(float) y, (float) z)/GetLength();
}

float IntVector3::GetOrientationDegrees() const
{
	Vector3 normal = GetNormalized();

	float deg = Atan2Degreesf(normal.y, normal.x);

	return deg;
}

void IntVector3::SetFromText(const char * text)
{
	std::string stringText = (std::string) text;
	unsigned int comma1Index = (unsigned int) stringText.find(',');
	if (comma1Index != std::string::npos){
		std::string xString = stringText.substr(0, comma1Index);
		unsigned int comma2Index = (unsigned int) stringText.find(',', comma1Index);
		std::string yString = stringText.substr(comma1Index+1, comma2Index);
		std::string zString = "0";
		if (comma2Index != std::string::npos){
			zString = stringText.substr(comma2Index + 1);
		}
		x = atoi(xString.c_str());
		y = atoi(yString.c_str());
		z = atoi(zString.c_str());
	}


}

Vector3 IntVector3::GetVector3() const
{
	return Vector3((float) x, (float) y, (float) z);
}



float GetDistance(const IntVector3 & a, const IntVector3 & b)
{
	float dx = (float)a.x - (float)b.x;
	float dy = (float)a.y - (float)b.y;
	float dz = (float)a.z - (float)b.z;
	return sqrtf((dx * dx) + (dy*dy) + (dz * dz));
}

float GetDistanceSquared(const IntVector3 & a, const IntVector3 & b)
{
	float dx = (float)a.x - (float)b.x;
	float dy = (float)a.y - (float)b.y;
	float dz = (float)a.z - (float)b.z;
	return ((dx * dx) + (dy*dy) + (dz * dz));
}


const IntVector3 Interpolate(const IntVector3 & start, const IntVector3 & end, float fractionTowardEnd)
{
	int x = Interpolate(start.x, end.x, fractionTowardEnd);
	int y = Interpolate(start.y, end.y, fractionTowardEnd);
	int z = Interpolate(start.z, end.z, fractionTowardEnd);
	return IntVector3(x,y,z);
}

const IntVector3 Cross(const IntVector3 & A, const IntVector3 & B)
{
	return IntVector3((A.y * B.z) - (A.z * B.y), (A.z * B.x) - (A.x * B.z), (A.x * B.y) - (A.y * B.x));
}


