#include "Transform2D.hpp"

Matrix44 transform2D_t::GetMatrix() const
{
	Matrix44 mat = Matrix44();
	mat.SetIdentity();
	mat.Translate2D(m_position);
	mat.RotateDegrees2D(m_rotation);
	mat.Scale2D(m_scale.x, m_scale.y);
	return mat;
}

void transform2D_t::SetMatrix(Matrix44 const & mat)
{
	m_scale = mat.GetScale().XY();
	m_rotation = mat.GetEulerAngles().z;
	m_position = mat.GetPosition().XY();

}

void transform2D_t::SetPosition(Vector2 pos)
{
	m_position = pos;
}

void transform2D_t::Translate(Vector2 offset)
{
	m_position+=offset;
}

Vector2 transform2D_t::GetPosition() const
{
	return m_position;
}

void transform2D_t::SetRotationEuler(float euler)
{
	m_rotation = euler;
}

void transform2D_t::RotateByEuler(float euler)
{
	m_rotation+=euler;
	//m_euler.x = ClampFloat( m_euler.x, -90.f, 90.f ); 
	//m_euler.y = modf( m_euler.y, 2.0f * PI ); 

}

float transform2D_t::GetEulerAngles() const
{
	return m_rotation;
}

void transform2D_t::SetScale(Vector2 s)
{
	m_scale = s;
}

Vector2 transform2D_t::GetScale() const
{
	return m_scale;
}

Transform2D::Transform2D()
{
	m_transformStruct = transform2D_t();
	m_localMatrix = Matrix44();
	m_localMatrix.SetIdentity();
	m_parent = nullptr;
	m_children = std::vector<Transform2D*>();
}

Transform2D::~Transform2D()
{
	m_children.clear();
}

Matrix44 Transform2D::GetWorldMatrix() const
{
	//m_worldMatrix = Matrix44(m_localMatrix);
	//if (m_parent != nullptr){
	//	Matrix44 parent = m_parent->GetWorldMatrix();
	//	parent.Append(m_worldMatrix);
	//	return parent;
	//}
	return m_worldMatrix;
}

Matrix44 Transform2D::GetLocalMatrix() const
{
	return m_localMatrix;
}

void Transform2D::SetLocalMatrix(Matrix44 const & mat)
{
	m_transformStruct.SetMatrix(mat);
	SetMatrices();
}

void Transform2D::SetLocalPosition(Vector2 pos)
{
	m_transformStruct.SetPosition(pos);
	SetMatrices();
}

void Transform2D::TranslateLocal(Vector2 offset)
{
	m_transformStruct.Translate(offset);
	SetMatrices();
}

Vector2 Transform2D::GetWorldPosition() const
{
	Matrix44 world = GetWorldMatrix();
	return world.GetPosition().XY();
}

Vector2 Transform2D::GetLocalPosition() const
{
	return m_transformStruct.GetPosition();
}

float Transform2D::GetRotation() const
{
	return m_transformStruct.GetEulerAngles();
}

void Transform2D::SetRotationEuler(float euler)
{
	m_transformStruct.SetRotationEuler(euler);

	SetMatrices();
}

void Transform2D::RotateByEuler(float euler)
{
	m_transformStruct.RotateByEuler(euler);
	SetMatrices();
}


void Transform2D::SetScale(Vector2 s)
{
	m_transformStruct.SetScale(s);
	SetMatrices();
}

Vector2 Transform2D::GetScale() const
{
	return m_transformStruct.GetScale();
}

//void Transform2D::SetLocalPosition2D(Vector2 pos, float z)
//{
//	SetLocalPosition(Vector3(pos, z));
//}
//
//void Transform2D::TranslateLocal2D(Vector2 offset)
//{
//	TranslateLocal(Vector3(offset, 0.f));
//}
//
//void Transform2D::SetRotationEuler2D(float rotation)
//{
//	SetRotationEuler(Vector3(0.f,0.f,rotation));
//}
//
//void Transform2D::RotateByEuler2D(float rotationOffset)
//{
//	RotateByEuler(Vector3(0.f,0.f,rotationOffset));
//}
//
//void Transform2D::SetScale2D(Vector2 s)
//{
//	SetScale(Vector3(s,1.f));
//}

//Vector3 Transform2D::GetForward() const
//{
//	return GetWorldMatrix().GetK().XYZ().GetNormalized();
//}

Vector3 Transform2D::GetRight() const
{
	return GetWorldMatrix().GetI().XYZ().XY().GetNormalized();
}

Vector3 Transform2D::GetUp() const
{
	return GetWorldMatrix().GetJ().XYZ().XY().GetNormalized();
}

void Transform2D::LookAt(const Vector3 & position, const Vector3 & target, const Vector3 & up)
{
	SetLocalMatrix(Matrix44::LookAt(position, target, up));
}

void Transform2D::SetParent(Transform2D * t)
{
	m_parent = t;
	t->AddChild(this);
	UpdateWorldMatrix();
}

void Transform2D::AddChild(Transform2D * t)
{
	m_children.push_back(t);
}

void Transform2D::SetMatrices()
{
	m_localMatrix = m_transformStruct.GetMatrix();
	UpdateWorldMatrix();
}

void Transform2D::UpdateWorldMatrix()
{
	m_worldMatrix = Matrix44::IDENTITY;
	if (m_parent != nullptr){
		m_worldMatrix = m_parent->GetWorldMatrix();
	}
	m_worldMatrix.Append(m_localMatrix);

	for(Transform2D* t : m_children){
		t->UpdateWorldMatrix();
	}
}