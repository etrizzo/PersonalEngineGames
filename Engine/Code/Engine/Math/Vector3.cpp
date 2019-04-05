#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"
#include <iostream>


const Vector3 Vector3::ZERO		= Vector3(0.f,0.f,0.f);
const Vector3 Vector3::ONE		= Vector3(1.f,1.f,1.f);
const Vector3 Vector3::HALF		= Vector3(.5f,.5f,.5f);
const Vector3 Vector3::Y_AXIS	= Vector3(0.f,1.f,0.f);
const Vector3 Vector3::X_AXIS	= Vector3(1.f, 0.f, 0.f);
const Vector3 Vector3::Z_AXIS	= Vector3(0.f,0.f,1.f);


//-----------------------------------------------------------------------------------------------
Vector3::Vector3( const Vector3& copy )
	: x( 0.f )
	, y( 0.f )
	, z( 0.f )
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}



//-----------------------------------------------------------------------------------------------
Vector3::Vector3( float initialX, float initialY , float initialZ)
	: x( 0.f )
	, y( 0.f )
	, z( 0.f )
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

Vector3::Vector3(const Vector2 & vec2, float initialz)
{
	x = vec2.x;
	y = vec2.y;
	z = initialz;
}


const Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

//-----------------------------------------------------------------------------------------------
const Vector3 Vector3::operator + ( const Vector3& vecToAdd ) const
{
	return Vector3( x + vecToAdd.x, y + vecToAdd.y , z + vecToAdd.z); 
}


//-----------------------------------------------------------------------------------------------
const Vector3 Vector3::operator-( const Vector3& vecToSubtract ) const
{
	return Vector3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z); 
}


//-----------------------------------------------------------------------------------------------
const Vector3 Vector3::operator*( float uniformScale ) const
{
	return Vector3( x*uniformScale, y*uniformScale , z*uniformScale); 
}


//-----------------------------------------------------------------------------------------------
const Vector3 Vector3::operator/( float inverseScale ) const
{
	float scale = 1 / inverseScale;
	return Vector3( x * scale, y * scale, z * scale ); 
}


