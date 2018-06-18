#include "Matrix44.hpp"


const Matrix44 Matrix44::IDENTITY = Matrix44();

Matrix44::Matrix44()
{
	SetIdentity();
}

Matrix44::Matrix44(const float * sixteenValuesBasisMajor)
{
	SetValues(sixteenValuesBasisMajor);
}

Matrix44::Matrix44(const Vector2 & iBasis, const Vector2 & jBasis, const Vector2 & translation)
{
	Ix = iBasis.x;
	Iy = iBasis.y;
	Jx = jBasis.x;
	Jy = jBasis.y;

	Tx = translation.x;
	Ty = translation.y;
}

Matrix44::Matrix44(const Vector3 & iBasis, const Vector3 & jBasis, const Vector3 & kBasis, const Vector3 & translation)
{
	Ix = iBasis.x;
	Iy = iBasis.y;
	Iz = iBasis.z;

	Jx = jBasis.x;
	Jy = jBasis.y;
	Jz = jBasis.z;

	Kx = kBasis.x;
	Ky = kBasis.y;
	Kz = kBasis.z;

	Tx = translation.x;
	Ty = translation.y;
	Tz = translation.z;
}

Matrix44::Matrix44(const Vector4 & iBasis, const Vector4 & jBasis, const Vector4 & kBasis, const Vector4 & translation)
{
	Ix = iBasis.x;
	Iy = iBasis.y;
	Iz = iBasis.z;
	Iw = iBasis.w;

	Jx = jBasis.x;
	Jy = jBasis.y;
	Jz = jBasis.z;
	Jw = jBasis.w;

	Kx = kBasis.x;
	Ky = kBasis.y;
	Kz = kBasis.z;
	Kw = kBasis.w;

	Tx = translation.x;
	Ty = translation.y;
	Tz = translation.z;
	Tw = translation.w;
}

Vector2 Matrix44::TransformPosition2D(const Vector2 & position2D)
{
	Vector2 displacement = TransformDisplacement2D(position2D);
	Vector2 transform = Vector2(Tx, Ty);
	return displacement + transform;
}

Vector2 Matrix44::TransformDisplacement2D(const Vector2 & displacement2D)
{
	float x = (Ix * displacement2D.x) + (Jx * displacement2D.y);
	float y = (Iy * displacement2D.x) + (Jy * displacement2D.y);
	return Vector2(x,y);
}

Vector3 Matrix44::TransformPosition(const Vector3 & position, Matrix44 mat)
{
	Vector4 pos = Vector4(position, 1);
	//Matrix44 i = Matrix44::IDENTITY;
	//i.SetT(Vector4(position, 1));
	////mat.Translate3D(position);
	//i.Append(mat);

	float x = pos.DotProduct(mat.GetX());
	float y = pos.DotProduct(mat.GetY());
	float z = pos.DotProduct(mat.GetZ());

	return Vector3(x,y,z);
}


void Matrix44::Append(const Matrix44 & matrixToAppend)
{
	Vector4 I = matrixToAppend.GetI();
	Vector4 J = matrixToAppend.GetJ();
	Vector4 K = matrixToAppend.GetK();
	Vector4 T = matrixToAppend.GetT();

	Vector4 X = GetX();
	Vector4 Y = GetY();
	Vector4 Z = GetZ();
	Vector4 W = GetW();

	float* newVals = new float[16];
	int Istart = 0;
	int Jstart = 4;
	int Kstart = 8;
	int Tstart = 12;

	newVals[Istart + 0] = I.DotProduct(X);
	newVals[Istart + 1] = I.DotProduct(Y);
	newVals[Istart + 2] = I.DotProduct(Z);
	newVals[Istart + 3] = I.DotProduct(W);

	newVals[Jstart + 0] = J.DotProduct(X);
	newVals[Jstart + 1] = J.DotProduct(Y);
	newVals[Jstart + 2] = J.DotProduct(Z);
	newVals[Jstart + 3] = J.DotProduct(W);

	newVals[Kstart + 0] = K.DotProduct(X);
	newVals[Kstart + 1] = K.DotProduct(Y);
	newVals[Kstart + 2] = K.DotProduct(Z);
	newVals[Kstart + 3] = K.DotProduct(W);

	newVals[Tstart + 0] = T.DotProduct(X);
	newVals[Tstart + 1] = T.DotProduct(Y);
	newVals[Tstart + 2] = T.DotProduct(Z);
	newVals[Tstart + 3] = T.DotProduct(W);
	
	SetValues(newVals);

}

