#pragma once
#include "Engine/Math/Matrix44.hpp"
#include <vector>



//the only thing we will add to this is the quaternion class
// this is just the local transform of the transform.
struct transform_t 
{
	transform_t()
		: m_position(0.,0.f,0.f)
		, m_scale(1.f, 1.f, 1.f) {}

	Vector3 m_position; 
	float m_yaw		= 0.f;
	float m_pitch	= 0.f;
	float m_roll	= 0.f;
	Vector3 m_scale; 

	Matrix44 GetMatrix() const; 

	//there's no matrix on this - so what does this do?
	//sets pos, euler, and scale from the matrix
	// have to pull out these values!
	// scale = (|i|, |j|, |k|);
	// euler = (we did it yesterday - normalize basis first)
	// pos = (tx, ty, tz); 
	void SetMatrix( Matrix44 const &mat ); 

	void SetPosition( Vector3 pos ); 
	void Translate( Vector3 offset ); 
	Vector3 GetPosition() const; 

	void SetRotationEuler( Vector3 euler ); 
	void SetRotationEuler( float yaw, float pitch, float roll);
	void RotateByEuler( Vector3 euler ); 
	void RotateByEuler(float yaw, float pitch, float roll);
	Vector3 GetEulerAnglesYawPitchRoll() const; 
	inline float GetPitch() const { return m_pitch; };
	inline float GetYaw() const { return m_yaw; };
	inline float GetRoll() const { return m_roll; };

	void SetScale( Vector3 s ); 
	Vector3 GetScale() const; 

#if defined( USE_X_FORWARD_Z_UP )
	
	Matrix44 m_worldBasis = Matrix44(Vector3(0.f, -1.f, 0.f), 		//right
									Vector3(0.f, 0.f, 1.f), 		//up
									Vector3(1.f, 0.f, 0.f));		//forward
#else
	Matrix44 m_worldBasis = Matrix44::IDENTITY;
#endif


	// STATICS
	static transform_t const IDENTITY; 
};



// all renderables have this, and the camera.
// Can store this either as a pointer or by value
class Transform 
{
public:
	Transform();
	~Transform();
	// should only interface with the data using these
	// transform_t 
	virtual Matrix44 GetWorldMatrix() const;
	Matrix44 GetLocalMatrix() const;  //local to parent matrix, not local to world.
	void SetLocalMatrix( Matrix44 const &mat ); 
	//void SetWorldMatrix(Matrix44 const & mat);

	// 3D utilities
	void SetLocalPosition( Vector3 pos );         //this is allllll local transforming
	void TranslateLocal( Vector3 offset ); 
	Vector3 GetWorldPosition() const; 
	Vector3 GetLocalPosition() const;

	void SetRotationEuler(float yaw, float pitch, float roll);
	void RotateByEuler(float yaw, float pitch, float roll);

	//assumes X right, Y up, Z forward
	void SetRotationEuler( Vector3 euler ); 
	//assumes X right, Y up, Z forward
	void RotateByEuler( Vector3 euler ); 
	Vector3 GetEulerAnglesYawPitchRoll() const; 

	void SetScale( Vector3 s ); 
	Vector3 GetScale() const; 

	Vector3 WorldToLocal(Vector3 worldPos) const;
	void LocalLookAt(Vector3 target);
	void WorldLookAt(Vector3 target);


	Vector3 GetForward() const;
	Vector3 GetRight() const;
	Vector3 GetUp() const;

	void LookAt(const Vector3& position, const Vector3& target, const Vector3& up = Vector3::Y_AXIS);
	
	Transform* GetParent() const;
	void SetParent(Transform* t);
	void AddChild(Transform* t);

	void SetMatrices();
	void UpdateWorldMatrix();

	

private:
	transform_t m_transformStruct; 
	Matrix44 m_localMatrix;
	Matrix44 m_worldMatrix;

	Transform* m_parent = nullptr;
	std::vector<Transform*> m_children;
};