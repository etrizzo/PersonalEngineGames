#include "Transform.hpp"

Matrix44 transform_t::GetMatrix() const
{
	Matrix44 mat = Matrix44();
	mat.SetIdentity();
	mat.Translate3D(m_position);
	mat.RotateDegrees3D(m_euler);
	mat.Scale3D(m_scale);
	return mat;
}

void transform_t::SetMatrix(Matrix44 const & mat)
{
	m_scale = mat.GetScale();
	m_euler = mat.GetEulerAngles();
	m_position = mat.GetPosition();

}

void transform_t::SetPosition(Vector3 pos)
{
	m_position = pos;
}

void transform_t::Translate(Vector3 offset)
{
	m_position+=offset;
}

Vector3 transform_t::GetPosition() const
{
	return m_position;
}

void transform_t::SetRotationEuler(Vector3 euler)
{
	m_euler = euler;
}

void transform_t::RotateByEuler(Vector3 euler)
{
	m_euler+=euler;
	m_euler.x = ClampFloat( m_euler.x, -90.f, 90.f ); 
	//m_euler.y = modf( m_euler.y, 2.0f * PI ); 

}

Vector3 transform_t::GetEulerAngles() const
{
	return m_euler;
}

void transform_t::SetScale(Vector3 s)
{
	m_scale = s;
}

Vector3 transform_t::GetScale() const
{
	return m_scale;
}

Transform::Transform()
{
	m_transformStruct = transform_t();
	m_localMatrix = Matrix44();
	m_localMatrix.SetIdentity();
	m_parent = nullptr;
	m_children = std::vector<Transform*>();
}

Transform::~Transform()
{
	m_children.clear();
}

Matrix44 Transform::GetWorldMatrix() const
{
	//m_worldMatrix = Matrix44(m_localMatrix);
	//if (m_parent != nullptr){
	//	Matrix44 parent = m_parent->GetWorldMatrix();
	//	parent.Append(m_worldMatrix);
	//	return parent;
	//}
	return m_worldMatrix;
}

Matrix44 Transform::GetLocalMatrix() const
{
	return m_localMatrix;
}

void Transform::SetLocalMatrix(Matrix44 const & mat)
{
	m_transformStruct.SetMatrix(mat);
	SetMatrices();
}

void Transform::SetLocalPosition(Vector3 pos)
{
	m_transformStruct.SetPosition(pos);
	SetMatrices();
}

void Transform::TranslateLocal(Vector3 offset)
{
	m_transformStruct.Translate(offset);
	SetMatrices();
}

Vector3 Transform::GetWorldPosition() const
{
	Matrix44 world = GetWorldMatrix();
	return world.GetPosition();
}

Vector3 Transform::GetLocalPosition() const
{
	return m_transformStruct.GetPosition();
}

void Transform::SetRotationEuler(Vector3 euler)
{
	m_transformStruct.SetRotationEuler(euler);
	
	SetMatrices();
}

void Transform::RotateByEuler(Vector3 euler)
{
	m_transformStruct.RotateByEuler(euler);
	SetMatrices();
}

Vector3 Transform::GetEulerAngles() const
{
	return m_transformStruct.GetEulerAngles();
}

void Transform::SetScale(Vector3 s)
{
	m_transformStruct.SetScale(s);
	SetMatrices();
}

Vector3 Transform::GetScale() const
{
	return m_transformStruct.GetScale();
}

void Transform::SetLocalPosition2D(Vector2 pos, float z)
{
	SetLocalPosition(Vector3(pos, z));
}

void Transform::TranslateLocal2D(Vector2 offset)
{
	TranslateLocal(Vector3(offset, 0.f));
}

void Transform::SetRotationEuler2D(float rotation)
{
	SetRotationEuler(Vector3(0.f,0.f,rotation));
}

void Transform::RotateByEuler2D(float rotationOffset)
{
	RotateByEuler(Vector3(0.f,0.f,rotationOffset));
}

void Transform::SetScale2D(Vector2 s)
{
	SetScale(Vector3(s,1.f));
}

Vector3 Transform::GetForward() const
{
	return GetWorldMatrix().GetK().XYZ().GetNormalized();
}

Vector3 Transform::GetRight() const
{
	return GetWorldMatrix().GetI().XYZ().GetNormalized();
}

Vector3 Transform::GetUp() const
{
	return GetWorldMatrix().GetJ().XYZ().GetNormalized();
}

void Transform::LookAt(const Vector3 & position, const Vector3 & target, const Vector3 & up)
{
	SetLocalMatrix(Matrix44::LookAt(position, target, up));
}

void Transform::SetParent(Transform * t)
{
	m_parent = t;
	t->AddChild(this);
	UpdateWorldMatrix();
}

void Transform::AddChild(Transform * t)
{
	m_children.push_back(t);
}

void Transform::SetMatrices()
{
	m_localMatrix = m_transformStruct.GetMatrix();
	UpdateWorldMatrix();
}

void Transform::UpdateWorldMatrix()
{
	m_worldMatrix = Matrix44::IDENTITY;
	if (m_parent != nullptr){
		m_worldMatrix = m_parent->GetWorldMatrix();
	}
	m_worldMatrix.Append(m_localMatrix);

	for(Transform* t : m_children){
		t->UpdateWorldMatrix();
	}
}