void Matrix44::RotateDegrees2D(float rotationDegreesAboutZ)
{
	//RotateDegrees3D(Vector3(0.f,0.f, rotationDegreesAboutZ));
	Matrix44 rotationMatrix = MakeRotationDegrees2D(rotationDegreesAboutZ);

	Append(rotationMatrix);
}

void Matrix44::Translate2D(const Vector2 & translation)
{
	Matrix44 translationMatrix = MakeTranslation2D(translation);
	Append(translationMatrix);
}

void Matrix44::ScaleUniform2D(float scaleXY)
{
	Matrix44 scaleMatrix = MakeScaleUniform2D(scaleXY);
	Append(scaleMatrix);
}

void Matrix44::Scale2D(float scaleX, float scaleY)
{
	Matrix44 scaleMatrix = MakeScale2D(scaleX, scaleY);
	Append(scaleMatrix);
}

void Matrix44::RotateDegrees3D(const Vector3 & rotation)
{
	Matrix44 rotationMatrix = MakeRotationDegrees3D(rotation);

	Append(rotationMatrix);
}

void Matrix44::Translate3D(const Vector3 & translation)
{
	Matrix44 translationMatrix = MakeTranslation3D(translation);
	Append(translationMatrix);
}

void Matrix44::ScaleUniform3D(float scaleXYZ)
{
	Scale3D(scaleXYZ, scaleXYZ, scaleXYZ);
}

void Matrix44::Scale3D(float scaleX, float scaleY, float scaleZ)
{
	Matrix44 scaleMatrix = MakeScale3D(scaleX, scaleY, scaleZ);
	Append(scaleMatrix);
}

void Matrix44::Scale3D(const Vector3 & scaleXYZ)
{
	Scale3D(scaleXYZ.x, scaleXYZ.y, scaleXYZ.z);
}


void Matrix44::RotateDegreesX(float pitch)
{
	Matrix44 rotation = MakeRotationDegreesX(pitch);
	Append(rotation);
}

void Matrix44::RotateDegreesY(float yaw)
{
	Matrix44 rotation = MakeRotationDegreesY(yaw);
	Append(rotation);
}

void Matrix44::RotateDegreesZ(float roll)
{
	Matrix44 rotation = MakeRotationDegreesZ(roll);
	Append(rotation);
}

Matrix44 Matrix44::MakeRotationDegrees2D(float rotationDegreesAboutZ)
{
	//Vector2 x = Vector2(CosDegreesf(rotationDegreesAboutZ), SinDegreesf(rotationDegreesAboutZ));
	//Vector2 y = Vector2(-1.f * SinDegreesf(rotationDegreesAboutZ), CosDegreesf(rotationDegreesAboutZ));
	//
	//return Matrix44(x,y);

	return MakeRotationDegrees3D(0.f,0.f,rotationDegreesAboutZ);
}

Matrix44 Matrix44::MakeRotationDegrees3D(const Vector3& rotationPitchYawRoll)
{
	return MakeRotationDegrees3D(rotationPitchYawRoll.x, rotationPitchYawRoll.y, rotationPitchYawRoll.z);
}

