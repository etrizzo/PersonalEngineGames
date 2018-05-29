#pragma once
//#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"

class Vector4{
public :
	Vector4(){};
	explicit Vector4(float vx, float vy, float vz, float vw);
	explicit Vector4(Vector3 xyz, float w = 0.f);

	Vector4 operator*(float scale);

	float x;
	float y;
	float z;
	float w;

	float DotProduct(const Vector4 vectorToDot);
	Vector3 XYZ() const;
};

class Matrix44{
public:
	Matrix44();

	explicit Matrix44( const float* sixteenValuesBasisMajor);
	explicit Matrix44( const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation = Vector2(0.f,0.f));
	explicit Matrix44( const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation = Vector3(0.f,0.f,0.f));
	explicit Matrix44( const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation = Vector4(0.f,0.f,0.f,0.f));


	float Ix = 1.f;
	float Iy = 0.f;
	float Iz = 0.f;
	float Iw = 0.f;

	float Jx = 0.f;
	float Jy = 1.f;
	float Jz = 0.f;
	float Jw = 0.f;

	float Kx = 0.f;
	float Ky = 0.f;
	float Kz = 1.f;
	float Kw = 0.f;

	float Tx = 0.f;
	float Ty = 0.f;
	float Tz = 0.f;
	float Tw = 1.f;

	//Accessors
	Vector2 TransformPosition2D(const Vector2& position2D);	//assumes z = 0, w = 1
	Vector2 TransformDisplacement2D( const Vector2& displacement2D);	//assumes z = 0; w = 0
	

	// Mutators
	void SetIdentity();
	void SetValues( const float* sixteenValuesBasisMajor ); // float[16] array in order Ix, Iy...
	void Append( const Matrix44& matrixToAppend ); // a.k.a. Concatenate (right-multiply)

	void RotateDegrees2D( float rotationDegreesAboutZ ); // 
	void Translate2D( const Vector2& translation );
	void ScaleUniform2D( float scaleXY );
	void Scale2D( float scaleX, float scaleY );

	void RotateDegrees3D(const Vector3& rotation);
	void Translate3D(const Vector3& translation);
	void ScaleUniform3D(float scaleXYZ);
	void Scale3D(float scaleX, float scaleY, float scaleZ);
	void Scale3D(const Vector3& scaleXYZ);

	void RotateDegreesX(float pitch);
	void RotateDegreesY(float yaw);
	void RotateDegreesZ(float roll);


	// Producers
	static Matrix44 MakeRotationDegrees2D( float rotationDegreesAboutZ );
	static Matrix44 MakeRotationDegrees3D( const Vector3& rotationPitchYawRoll);
	static Matrix44 MakeRotationDegrees3D( float pitch, float yaw, float roll);
	static Matrix44 MakeRotationDegreesX(float pitch);
	static Matrix44 MakeRotationDegreesY(float yaw);
	static Matrix44 MakeRotationDegreesZ(float roll);
	static Matrix44 MakeTranslation2D( const Vector2& translation );
	static Matrix44 MakeTranslation3D( const Vector3& translation );
	static Matrix44 MakeScaleUniform2D( float scaleXY );
	static Matrix44 MakeScaleUniform3D( float scaleXYZ);
	static Matrix44 MakeScale2D( float scaleX, float scaleY );
	static Matrix44 MakeScale3D( float scaleX, float scaleY, float scaleZ);
	static Matrix44 MakeOrtho2D( const Vector2& bottomLeft, const Vector2& topRight );
	static Matrix44 MakeOrtho3D( const Vector3& nearBottomLeft, const Vector3& farTopRight);
	static Matrix44 MakePerspectiveProjection( float fov_degrees, float aspect, float nearZ, float farZ );

	static Matrix44 LookAt(const Vector3& position, const Vector3& target, const Vector3& up = Vector3::UP);
	static Vector3 TransformPosition(const Vector3& position, Matrix44 mat);


	void ScaleMatrix(float scaleFactor);

	Vector4 GetI() const;
	Vector4 GetJ() const;
	Vector4 GetK() const;
	Vector4 GetT() const;
				   
	Vector4 GetX() const;
	Vector4 GetY() const;
	Vector4 GetZ() const;
	Vector4 GetW() const;

	Vector3 GetScale() const;
	Vector3 GetEulerAngles() const;
	Vector3 GetPosition() const;


	void SetI(Vector4 vals);
	void SetJ(Vector4 vals);
	void SetK(Vector4 vals);
	void SetT(Vector4 vals);

	void SetX(Vector4 vals);
	void SetY(Vector4 vals);
	void SetZ(Vector4 vals);
	void SetW(Vector4 vals);

	const static Matrix44 IDENTITY;

private:
	float Dot4114(float x1, float y1, float z1, float w1, float x2, float y2, float z2, float w2);

};

Matrix44 InvertFast( const Matrix44& matrix);
