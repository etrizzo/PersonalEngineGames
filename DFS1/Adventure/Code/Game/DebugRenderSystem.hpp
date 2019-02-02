#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"

enum DebugRenderMode 
{
	DEBUG_RENDER_IGNORE_DEPTH	, // will always draw and be visible 
	DEBUG_RENDER_USE_DEPTH		, // draw using normal depth rules
	DEBUG_RENDER_HIDDEN			, // only draws if it would be hidden by depth
	DEBUG_RENDER_XRAY			, // always draws, but hidden area will be drawn differently
};

enum RenderTaskType
{
	RENDER_TASK_AABB2	,		//2D tasks
	RENDER_TASK_LINE2	,
	RENDER_TASK_TEXT2	,
	NUM_2D_TASKS		,

	RENDER_TASK_POINT	,
	RENDER_TASK_BASIS	,
	RENDER_TASK_QUAD	,
	RENDER_TASK_SPHERE	,
	RENDER_TASK_AABB3	,
	RENDER_TASK_LINE3	,
	RENDER_TASK_RULER	,		//line3D but with endpoints/midpoint render, and distance rendered as text
	RENDER_TASK_TEXT3	,
	RENDER_TASK_TAG		,
	NUM_RENDER_TASKS
};

struct DebugRenderInfo{
	DebugRenderInfo();
	DebugRenderInfo(float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode);
	float m_lifetime;
	float m_age;
	RGBA m_startColor;
	RGBA m_endColor;
	DebugRenderMode m_mode;
};



class PerspectiveCamera;
class Camera;
class DebugRenderTask;
class DebugRenderTask_Ruler;



class DebugRenderSystem{
public:
	DebugRenderSystem(){};
	~DebugRenderSystem();

	//want to start debug render mode from the current camera, i guess
	void Startup(Camera* currentCamera);
	void Shutdown();

	void DetachCamera();		//detaches debug render camera to be able to fly around and junk
	void ReattachCamera();			//reattaches to game camera

	void UpdateAndRender();
	void HandleInput();
	void HandleCameraInput();
	void SetCamera();

	void AddCurrent3DTask();
	void AddCurrent2DTask();
	void SetCurrentTask(RenderTaskType newType);
	void SetDrawMode(DebugRenderMode newMode);

	void SetDefaultColor(RGBA start, RGBA end);
	void SetDefaultLifetime(float lifetime);


	bool IsActive() { return m_isActive; };
	bool IsRendering() {return m_isRendering; };
	void ToggleRendering();
	void Clear();
	PerspectiveCamera* m_camera;
	PerspectiveCamera* m_gameCamera;
	bool m_isDetached = false;

	Vector3 m_currentCameraEuler = Vector3::ZERO;
	bool m_isActive = false;
	bool m_isRendering = false;

	RGBA m_defaultStartColor = RGBA::RED;
	RGBA m_defaultEndColor   = RGBA::GREEN;
	float m_defaultLifetime  = 5.f;

	RenderTaskType m_current3DTask = RENDER_TASK_AABB3;
	RenderTaskType m_current2DTask = RENDER_TASK_TEXT2;
	DebugRenderMode m_currentMode = DEBUG_RENDER_USE_DEPTH;

	void ToggleInfo() { m_showInfo = !m_showInfo; };
	void ToggleScreenGrid() {m_showScreenSpace = !m_showScreenSpace; };
	

	/************************************************************************/
	/* 2D Systems                                                           */
	/************************************************************************/

	void MakeDebugRender2DQuad( float lifetime, 
		AABB2 const &bounds = AABB2::ZERO_TO_ONE, 
		RGBA const &start_color = RGBA::RED, 
		RGBA const &end_color = RGBA::GREEN ); 

	void MakeDebugRenderCircle( float lifetime, 
		Disc2 const &circle, bool dottedLine,
		DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH,
		RGBA const &start_color = RGBA::RED, 
		RGBA const &end_color = RGBA::GREEN ); 

	void MakeDebugRenderCircle( float lifetime, 
		const Vector2& center, float radius, bool dottedLine,
		DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH,
		RGBA const &start_color = RGBA::RED, 
		RGBA const &end_color = RGBA::GREEN ); 

	void MakeDebugRender2DLine( float lifetime, 
		Vector2 const &p0 = Vector2::ZERO, RGBA const &p0_color = RGBA::WHITE,
		Vector2 const &p1 = Vector2::ONE, RGBA const &p1_color = RGBA::WHITE,
		RGBA const &start_color = RGBA::RED, 
		RGBA const &end_color = RGBA::GREEN ); 

	void MakeDebugRender2DText( float lifetime, 
		std::string text = "2D Text",
		Vector2 position = Vector2::ZERO, 
		float scale = .1f, 
		RGBA const &start_color = RGBA::RED, 
		RGBA const &end_color = RGBA::GREEN ); 