//yaw pitch roll matrix
//source for explicit rotation matrix: http://danceswithcode.net/engineeringnotes/rotations_in_3d/rotations_in_3d_part1.html
Matrix44 Matrix44::MakeRotationDegrees3D(float u, float v, float w)
{
	//Vector3 x = Vector3(CosDegreesf(v) * CosDegreesf(w), (SinDegreesf(u) * SinDegreesf(v) * CosDegreesf(w)) - (CosDegreesf(u) * SinDegreesf(w)),  (SinDegreesf(u) * SinDegreesf(w)) + (CosDegreesf(u) * SinDegreesf(v) * CosDegreesf(w)));
	//Vector3 y = Vector3(CosDegreesf(v) * CosDegreesf(w), (CosDegreesf(u) * CosDegreesf(w)) + (SinDegreesf(u) * SinDegreesf(v) * SinDegreesf(w)), (CosDegreesf(u) * SinDegreesf(v) * SinDegreesf(w)) - (SinDegreesf(u) * CosDegreesf(w)));
	//Vector3 z = Vector3(-SinDegreesf(v), SinDegreesf(u) * CosDegreesf(v), CosDegreesf(u) * CosDegreesf(v));
	
	Matrix44 R;
	R.SetIdentity();
	
	
	R.Append(MakeRotationDegreesY(v));

	R.Append(MakeRotationDegreesX(u));
	R.Append(MakeRotationDegreesZ(w));
	
	return R;
}

Matrix44 Matrix44::MakeRotationDegreesX(float pitch)
{
	float sinx = SinDegreesf(pitch);
	float cosx = CosDegreesf(pitch);
	Matrix44 mat;
	mat.SetIdentity();

	Vector3 y = Vector3(0.f, cosx, sinx);
	Vector3 z = Vector3(0.f, -sinx, cosx);

	mat.SetY(Vector4(y, 0.f));
	mat.SetZ(Vector4(z, 0.f));
	return mat;
}

Matrix44 Matrix44::MakeRotationDegreesY(float yaw)
{
	float siny = SinDegreesf(yaw);
	float cosy = CosDegreesf(yaw);
	Matrix44 mat;
	mat.SetIdentity();

	Vector3 x = Vector3(cosy, 0.f, siny);
	Vector3 z = Vector3(-siny, 0.f, cosy);

	mat.SetX(Vector4(x, 0.f));
	mat.SetZ(Vector4(z, 0.f));
	return mat;
}

Matrix44 Matrix44::MakeRotationDegreesZ(float roll)
{
	float sinz = SinDegreesf(roll);
	float cosz = CosDegreesf(roll);
	Matrix44 mat;
	mat.SetIdentity();

	Vector3 x = Vector3(cosz, -sinz, 0.f);
	Vector3 y = Vector3(sinz,  cosz, 0.f);

	mat.SetX(Vector4(x, 0.f));
	mat.SetY(Vector4(y, 0.f));
	return mat;
}

Matrix44 Matrix44::MakeTranslation2D(const Vector2 & translation)
{
	return Matrix44(Vector2(1,0), Vector2(0,1), translation);
}

Matrix44 Matrix44::MakeTranslation3D(const Vector3 & translation)
{
	Matrix44 mat = Matrix44();
	mat.SetIdentity();
	mat.SetT(Vector4(translation.x, translation.y, translation.z, 1.f));
	return mat;
}

Matrix44 Matrix44::MakeScaleUniform2D(float scaleXY)
{
	return Matrix44(Vector2(scaleXY, 0), Vector2(0, scaleXY));
}

Matrix44 Matrix44::MakeScaleUniform3D(float scaleXYZ)
{
	return Matrix44(Vector3(scaleXYZ, 0, 0), Vector3(0, scaleXYZ, 0), Vector3(0, 0, scaleXYZ));
}

Matrix44 Matrix44::MakeScale2D(float scaleX, float scaleY)
{
	return Matrix44(Vector2(scaleX, 0), Vector2(0, scaleY));
}

Matrix44 Matrix44::MakeScale3D(float scaleX, float scaleY, float scaleZ)
{
	return Matrix44(Vector3(scaleX, 0, 0), Vector3(0, scaleY, 0), Vector3(0, 0, scaleZ));
}

