#include "DebugRenderTask.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/Game.hpp"


//DebugCamera::DebugCamera(Transform camT) 
//{
//	float coreLength = .8f;
//	float barrelLength = .4f;
//	Vector3 barrelOffset = Vector3(0.f,0.f, coreLength * .5f + barrelLength * .5f );
//	Vector3 lensOffset = Vector3(0.f,0.f, barrelOffset.z + (barrelLength * .5f));
//	RGBA color = RGBA(255,255,255,128);
//	MeshBuilder mb = MeshBuilder();
//	Vector3 camPos = camT.GetLocalPosition();
//	mb.Begin(PRIMITIVE_TRIANGLES, true);
//	mb.AppendCube(camPos					, Vector3(coreLength*.7f	,coreLength*.7f		, coreLength			) , color);		//the core
//	mb.AppendCube(camPos + barrelOffset		, Vector3(barrelLength*.5f	,barrelLength*.5f	, barrelLength			) , color);		//the lens barrel (?)
//	mb.AppendCube(camPos + lensOffset		, Vector3(barrelLength		,barrelLength		, barrelLength * .25f	) , color);		//the lens cap
//	mb.End();
//	m_mesh = mb.CreateMesh(VERTEX_TYPE_3DPCU);
//
//	SetTransform(camT);
//
//}
//
//DebugCamera::~DebugCamera()
//{
//}
//
//void DebugCamera::Render()
//{
//	g_theRenderer->BindShader("wireframe");
//	Entity::Render();
//	g_theRenderer->ReleaseShader();
//}


