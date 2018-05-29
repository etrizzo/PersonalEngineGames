#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <iostream>


const Vector2 Vector2::ZERO = Vector2(0.f,0.f);
const Vector2 Vector2::ONE = Vector2(1.f,1.f);
const Vector2 Vector2::HALF = Vector2(.5f,.5f);


//-----------------------------------------------------------------------------------------------
Vector2::Vector2( const Vector2& copy )
	: x( 99.f )
	, y( 99.f )
{
	x = copy.x;
	y = copy.y;
}



//-----------------------------------------------------------------------------------------------
Vector2::Vector2( float initialX, float initialY )
	: x( 99.f )
	, y( 99.f )
{
	x = initialX;
	y = initialY;
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator + ( const Vector2& vecToAdd ) const
{
	return Vector2( x + vecToAdd.x, y + vecToAdd.y ); 
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator-( const Vector2& vecToSubtract ) const
{
	return Vector2( x - vecToSubtract.x, y - vecToSubtract.y); 
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator*( float uniformScale ) const
{
	return Vector2( x*uniformScale, y*uniformScale ); 
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator/( float inverseScale ) const
{
	return Vector2( x * (1 /inverseScale), y * (1 / inverseScale) ); 
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator+=( const Vector2& vecToAdd )
{
	x = x + vecToAdd.x; 
	y = y + vecToAdd.y; 
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator-=( const Vector2& vecToSubtract )
{
	x = x - vecToSubtract.x; 
	y = y - vecToSubtract.y; 
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator*=( const float uniformScale )
{
	x = x * uniformScale;
	y = y * uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator/=( const float uniformDivisor )
{
	x = x * (1/uniformDivisor);
	y = y * (1/uniformDivisor);
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator=( const Vector2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vector2 operator*( float uniformScale, const Vector2& vecToScale )
{
	return (vecToScale * uniformScale); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
bool Vector2::operator==( const Vector2& compare ) const
{
	if ((x == compare.x) && (y == compare.y)){
		return true;
	}
	return false; 
}


//-----------------------------------------------------------------------------------------------
bool Vector2::operator!=( const Vector2& compare ) const
{
	if ((x != compare.x) || (y != compare.y)){
		return true;
	}
	return false; 
}

float Vector2::GetLength() const
{
	return sqrtf((x * x) + (y * y));
}


float Vector2::GetLengthSquared() const
{
	return ((x*x) + (y*y));
}

float Vector2::NormalizeAndGetLength() 
{
	float length = this->GetLength();
	x = x/length;
	y = y/length;
	return length;

}

Vector2 Vector2::GetNormalized() const
{
	return Vector2(this->x, this->y)/this->GetLength();
}

float Vector2::GetOrientationDegrees() const
{
	Vector2 normal = GetNormalized();

	float deg = Atan2Degreesf(normal.y, normal.x);

	return deg;
}

Vector2 Vector2::MakeDirectionAtDegrees(float degrees)
{
	float xCoord = CosDegreesf(degrees);
	float yCoord = SinDegreesf(degrees);
	return Vector2(xCoord,yCoord);
}

Vector3 Vector2::GetVector3() const
{
	return Vector3(x,y,0);
}

void Vector2::SetFromText(const char * text)
{
	std::string stringText = (std::string) text;
	unsigned int commaIndex = (unsigned int) stringText.find(',');
	if (commaIndex != std::string::npos){
		std::string xString = stringText.substr(0, commaIndex);
		std::string yString = stringText.substr(commaIndex+1);
		x = (float) atof(xString.c_str());
		y = (float) atof(yString.c_str());
	}


}



float GetDistance(const Vector2 & a, const Vector2 & b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return sqrtf((dx * dx) + (dy*dy));
}

float GetDistanceSquared(const Vector2 & a, const Vector2 & b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return ((dx * dx) + (dy*dy));
}

const Vector2 GetProjectedVector(const Vector2 & vectorToProject, const Vector2 & projectOnto)
{
	Vector2 normalizedProjectOnto = projectOnto.GetNormalized();
	float dot = DotProduct(vectorToProject, normalizedProjectOnto);
	return Vector2(dot * normalizedProjectOnto);
}

const Vector2 GetTransformedIntoBasis(const Vector2 & originalVector, const Vector2 & newBasisI, const Vector2 & newBasisJ)
{
	Vector2 normalizedI = newBasisI.GetNormalized();
	Vector2 normalizedJ = newBasisJ.GetNormalized();
	

	return Vector2(DotProduct(originalVector, normalizedI), DotProduct(originalVector, normalizedJ));
}

const Vector2 GetTransformedOutOfBasis(const Vector2 & vectorInBasis, const Vector2 & oldBasisI, const Vector2 & oldBasisJ)
{
	Vector2 IComponent = vectorInBasis.x * oldBasisI.GetNormalized();
	Vector2 JComponent = vectorInBasis.y * oldBasisJ.GetNormalized();

	return IComponent + JComponent;
}

void DecomposeVectorIntoBasis(const Vector2 & originalVector, const Vector2 & newBasisI, const Vector2 & newBasisJ, Vector2 & out_vectorAlongI, Vector2 & out_vectorAlongJ)
{
	out_vectorAlongI = GetProjectedVector(originalVector, newBasisI);
	out_vectorAlongJ = GetProjectedVector(originalVector, newBasisJ);


}

const Vector2 Reflect(const Vector2 & vectorToBounce, const Vector2 & normal)
{
	Vector2 normalComponent = GetProjectedVector(vectorToBounce, normal);
	Vector2 tangentComponent = vectorToBounce - normalComponent;
	return vectorToBounce - ( 2 * normalComponent);
}

const Vector2 Interpolate(const Vector2 & start, const Vector2 & end, float fractionTowardEnd)
{
	float x = Interpolate(start.x, end.x, fractionTowardEnd);
	float y = Interpolate(start.y, end.y, fractionTowardEnd);
	return Vector2(x,y);
}


