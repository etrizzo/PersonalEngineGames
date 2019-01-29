#pragma once
#include "Game/DebugRenderSystem.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"



////a class for rendering a wireframe representation of the game's camera while in debug system
////just some cubes because nobody makes .objs of game cameras idk
//class DebugCamera: public Entity{
//public:
//	DebugCamera(Transform cameraTransform);
//	~DebugCamera();
//
//	void Render();
//};

class DebugRenderTask{
public:
	DebugRenderTask(float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	~DebugRenderTask(){};

	virtual void Age(float ds);
	virtual void Render() = 0;

	bool IsDead() const;

	DebugRenderInfo m_info;
};


/*
=============
3D TASKS
=============
*/

class DebugRenderTask_Point: public DebugRenderTask{
public:
	DebugRenderTask_Point(Vector3 pos, float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;

	Vector3 m_position;
};


class DebugRenderTask_Basis: public DebugRenderTask_Point{
public: 
	DebugRenderTask_Basis(Vector3 pos, Matrix44 basis, float size = 1.f, float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;
	float m_size = .1f;
	Vector3 m_right;
	Vector3 m_up;
	Vector3 m_forward;
};


class DebugRenderTask_Quad: public DebugRenderTask{
public:
	DebugRenderTask_Quad(Vector3 center, Vector2 size, Vector3 right = Vector3::RIGHT, Vector3 up = Vector3::UP, float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;

	Plane m_plane;
};

class DebugRenderTask_Sphere: public DebugRenderTask{
public:
	DebugRenderTask_Sphere(Vector3 center, float radius, int wedges, int slices, float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;

	Sphere m_sphere;
};

class DebugRenderTask_AABB3: public DebugRenderTask{
public:
	DebugRenderTask_AABB3(Vector3 center, float size, float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;

	AABB3 m_cube;
};

class DebugRenderTask_Line3: public DebugRenderTask{
public:
	DebugRenderTask_Line3(Vector3 p0, Vector3 p1, RGBA c0, RGBA c1,float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;

	Vector3 m_p0;
	Vector3 m_p1;
	RGBA m_c0;
	RGBA m_c1;
};

class DebugRenderTask_Ruler: public DebugRenderTask_Line3{
public:
	DebugRenderTask_Ruler(Vector3 p0, Vector3 p1, RGBA c0, RGBA c1,float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;

	void SetEndpoint(Vector3 newPoint);
	void SetLifetimeAndReset(float lifeTime);

	float m_distance;
	Vector3 m_midPoint;
};

class DebugRenderTask_Text3: public DebugRenderTask{
public:
	DebugRenderTask_Text3(std::string text, Vector3 center, float size, float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, Vector3 up = Vector3::UP, Vector3 right = Vector3::RIGHT,  DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;

	std::string m_text;
	Vector3 m_centerPivot;
	float m_size;

	Vector3 m_up;
	Vector3 m_right;
};

class DebugRenderTask_Tag: public DebugRenderTask{
public:
	DebugRenderTask_Tag(std::string text, Vector3 center, float size, float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE,  DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;

	std::string m_text;
	Vector3 m_centerPivot;
	float m_size;

	Vector3 m_up;
	Vector3 m_right;
};





/*
=============
  2D TASKS
=============
*/


class DebugRenderTask_AABB2: public DebugRenderTask{
public:
	DebugRenderTask_AABB2(AABB2 m_bounds = AABB2::ZERO_TO_ONE, float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;

	AABB2 m_bounds;
};

class DebugRenderTask_LINE2: public DebugRenderTask{
public:
	DebugRenderTask_LINE2(Vector2 start = Vector2::ZERO, RGBA p0Color = RGBA::WHITE, Vector2 end = Vector2::ONE, RGBA p1Color = RGBA::WHITE,float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;

	Vector2 m_start;
	RGBA m_p0Color;
	Vector2 m_end;
	RGBA m_p1Color;
};

class DebugRenderTask_TEXT2: public DebugRenderTask{
public:
	DebugRenderTask_TEXT2(std::string text, Vector2 pivot = Vector2::ONE, float size = .1f, float lifetime = 0.f, RGBA startColor = RGBA::WHITE, RGBA endColor = RGBA::WHITE, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);
	void Render() override;

	std::string m_text;
	Vector2 m_pos;
	float m_size;
};