Matrix44 Matrix44::MakeOrtho2D(const Vector2 & bottomLeft, const Vector2 & topRight)
{
	float* newVals = new float[16];
	for (int i = 0; i < 16; i++){
		newVals[i] = 0.f;
	}
	newVals[0] = 2.f / (topRight.x - bottomLeft.x);
	newVals[5] = 2.f / (topRight.y - bottomLeft.y);
	newVals[10] = -2.f / (1.f - 0.f);		//-2 / far - near?;
	newVals[12] = -1.f * ((topRight.x + bottomLeft.x) / (topRight.x  - bottomLeft.x));
	newVals[13] = -1.f * ((topRight.y + bottomLeft.y) / (topRight.y - bottomLeft.y));
	newVals[14] = -1.f;		//TBD
	newVals[15] = 1.f;
	return Matrix44(newVals);
}


//3D ortho matrix reference: https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/orthographic-projection-matrix
Matrix44 Matrix44::MakeOrtho3D(const Vector3 & nearBottomLeft, const Vector3 & farTopRight)
{
	Matrix44 newMatrix = Matrix44();
	newMatrix.SetIdentity();
	float* newVals = new float[16];
	for (int i = 0; i < 16; i++){
		newVals[i] = 0.f;
	}
	newVals[0] =	2.f / (farTopRight.x - nearBottomLeft.x);		//2 / r - l
	newVals[5] =	2.f / (farTopRight.y - nearBottomLeft.y);		//2 / t - b
	newVals[10] =	2.f / (farTopRight.z - nearBottomLeft.z);		//-2 / far - near?;
	//newVals[12] =	-1.f * ((farTopRight.x + nearBottomLeft.x) / (farTopRight.x  - nearBottomLeft.x));
	//newVals[13] =	-1.f * ((farTopRight.y + nearBottomLeft.y) / (farTopRight.y - nearBottomLeft.y));
	//newVals[14] =	-1.f * ((farTopRight.z + nearBottomLeft.z) / (farTopRight.z - nearBottomLeft.z));
	newVals[15] =	1.f;
	newMatrix.SetValues(newVals);
	return newMatrix;
}

Matrix44 Matrix44::MakePerspectiveProjection(float fov_degrees, float aspect, float nz, float fz)
{
	float d = 1.0f / tanf(fov_degrees * .5f);
	d = (float) fabs(d);
	float q = 1.0f / (fz - nz); 

	Vector4 i = Vector4( d / aspect, 0,    0,                   0 );  
	Vector4 j = Vector4( 0,          d,    0,                   0 ); 
	Vector4 k = Vector4( 0,          0,    (nz + fz) * q,       1 );
	Vector4 t = Vector4( 0,          0,    -2.0f * nz * fz * q, 0 ); 

	return Matrix44( i, j, k, t ); 
}

Matrix44 Matrix44::LookAt(const Vector3 & position, const Vector3 & target, const Vector3 & up)
{
	Vector3 direction = target - position;
	Vector3 newForward = direction.GetNormalized();
	//figure out other 2 axes
	Vector3 newRight = Cross(up, newForward);
	newRight.NormalizeAndGetLength();
	Vector3 newUp = Cross(newForward, newRight);
	Matrix44 lookMatrix = Matrix44(newRight, newUp, newForward, position);
	return lookMatrix;
}

void Matrix44::ScaleMatrix(float scaleFactor)
{
	Ix*= scaleFactor;
	Iy*= scaleFactor;
	Iz*= scaleFactor;
	Iw*= scaleFactor;

	Jx*= scaleFactor;
	Jy*= scaleFactor;
	Jz*= scaleFactor;
	Jw*= scaleFactor;

	Kx*= scaleFactor;
	Ky*= scaleFactor;
	Kz*= scaleFactor;
	Kw*= scaleFactor;

	Tx*= scaleFactor;
	Ty*= scaleFactor;
	Tz*= scaleFactor;
	Tw*= scaleFactor;
}