//-----------------------------------------------------------------------------------------------
void Vector3::operator+=( const Vector3& vecToAdd )
{
	x = x + vecToAdd.x; 
	y = y + vecToAdd.y; 
	z = z + vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vector3::operator-=( const Vector3& vecToSubtract )
{
	x = x - vecToSubtract.x; 
	y = y - vecToSubtract.y; 
	z = z - vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vector3::operator*=( const float uniformScale )
{
	x = x * uniformScale;
	y = y * uniformScale;
	z = z * uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vector3::operator/=( const float uniformDivisor )
{
	float scale = 1/uniformDivisor;
	x = x * scale;
	y = y * scale;
	z = z * scale;
}


//-----------------------------------------------------------------------------------------------
void Vector3::operator=( const Vector3& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
const Vector3 operator*( float uniformScale, const Vector3& vecToScale )
{
	return (vecToScale * uniformScale); 
}


//-----------------------------------------------------------------------------------------------
bool Vector3::operator==( const Vector3& compare ) const
{
	if ((x == compare.x) && (y == compare.y) && (z == compare.z)){
		return true;
	}
	return false; 
}


//-----------------------------------------------------------------------------------------------
bool Vector3::operator!=( const Vector3& compare ) const
{
	if ((x != compare.x) || (y != compare.y) || (z != compare.z)){
		return true;
	}
	return false; 
}

float Vector3::GetLength() const
{
	return sqrtf((x * x) + (y * y) + (z * z));
}


float Vector3::GetLengthSquared() const
{
	return ((x*x) + (y*y) + (z * z));
}

float Vector3::NormalizeAndGetLength() 
{
	float length = this->GetLength();
	x = x/length;
	y = y/length;
	z = z/length;
	return length;

}

void Vector3::ClampLength(float maxLength)
{
	//if the vector is longer than newlength, normalize and multiply by maxLength;
	if (GetLengthSquared() > (maxLength * maxLength))
	{
		NormalizeAndGetLength();
		x *= maxLength;
		y *= maxLength;
		z *= maxLength;
	}
}

Vector3 Vector3::GetNormalized() const
{
	return Vector3(this->x, this->y, this->z)/this->GetLength();
}

float Vector3::GetOrientationDegrees() const
{
	Vector3 normal = GetNormalized();

	float deg = Atan2Degreesf(normal.y, normal.x);

	return deg;
}

Vector3 Vector3::MakeDirectionAtDegrees(float degrees)
{
	float xCoord = CosDegreesf(degrees);
	float yCoord = SinDegreesf(degrees);
	return Vector3(xCoord,yCoord);
}

Vector2 Vector3::XY() const
{
	return Vector2(x,y);
}

Vector2 Vector3::XZ() const
{
	return Vector2(x, z);
}

void Vector3::SetFromText(const char * text)
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
		x = (float) atof(xString.c_str());
		y = (float) atof(yString.c_str());
		z = (float) atof(zString.c_str());
	}


}



float GetDistance(const Vector3 & a, const Vector3 & b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return sqrtf((dx * dx) + (dy*dy) + (dz * dz));
}

float GetDistanceSquared(const Vector3 & a, const Vector3 & b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return ((dx * dx) + (dy*dy) + (dz * dz));
}

Vector3 Average(const Vector3 & a, const Vector3 & b)
{
	return ((a + b ) * .5f);
}

const Vector3 GetProjectedVector(const Vector3 & vectorToProject, const Vector3 & projectOnto)
{
	Vector3 normalizedProjectOnto = projectOnto.GetNormalized();
	float dot = DotProduct(vectorToProject, normalizedProjectOnto);
	return Vector3(dot * normalizedProjectOnto);
}

const Vector3 GetTransformedIntoBasis(const Vector3 & originalVector, const Vector3 & newBasisI, const Vector3 & newBasisJ)
{
	Vector3 normalizedI = newBasisI.GetNormalized();
	Vector3 normalizedJ = newBasisJ.GetNormalized();


	return Vector3(DotProduct(originalVector, normalizedI), DotProduct(originalVector, normalizedJ));
}

const Vector3 GetTransformedOutOfBasis(const Vector3 & vectorInBasis, const Vector3 & oldBasisI, const Vector3 & oldBasisJ)
{
	Vector3 IComponent = vectorInBasis.x * oldBasisI.GetNormalized();
	Vector3 JComponent = vectorInBasis.y * oldBasisJ.GetNormalized();

	return IComponent + JComponent;
}

void DecomposeVectorIntoBasis(const Vector3 & originalVector, const Vector3 & newBasisI, const Vector3 & newBasisJ, Vector3 & out_vectorAlongI, Vector3 & out_vectorAlongJ)
{
	out_vectorAlongI = GetProjectedVector(originalVector, newBasisI);
	out_vectorAlongJ = GetProjectedVector(originalVector, newBasisJ);


}

const Vector3 Reflect(const Vector3 & vectorToBounce, const Vector3 & normal)
{
	Vector3 normalComponent = GetProjectedVector(vectorToBounce, normal);
	Vector3 tangentComponent = vectorToBounce - normalComponent;
	return vectorToBounce - ( 2 * normalComponent);
}

const Vector3 Interpolate(const Vector3 & start, const Vector3 & end, float fractionTowardEnd)
{
	float x = Interpolate(start.x, end.x, fractionTowardEnd);
	float y = Interpolate(start.y, end.y, fractionTowardEnd);
	float z = Interpolate(start.z, end.z, fractionTowardEnd);
	return Vector3(x,y,z);
}

const Vector3 Cross(const Vector3 & A, const Vector3 & B)
{
	return Vector3((A.y * B.z) - (A.z * B.y), (A.z * B.x) - (A.x * B.z), (A.x * B.y) - (A.y * B.x));
}

const Vector3 GetRandomNormalizedDirection()
{
	return Vector3(GetRandomFloatZeroToOne(), GetRandomFloatZeroToOne(), GetRandomFloatZeroToOne()).GetNormalized();
}

const Vector3 GetRandomPointOnSphere()
{
	return Vector3(GetRandomFloatInRange(-1.f,1.f), GetRandomFloatInRange(-1.f,1.f), GetRandomFloatInRange(-1.f,1.f)).GetNormalized();
}


