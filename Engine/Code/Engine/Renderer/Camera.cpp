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
	Vector3 barrelOffset = Vector3(0.f,0.f, coreLength * .5f + barrelLength * .5f );
	Vector3 lensOffset = Vector3(0.f,0.f, barrelOffset.z + (barrelLength * .5f));
	RGBA color = RGBA(255,255,255,128);
	MeshBuilder mb = MeshBuilder();
	Vector3 camPos = m_transform.GetWorldPosition();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(camPos					, Vector3(coreLength*.7f	,coreLength*.7f		, coreLength			) , color);		//the core
	mb.AppendCube(camPos + barrelOffset		, Vector3(barrelLength*.5f	,barrelLength*.5f	, barrelLength			) , color);		//the lens barrel (?)
	mb.AppendCube(camPos + lensOffset		, Vector3(barrelLength		,barrelLength		, barrelLength * .25f	) , color);		//the lens cap
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


void Camera::LookAt(Vector3 position, Vector3 target, Vector3 up)
{
	Matrix44 lookat = Matrix44::LookAt(position, target, up);
	m_transform.SetLocalMatrix(lookat);
	//m_viewMatrix = m_cameraMatrix;
	m_viewMatrix = InvertFast( lookat );
}

void Camera::SetProjection(Matrix44 proj)
{
	m_projMatrix.Load(proj);
}

void Camera::SetProjectionOrtho(float size, float aspect, float nearVal, float farVal)
{
	float width = size * aspect;
	m_orthographicSize = size;
	m_bounds = AABB2(0.f,0.f, width, size);
	SetProjection(Matrix44::MakeOrtho3D(Vector3(0.f,0.f,nearVal), Vector3(width, size, farVal)));
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

void Camera::AddSkybox(const char* skyboxTexture)
{
	m_skybox = new Skybox(skyboxTexture);
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
