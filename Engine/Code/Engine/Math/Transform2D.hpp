#pragma once
#include "Engine/Math/Matrix44.hpp"
#include <vector>



//the only thing we will add to this is the quaternion class
// this is just the local transform of the transform.
struct transform2D_t 
{
	transform2D_t()
		: m_position(0.,0.f)
		, m_rotation(0.f)
		, m_scale(1.f, 1.f) {}

	Vector2 m_position; 
	float m_rotation; 
	Vector2 m_scale; 

	Matrix44 GetMatrix() const; 

	//there's no matrix on this - so what does this do?
	//sets pos, euler, and scale from the matrix
	// have to pull out these values!
	// scale = (|i|, |j|, |k|);
	// euler = (we did it yesterday - normalize basis first)
	// pos = (tx, ty, tz); 
	void SetMatrix( Matrix44 const &mat ); 

	void SetPosition( Vector2 pos ); 
	void Translate( Vector2 offset ); 
	Vector2 GetPosition() const; 

	void SetRotationEuler( float euler ); 
	void RotateByEuler( float euler ); 
	float GetEulerAngles() const; 

	void SetScale( Vector2 s ); 
	Vector2 GetScale() const; 

	// STATICS
	static transform2D_t const IDENTITY; 
};


// this will get refactored later
// all renderables have this, and the camera.
// Can store this either as a pointer or by value
class Transform2D 
{
public:
	Transform2D();
	~Transform2D();
	// should only interface with the data using these
	// transform_t 
	Matrix44 GetWorldMatrix() const;
	Matrix44 GetLocalMatrix() const;  //local to parent matrix, not local to world.
	void SetLocalMatrix( Matrix44 const &mat ); 
	Vector2 GetWorldPosition() const; 
	Vector2 GetLocalPosition() const;
	float GetRotation() const;
	Vector2 GetScale() const;

	//2D Utilities
	void SetLocalPosition( Vector2 pos );         //this is allllll local transforming
	void TranslateLocal( Vector2 offset ); 

	void SetRotationEuler( float rotation ); 
	void RotateByEuler( float rotationOffset ); 

	void SetScale( Vector2 s ); 


	//Vector3 GetForward() const;
	Vector3 GetRight() const;
	Vector3 GetUp() const;

	void LookAt(const Vector3& position, const Vector3& target, const Vector3& up = Vector3::UP);

	void SetParent(Transform2D* t);
	void AddChild(Transform2D* t);

	void SetMatrices();
	void UpdateWorldMatrix();

private:
	transform2D_t m_transformStruct; 
	Matrix44 m_localMatrix;
	Matrix44 m_worldMatrix;



	Transform2D* m_parent = nullptr;
	std::vector<Transform2D*> m_children;
};