void Matrix44::SetIdentity()
{
	Ix = 1.f;
	Iy = 0.f;
	Iz = 0.f;
	Iw = 0.f;

	Jx = 0.f;
	Jy = 1.f;
	Jz = 0.f;
	Jw = 0.f;

	Kx = 0.f;
	Ky = 0.f;
	Kz = 1.f;
	Kw = 0.f;

	Tx = 0.f;
	Ty = 0.f;
	Tz = 0.f;
	Tw = 1.f;
}

void Matrix44::SetValues(const float * sixteenValuesBasisMajor)
{
	int Istart = 0;
	int Jstart = 4;
	int Kstart = 8;
	int Tstart = 12;

	Ix = sixteenValuesBasisMajor[Istart + 0];
	Iy = sixteenValuesBasisMajor[Istart + 1];
	Iz = sixteenValuesBasisMajor[Istart + 2];
	Iw = sixteenValuesBasisMajor[Istart + 3];

	Jx = sixteenValuesBasisMajor[Jstart + 0];
	Jy = sixteenValuesBasisMajor[Jstart + 1];
	Jz = sixteenValuesBasisMajor[Jstart + 2];
	Jw = sixteenValuesBasisMajor[Jstart + 3];

	Kx = sixteenValuesBasisMajor[Kstart + 0];
	Ky = sixteenValuesBasisMajor[Kstart + 1];
	Kz = sixteenValuesBasisMajor[Kstart + 2];
	Kw = sixteenValuesBasisMajor[Kstart + 3];

	Tx = sixteenValuesBasisMajor[Tstart + 0];
	Ty = sixteenValuesBasisMajor[Tstart + 1];
	Tz = sixteenValuesBasisMajor[Tstart + 2];
	Tw = sixteenValuesBasisMajor[Tstart + 3];

	delete sixteenValuesBasisMajor;
}

float Matrix44::Dot4114(float x1, float y1, float z1, float w1, float x2, float y2, float z2, float w2)
{
	float dot = (x1 * x2) + (y1 * y2) + (z1 * z2) + (w1 * w2);
	return dot;
}

Vector4 Matrix44::GetI() const
{
	return Vector4(Ix,Iy,Iz,Iw);
}

Vector4 Matrix44::GetJ() const
{
	return Vector4(Jx,Jy,Jz,Jw);
}

Vector4 Matrix44::GetK() const
{
	return Vector4(Kx,Ky,Kz,Kw);
}

Vector4 Matrix44::GetT() const
{
	return Vector4(Tx,Ty,Tz,Tw);
}

Vector4 Matrix44::GetX() const
{
	return Vector4(Ix,Jx,Kx,Tx);
}

Vector4 Matrix44::GetY() const
{
	return Vector4(Iy,Jy,Ky,Ty);
}

Vector4 Matrix44::GetZ() const
{
	return Vector4(Iz,Jz,Kz,Tz);
}

Vector4 Matrix44::GetW() const
{
	return Vector4(Iw,Jw,Kw,Tw);
}

Vector3 Matrix44::GetScale() const
{
	return Vector3(GetI().XYZ().GetLength(), GetJ().XYZ().GetLength(), GetK().XYZ().GetLength());	// scale = (|i|, |j|, |k|)
}

Vector3 Matrix44::GetEulerAngles() const
{
	float x;
	float y;
	float z;

	float sx = Ky;
	sx = ClampFloat(sx, -1.f, 1.f);
	x = ConvertRadiansToDegrees(asinf(sx));
	
	float cx = CosDegreesf(x);
	if (cx != 0.f){
		y = Atan2Degreesf(Kx, Kz);
		z = Atan2Degreesf(Iy, Jy);
	} else {
		z = 0.f;
		y = Atan2Degreesf(Jx, Ix);
	}
	
	//x = Atan2Degreesf(Iy, Ix);
	//y = Atan2Degreesf(-Iz, sqrtf((Jz * Jz) + (Kz*Kz)));
	//z = Atan2Degreesf(Jz, Kz)

	//x = Atan2Degreesf(Ky, Kz);
	//float c2 = sqrtf((Ix * Ix) + (Jx * Jx));
	//y = Atan2Degreesf(-Kx, c2);
	//z = Atan2Degreesf(Jx, Ix)


	return Vector3(x,y,z);
}