DebugRenderTask::DebugRenderTask(float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
{
	m_info = DebugRenderInfo(lifetime, startColor, endColor, mode);
}

void DebugRenderTask::Age(float ds)
{
	if (m_info.m_lifetime >= 0.f){
		m_info.m_age+=ds;
	}
}

bool DebugRenderTask::IsDead() const
{

	if (m_info.m_lifetime >= 0.f){
		bool dead = m_info.m_age > m_info.m_lifetime;
		return dead;
	} 
	return false;
}

DebugRenderTask_Point::DebugRenderTask_Point(Vector3 pos, float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
	: DebugRenderTask(lifetime, startColor, endColor, mode)
{
	m_position = pos;
}

void DebugRenderTask_Point::Render()
{

	RGBA color = Interpolate(m_info.m_startColor, m_info.m_endColor, m_info.m_age / m_info.m_lifetime);
	g_theRenderer->DrawPoint(m_position, .1f, RIGHT, UP, FORWARD, color);

	//g_theRenderer->DrawPoint(m_position, 1.f,RIGHT, UP, Vector3::FORWARD, RGBA::WHITE);
	
}

DebugRenderTask_Basis::DebugRenderTask_Basis(Vector3 pos, Matrix44 basis, float size, float lifetime, DebugRenderMode mode)
	: DebugRenderTask_Point(pos, lifetime, RGBA::WHITE, RGBA::WHITE, mode)
{
	m_size = size;
	m_right = basis.GetI().XYZ();
	m_up = basis.GetJ().XYZ();
	m_forward = basis.GetK().XYZ();
}

DebugRenderTask_Basis::DebugRenderTask_Basis(Vector3 pos, Vector3 right, Vector3 up, Vector3 forward, float size, float lifetime, DebugRenderMode mode)
	: DebugRenderTask_Point(pos, lifetime, RGBA::WHITE, RGBA::WHITE, mode)
{
	m_size = size;
	m_right = right;
	m_up = up;
	m_forward = forward;
}

void DebugRenderTask_Basis::Render()
{
	RGBA color = m_info.m_startColor;
	RGBA rightColor		= RGBA::RED;
	RGBA upColor		= RGBA::GREEN;
	RGBA forwardColor	= RGBA::BLUE;
	if (m_info.m_mode == DEBUG_RENDER_XRAY || m_info.m_mode == DEBUG_RENDER_HIDDEN)
	{
		rightColor = rightColor.GetColorWithAlpha(80);
		upColor = upColor.GetColorWithAlpha(80);
		forwardColor = forwardColor.GetColorWithAlpha(80);
	}

	g_theRenderer->DrawLine3D(m_position, m_position + (m_right * m_size), rightColor, rightColor);
	g_theRenderer->DrawLine3D(m_position, m_position + (m_up * m_size),	upColor, upColor);
	g_theRenderer->DrawLine3D(m_position, m_position + (m_forward * m_size), forwardColor, forwardColor);

	//g_theRenderer->DrawPoint(m_position, m_size, m_right, m_up, m_forward, right, up, forward);
}

DebugRenderTask_Quad::DebugRenderTask_Quad(Vector3 center, Vector2 size, Vector3 right, Vector3 up, float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
	:DebugRenderTask(lifetime, startColor, endColor, mode)
{
	m_plane = Plane(center, up, right, size);
}



void DebugRenderTask_Quad::Render()
{
	RGBA color = Interpolate(m_info.m_startColor, m_info.m_endColor, m_info.m_age / m_info.m_lifetime);
	g_theRenderer->DrawPlane(m_plane, color);
}

DebugRenderTask_Sphere::DebugRenderTask_Sphere(Vector3 center, float radius, int wedges, int slices, float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
	:DebugRenderTask(lifetime, startColor, endColor, mode)
{
	m_sphere = Sphere(center, radius, wedges, slices);
}

void DebugRenderTask_Sphere::Render()
{
	RGBA color = Interpolate(m_info.m_startColor, m_info.m_endColor, m_info.m_age / m_info.m_lifetime);
	//g_theRenderer->UseShader("wireframe");
	g_theRenderer->EnableFillMode(FILL_MODE_WIRE);

	g_theRenderer->DrawSphere(m_sphere, color);
	g_theRenderer->ReleaseShader();
}

DebugRenderTask_AABB3::DebugRenderTask_AABB3(Vector3 center, float size, float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
	:DebugRenderTask(lifetime, startColor, endColor, mode)
{
	m_box = AABB3(center, size, size, size);
}

DebugRenderTask_AABB3::DebugRenderTask_AABB3(const AABB3 & box, float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
	: DebugRenderTask(lifetime, startColor, endColor, mode)
{
	m_box = box;
}

void DebugRenderTask_AABB3::Render()
{
	RGBA color = Interpolate(m_info.m_startColor, m_info.m_endColor, m_info.m_age / m_info.m_lifetime);
	g_theRenderer->EnableFillMode(FILL_MODE_WIRE);
	g_theRenderer->DrawCube(m_box.GetCenter(), m_box.GetDimensions(), color);
	g_theRenderer->ReleaseShader();
}

DebugRenderTask_AABB2::DebugRenderTask_AABB2(AABB2 bounds, float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
	:DebugRenderTask(lifetime, startColor, endColor, mode)
{
	m_bounds = bounds;
}

void DebugRenderTask_AABB2::Render()
{
	RGBA color = Interpolate(m_info.m_startColor, m_info.m_endColor, m_info.m_age / m_info.m_lifetime);
	g_theRenderer->DrawAABB2(m_bounds, color);
}

DebugRenderTask_LINE2::DebugRenderTask_LINE2(Vector2 start, RGBA p0Color, Vector2 end, RGBA p1Color, float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
	:DebugRenderTask(lifetime, startColor, endColor, mode)
{
	m_start = start;
	m_p0Color = p0Color;
	m_end = end;
	m_p1Color = p1Color;
}

void DebugRenderTask_LINE2::Render()
{
	RGBA tint = Interpolate(m_info.m_startColor, m_info.m_endColor, m_info.m_age / m_info.m_lifetime);
	RGBA p0 = Interpolate(m_p0Color, tint, .5f);
	RGBA p1 = Interpolate(m_p1Color, tint, .5f);
	g_theRenderer->DrawLine2D(m_start, m_end, p0, p1);
}

DebugRenderTask_TEXT2::DebugRenderTask_TEXT2(std::string text, Vector2 pos, float size, float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
	:DebugRenderTask(lifetime, startColor, endColor, mode)
{
	m_text = text;
	m_pos = pos;
	m_size = size;
}

void DebugRenderTask_TEXT2::Render()
{
	RGBA color = Interpolate(m_info.m_startColor, m_info.m_endColor, m_info.m_age / m_info.m_lifetime);
	AABB2 screenBox = g_theGame->GetUIBounds();
	g_theRenderer->DrawTextInBox2D(m_text, screenBox, m_pos, m_size, TEXT_DRAW_SHRINK_TO_FIT, color);
}

DebugRenderTask_Line3::DebugRenderTask_Line3(Vector3 p0, Vector3 p1, RGBA c0, RGBA c1, float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
	:DebugRenderTask(lifetime, startColor, endColor, mode)
{
	m_p0 = p0;
	m_p1 = p1;
	m_c0 = c0;
	m_c1 = c1;
}

void DebugRenderTask_Line3::Render()
{
	RGBA tint = Interpolate(m_info.m_startColor, m_info.m_endColor, m_info.m_age / m_info.m_lifetime);
	RGBA c0 = Interpolate(m_c0, tint, .5f);
	RGBA c1 = Interpolate(m_c1, tint, .5f);
	g_theRenderer->DrawLine3D(m_p0, m_p1, c0, c1);
}

DebugRenderTask_Text3::DebugRenderTask_Text3(std::string text, Vector3 center, float size, float lifetime, RGBA startColor, RGBA endColor, Vector3 up, Vector3 right, DebugRenderMode mode)
	:DebugRenderTask(lifetime, startColor, endColor, mode)
{
	m_text = text;
	m_centerPivot = center;
	m_size = size;

	m_up = up;
	m_right = right;
}

void DebugRenderTask_Text3::Render()
{
	RGBA tint = Interpolate(m_info.m_startColor, m_info.m_endColor, m_info.m_age / m_info.m_lifetime);
	//g_theRenderer->DrawText3D(m_text, m_centerPivot,  m_size,  m_up, m_right,tint);
	g_theRenderer->DrawTextAsSprite(m_text, m_centerPivot, Vector2::HALF, m_size, m_right, m_up, tint);
}

DebugRenderTask_Ruler::DebugRenderTask_Ruler(Vector3 p0, Vector3 p1, RGBA c0, RGBA c1, float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
	:DebugRenderTask_Line3(p0, p1, c0, c1, lifetime, startColor, endColor, mode)
{

	m_distance = GetDistance(p0, p1);
	m_midPoint = (p0 + p1) * .5f;
}

void DebugRenderTask_Ruler::Render()
{
	std::string distance = std::to_string(m_distance);
	distance.resize(6);
	
	RGBA tint = Interpolate(m_info.m_startColor, m_info.m_endColor, m_info.m_age / m_info.m_lifetime);
	RGBA c0 = Interpolate(m_c0, tint, .5f);
	RGBA c1 = Interpolate(m_c1, tint, .5f);
	RGBA midPointColor = Interpolate(c0, c1, .5f);

	g_theRenderer->DrawPoint(m_p0, .05f, RIGHT, UP, FORWARD, c0);
	g_theRenderer->DrawPoint(m_p1, .05f, RIGHT, UP, FORWARD, c1);
	g_theRenderer->DrawPoint(m_midPoint, .05f, RIGHT, UP, FORWARD, midPointColor);
	g_theRenderer->DrawTextAsSprite(distance, m_p1 + UP * .02f, Vector2::HALF, .05f, g_theGame->GetCurrentCameraRight(), g_theGame->GetCurrentCameraUp(), RGBA::WHITE);
	g_theRenderer->DrawLine3D(m_p0, m_p1, c0, c1);

}

void DebugRenderTask_Ruler::SetEndpoint(Vector3 newPoint)
{
	m_p1 = newPoint;
	m_distance = GetDistance(m_p0, m_p1);
	m_midPoint = (m_p0 + m_p1) * .5f;
}

void DebugRenderTask_Ruler::SetLifetimeAndReset(float lifeTime)
{
	m_info.m_lifetime = lifeTime;
	m_info.m_age = 0.f;
}

DebugRenderTask_Tag::DebugRenderTask_Tag(std::string text, Vector3 center, float size, float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
	:DebugRenderTask(lifetime, startColor, endColor, mode)
{
	m_text = text;
	m_centerPivot = center;
	m_size = size;

	m_up = g_theGame->GetCurrentCameraUp();
	m_right = g_theGame->GetCurrentCameraRight();
	
}

void DebugRenderTask_Tag::Render()
{
	m_up = g_theGame->GetCurrentCameraUp();
	m_right = g_theGame->GetCurrentCameraRight();
	RGBA tint = Interpolate(m_info.m_startColor, m_info.m_endColor, m_info.m_age / m_info.m_lifetime);
	g_theRenderer->DrawTextAsSprite(m_text, m_centerPivot, Vector2::HALF, m_size, m_right, m_up, tint);
}
