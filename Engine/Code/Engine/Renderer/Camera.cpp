#include "Camera.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

Camera::Camera()
{
	m_transform = Transform();
	m_projMatrix.LoadIdentity();
	m_viewMatrix.SetIdentity();
	m_orthographicSize = 1.f;

	//make debug mesh
	float coreLength = .8f;
	float barrelLength = .4f;
	Vector3 barrelOffset = GetForward() * (.5f * coreLength + barrelLength * .5f); //Vector3(0.f,0.f, coreLength * .5f + barrelLength * .5f );
	Vector3 lensOffset = GetForward() * ( barrelOffset.GetLength() + (barrelLength * .5f));
	RGBA color = RGBA(255,255,255,128);
	MeshBuilder mb = MeshBuilder();
	Vector3 camPos = m_transform.GetWorldPosition();
	Vector3 coreSize = (GetForward() * coreLength) + (GetRight() * coreLength * .7f) + (GetUp() * coreLength * .7f);
	Vector3 barrelSize = (GetForward() * barrelLength) + (GetRight() * barrelLength * .5f) + (GetUp() * barrelLength * .5f);
	Vector3 capSize = (GetForward() * barrelLength * .25f) + (GetRight() * barrelLength) + (GetUp() * barrelLength);
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(camPos					, coreSize		, color, m_transform.GetRight(), m_transform.GetUp(), m_transform.GetForward());		//the core
	mb.AppendCube(camPos + barrelOffset		, barrelSize	, color,  m_transform.GetRight(), m_transform.GetUp(), m_transform.GetForward());		//the lens barrel (?)
	mb.AppendCube(camPos + lensOffset		, capSize		, color,  m_transform.GetRight(), m_transform.GetUp(), m_transform.GetForward());		//the lens cap
	mb.End();
	m_debugmesh = mb.CreateSubMesh(VERTEX_TYPE_3DPCU);
}

Camera::~Camera()
{
	delete m_debugmesh;
}

void Camera::SetColorTarget(Texture * color_target)
{
	m_output.SetColorTarget(color_target);
}

void Camera::SetDepthStencilTarget(Texture * depth_target)
{
	m_output.SetDepthStencilTarget(depth_target);
}

Texture * Camera::GetDepthTarget()
{
	return m_output.m_depthStencilTarget;
}


void Camera::LookAt(Vector3 position, Vector3 target, Vector3 up)
{
	//Matrix44 lookat = Matrix44(worldBasis);
	Matrix44 lookat = Matrix44::LookAt(position, target, up);
	//lookat.Append(worldBasis);

	m_transform.SetLocalMatrix(lookat);
	//m_viewMatrix = m_cameraMatrix;
	m_viewMatrix = InvertFast( lookat );
}

void Camera::SetProjection(Matrix44 proj)
{
	m_projMatrix.Load(proj);
}

void Camera::SetProjectionOrtho(float height, float aspect, float nearVal, float farVal, const Vector2& screenMins)
{
	float width = height * aspect;
	m_orthographicSize = height;
	m_bounds = AABB2(screenMins, screenMins + Vector2(width, height));
	SetProjection(Matrix44::MakeOrtho3D(Vector3(screenMins,nearVal), Vector3(screenMins + Vector2(width, height), farVal)));
}



void Camera::SetProjectionOrtho(const Vector3 & nearBottomLeft, const Vector3 & farTopRight)
{
	m_orthographicSize = farTopRight.x - nearBottomLeft.x;
	m_bounds = AABB2(nearBottomLeft.XY(), farTopRight.XY());
	SetProjection(Matrix44::MakeOrtho3D(nearBottomLeft, farTopRight));
}

void Camera::SetProjectionOrtho(const Vector2 & nearBottomLeft, const Vector2 & farTopRight)
{
	m_orthographicSize = farTopRight.x - nearBottomLeft.x;
	m_bounds = AABB2(nearBottomLeft, farTopRight);
	SetProjection(Matrix44::MakeOrtho2D(nearBottomLeft, farTopRight));
}

Matrix44 Camera::GetProjectionMatrix()
{
	return m_projMatrix.GetTop();
}

void Camera::SetPosition(const Vector3 & position)
{
	m_transform.SetLocalPosition(position);
	m_viewMatrix = InvertFast( m_transform.GetWorldMatrix() );
}

void Camera::Translate(Vector3 translation)
{
	m_transform.TranslateLocal(translation);
	m_viewMatrix = InvertFast( m_transform.GetWorldMatrix() );
}

void Camera::Rotate(Vector3 eulerChange)
{
	m_transform.RotateByEuler(eulerChange);
	m_viewMatrix = InvertFast( m_transform.GetWorldMatrix() );
}

void Camera::Rotate(float yaw, float pitch, float roll)
{
	m_transform.RotateByEuler(yaw, pitch, roll);
	m_viewMatrix = InvertFast(m_transform.GetWorldMatrix());
}

void Camera::RotateAroundX(float x)
{
	Rotate(Vector3(x, 0.f,0.f));
}

void Camera::RotateAroundY(float y)
{
	Rotate(Vector3(0.f,y,0.f));
}

void Camera::RotateAroundZ(float z)
{
	Rotate(Vector3(0.f,0.f,z));
}

void Camera::AddSkybox(const char* skyboxTexture, const Vector3& right, const Vector3& up, const Vector3& forward)
{
	m_skybox = new Skybox(skyboxTexture, right, up, forward);
}


bool Camera::Finalize()
{
	return m_output.Finalize();
}

Vector3 Camera::GetPosition()
{
	return m_transform.GetWorldPosition();
}

Vector3 Camera::GetForward() const
{
	return m_transform.GetForward();
}

Vector3 Camera::GetRight() const
{
	return m_transform.GetRight();
}

Vector3 Camera::GetUp() const
{
	return m_transform.GetUp();
}

float Camera::GetOrthographicSize() const
{
	return m_orthographicSize;
}

Matrix44 Camera::GetViewMatrix()
{
	m_viewMatrix = InvertFast( m_transform.GetWorldMatrix() );
	return m_viewMatrix;
}

AABB2 Camera::GetBounds() const
{
	return m_bounds;
}

SubMesh* Camera::GetDebugMesh()
{
	//m_debugmesh->(m_transform);
	return m_debugmesh;
}