	//functions for supporting formatted 2D text for debug rendering to screen 
	void MakeDebugRender2DText( float lifetime, 
		float scale, 
		Vector2 alignment, 
		RGBA const &start_color, 
		RGBA const &end_color, 
		char const *format, 
		va_list args ); 

	void MakeDebugRender2DText( float lifetime, 
		float scale, 
		Vector2 alignment, 
		RGBA const &start_color, 
		RGBA const &end_color, 
		char const *format, 
		... ); 

	void MakeDebugRender2DText( float lifetime, 
		float scale, 
		Vector2 alignment, 
		RGBA const &color, 
		char const *format, 
		... ); 

	void MakeDebugRender2DText( float lifetime, 
		float scale, 
		Vector2 alignment, 
		char const *format, 
		... ); 

	//draws in top right in white
	void MakeDebugRender2DText(
		char const *format, 
		... ); 


	/************************************************************************/
	/* 3D Systems                                                           */
	/************************************************************************/

	void MakeDebugRenderPoint( float lifetime = 0.f, 
		Vector3 position = Vector3::ZERO, 
		RGBA const &start_color = RGBA::RED, 
		RGBA const &end_color = RGBA::GREEN,
		DebugRenderMode const mode = DEBUG_RENDER_USE_DEPTH); 

	void MakeDebugRenderLineSegment( Vector3 const& p0, Vector3 const& p1,
		RGBA const &p0_color = RGBA::WHITE, 
		RGBA const &p1_color = RGBA::WHITE, 
		float lifetime = 0.f, 
		RGBA const &start_color = RGBA::WHITE, 
		RGBA const &end_color = RGBA::WHITE, 
		DebugRenderMode const mode = DEBUG_RENDER_USE_DEPTH );

	void MakeDebugRenderBasis( float lifetime, 
		Vector3 position = Vector3::ZERO, 
		Matrix44 const &basis = Matrix44::IDENTITY, 
		RGBA const &start_color = RGBA::RED, 
		RGBA const &end_color = RGBA::GREEN, 
		DebugRenderMode const mode = DEBUG_RENDER_USE_DEPTH); 

	void MakeDebugRenderSphere( float lifetime, 
		Vector3 pos = Vector3::ZERO, 
		float const radius = 1.f, 
		int const wedges = 10,
		int const slices = 10,
		RGBA const &start_color = RGBA::RED, 
		RGBA const &end_color = RGBA::GREEN, 
		DebugRenderMode const mode = DEBUG_RENDER_USE_DEPTH); 

	void MakeDebugRenderWireAABB3( float lifetime, 
		Vector3 pos = Vector3::ZERO, 
		float const size = 1.f, 
		RGBA const &start_color = RGBA::RED, 
		RGBA const &end_color = RGBA::GREEN, 
		DebugRenderMode const mode = DEBUG_RENDER_USE_DEPTH); 

	void MakeDebugRenderQuad( float lifetime, 
		Vector3 center = Vector3::ZERO, 
		Vector2 size = Vector2::HALF,
		Vector3 right = RIGHT, Vector3 up = UP, 
		RGBA startColor = RGBA::RED, RGBA endColor = RGBA::GREEN, 
		DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);

	void MakeDebugRender3DText( std::string text ,
		float lifetime = 0.f, 
		Vector3 centerPos = Vector3::ZERO, 
		float scale = .1f, 
		Vector3 up = UP, Vector3 right = RIGHT,
		RGBA const &start_color = RGBA::RED, 
		RGBA const &end_color = RGBA::GREEN,
		DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);

	void MakeDebugRenderTag( std::string text ,
		float lifetime = 0.f, 
		Vector3 centerPos = Vector3::ZERO, 
		float scale = .1f, 
		RGBA const &start_color = RGBA::RED, 
		RGBA const &end_color = RGBA::GREEN,
		DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);

private:
	void Update();
	void Render();
	void Render3D();
	void Render2D();
	void RenderInfo();
	void DestroyTasks();
	void SetRendererDrawMode(DebugRenderTask* task);
	void ResetDepth();

	void StartRuler();
	void EndRuler();

	Clock* m_debugRenderClock;
	
	DebugRenderTask_Ruler* m_currentRuler = nullptr;

	std::vector<DebugRenderTask*> m_tasks3D;
	std::vector<DebugRenderTask*> m_tasks2D;
	bool m_showInfo = true;
	bool m_showScreenSpace = true;



	//keep track of all of the things to render/update
};


DebugRenderMode StringToDebugRenderMode(std::string mode);
RenderTaskType StringToDebugRenderTask(std::string task);

const char* GetTaskName(RenderTaskType type);
const char* GetTaskID(RenderTaskType type);
const char* GetTaskDescription(RenderTaskType type);