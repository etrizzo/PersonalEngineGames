#pragma once
#include "Engine/Math/Matrix44.hpp"
#include <vector>



//the only thing we will add to this is the quaternion class
// this is just the local transform of the transform.
struct transform_t 
{
	transform_t()
		: m_position(0.,0.f,0.f)
		, m_euler(0.,0.f,0.f)
		, m_scale(1.f, 1.f, 1.f) {}

	Vector3 m_position; 
	Vector3 m_euler; 
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
	void RotateByEuler( Vector3 euler ); 
	Vector3 GetEulerAngles() const; 

	void SetScale( Vector3 s ); 
	Vector3 GetScale() const; 


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
	Matrix44 GetWorldMatrix() const;
	Matrix44 GetLocalMatrix() const;  //local to parent matrix, not local to world.
	void SetLocalMatrix( Matrix44 const &mat ); 
	//void SetWorldMatrix(Matrix44 const & mat);

	// 3D utilities
	void SetLocalPosition( Vector3 pos );         //this is allllll local transforming
	void TranslateLocal( Vector3 offset ); 
	Vector3 GetWorldPosition() const; 
	Vector3 GetLocalPosition() const;

	void SetRotationEuler( Vector3 euler ); 
	void RotateByEuler( Vector3 euler ); 
	Vector3 GetEulerAngles() const; 

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