Vector3 Matrix44::GetPosition() const
{
	return GetT().XYZ();
}

void Matrix44::SetI(Vector4 vals)
{
	Ix = vals.x;
	Iy = vals.y;
	Iz = vals.z;
	Iw = vals.w;
}

void Matrix44::SetJ(Vector4 vals)
{
	Jx = vals.x;
	Jy = vals.y;
	Jz = vals.z;
	Jw = vals.w;
}

void Matrix44::SetK(Vector4 vals)
{
	Kx = vals.x;
	Ky = vals.y;
	Kz = vals.z;
	Kw = vals.w;
}

void Matrix44::SetT(Vector4 vals)
{
	Tx = vals.x;
	Ty = vals.y;
	Tz = vals.z;
	Tw = vals.w;
}

void Matrix44::SetX(Vector4 vals)
{
	Ix = vals.x;
	Jx = vals.y;
	Kx = vals.z;
	Tx = vals.w;
}

void Matrix44::SetY(Vector4 vals)
{
	Iy = vals.x;
	Jy = vals.y;
	Ky = vals.z;
	Ty = vals.w;
}

void Matrix44::SetZ(Vector4 vals)
{
	Iz = vals.x;
	Jz = vals.y;
	Kz = vals.z;
	Tz = vals.w;
}

void Matrix44::SetW(Vector4 vals)
{
	Iw = vals.x;
	Jw = vals.y;
	Kw = vals.z;
	Tw = vals.w;
}

Vector4::Vector4(float vx, float vy, float vz, float vw)
{
	x = vx;
	y = vy;
	z = vz;
	w = vw;
}

Vector4::Vector4(Vector3 xyz, float newW)
{
	x = xyz.x;
	y = xyz.y;
	z = xyz.z;
	w = newW;
}

Vector4 Vector4::operator*(float scale)
{
	return Vector4(scale * x, scale * y, scale * z, scale * w);
}

float Vector4::DotProduct(const Vector4 v)
{
	float retVal = (x * v.x) + (y * v.y) + (z * v.z) + (w * v.w);
	return retVal;
}

Vector3 Vector4::XYZ() const
{
	return Vector3(x,y,z);
}

Matrix44 InvertFast(const Matrix44 & matrix)
{
	//(RT)-1 = (T)-1 (R(Transpose))
	//-t.x should be (-t (dot) (R_transpose.I())
	Matrix44 RInverse = Matrix44();
	RInverse.SetIdentity();
	Vector3 x = matrix.GetX().XYZ();
	Vector3 y = matrix.GetY().XYZ();
	Vector3 z = matrix.GetZ().XYZ();

	RInverse.SetI(Vector4(x));
	RInverse.SetJ(Vector4(y));
	RInverse.SetK(Vector4(z));


	Matrix44 TInverse = Matrix44();
	TInverse.SetIdentity();
	Vector4 t = matrix.GetT();
	TInverse.SetT(Vector4(-t.x,-t.y,-t.z,1.f));

	RInverse.Append(TInverse);

	//Vector3 i = matrix.GetI().XYZ();
	//Vector3 j = matrix.GetJ().XYZ();
	//Vector3 k = matrix.GetK().XYZ();

	//newMat.SetX(Vector4(i));
	//newMat.SetY(Vector4(j));
	//newMat.SetZ(Vector4(k));

	//Vector4 w = matrix.GetW();
	//newMat.SetW(w * -1.f);

	return RInverse;
}
