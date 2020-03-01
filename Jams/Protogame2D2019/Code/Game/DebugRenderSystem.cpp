#include "DebugRenderSystem.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"
#include "Game/DebugRenderTask.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/RenderScene2D.hpp"



DebugRenderSystem::~DebugRenderSystem()
{
}

void DebugRenderSystem::Startup(Camera * currentCamera)
{

	m_debugRenderClock = new Clock(GetMasterClock());
	m_isActive = true;
	m_isRendering = true;
	m_gameCamera = (PerspectiveCamera* ) currentCamera;
}

void DebugRenderSystem::Shutdown()
{
	ReattachCamera();
	//delete m_camera;
	delete[] m_tasks3D.data();

	if (m_debugRenderClock){
		delete m_debugRenderClock;
	}
	m_isActive = false;
	m_isRendering = false;
	
}

void DebugRenderSystem::DetachCamera()
{
	if (!m_isDetached){
		m_isDetached = true;

		m_camera = new PerspectiveCamera(m_gameCamera);
		m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
		m_camera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );
		g_theGame->SetGameCamera(m_camera);
		g_theRenderer->BindCamera(m_camera);
		if (g_theGame->GetScene() != nullptr){
			g_theGame->GetScene()->RemoveCamera(m_gameCamera);
			g_theGame->GetScene()->AddCamera(m_camera);
		}
	}

}

void DebugRenderSystem::ReattachCamera()
{
	if (m_isDetached){
		m_isDetached = false;

		g_theGame->SetMainCamera();
		if (g_theGame->GetScene() != nullptr){
			g_theGame->GetScene()->RemoveCamera(m_camera);
			g_theGame->GetScene()->AddCamera(m_gameCamera);
		}
		delete m_camera;
	}
}

void DebugRenderSystem::UpdateAndRender()
{
	if (m_isActive){
		Update();
		Render();
		/*if (m_isRendering){
			Render();
		}*/
	}
	DestroyTasks();
}

void DebugRenderSystem::HandleInput()
{
	if (g_theInput->WasKeyJustPressed('X') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_A)){
		AddCurrent3DTask();
	}
	if (g_theInput->WasKeyJustPressed('C') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_Y)){
		AddCurrent2DTask();
	}
	if (g_theInput->WasKeyJustPressed(VK_F1) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_BACK)){
		ToggleInfo();
	}

	if (m_isDetached){
		HandleCameraInput();
	}
}

void DebugRenderSystem::HandleCameraInput()
{
	float ds = m_debugRenderClock->GetDeltaSeconds();

	float degPerSecond = 60.f;
	Vector3 rotation = Vector3::ZERO;

	if (g_theInput->IsKeyDown(VK_UP)){
		rotation.x += 1.f;
	}
	if (g_theInput->IsKeyDown(VK_DOWN)){
		rotation.x -=1.f;
	}
	if (g_theInput->IsKeyDown(VK_RIGHT)){
		rotation.y += 1.f;
	}
	if (g_theInput->IsKeyDown(VK_LEFT)){
		rotation.y -=1.f;
	}

	Vector2 controllerRotation = g_theInput->GetController(0)->GetRightThumbstickCoordinates();
	rotation+=Vector3(controllerRotation.y, controllerRotation.x, 0.f);

	m_camera->Rotate(rotation * degPerSecond * ds);


	//why are there 6 keys if there are only 4 directions
	float speed = 4.f;
	if (g_theInput->IsKeyDown('D')){
		m_camera->Translate(m_camera->GetRight() * ds * speed);
	}
	if (g_theInput->IsKeyDown('A')){
		m_camera->Translate(m_camera->GetRight() * ds * speed * -1.f);
	}
	if (g_theInput->IsKeyDown('W')){
		m_camera->Translate(m_camera->GetForward() * ds* speed );
	}
	if (g_theInput->IsKeyDown('S')){
		m_camera->Translate(m_camera->GetForward() * ds * speed * -1.f);
	}
	if (g_theInput->IsKeyDown('E') || g_theInput->GetController(0)->IsButtonDown(XBOX_BUMPER_RIGHT)){
		m_camera->Translate(UP * ds* speed );		//going off of camera's up feels very weird when it's not perfectly upright
	}
	if (g_theInput->IsKeyDown('Q') || g_theInput->GetController(0)->IsButtonDown(XBOX_BUMPER_LEFT)){
		m_camera->Translate(UP * ds * speed * -1.f);
	}

	Vector2 controllerTranslation = g_theInput->GetController(0)->GetLeftThumbstickCoordinates();

	m_camera->Translate(m_camera->GetForward() * controllerTranslation.y * ds * speed);
	m_camera->Translate(m_camera->GetRight() * controllerTranslation.x * ds * speed);

	
}

void DebugRenderSystem::SetCamera()
{
	g_theRenderer->SetCamera( m_camera ); 
}

void DebugRenderSystem::AddCurrent3DTask()
{
	Vector3 drawPos;
	if (m_camera != nullptr){
		drawPos = m_camera->GetPosition() + (m_camera->GetForward() * 5.f);
	} else {
		drawPos = g_theGame->m_currentCamera->GetPosition() + (g_theGame->m_currentCamera->GetForward() * 5.f);
	}

	switch (m_current3DTask){
	case RENDER_TASK_POINT:
		MakeDebugRenderPoint(m_defaultLifetime, drawPos, m_defaultStartColor, m_defaultEndColor, m_currentMode);
		break;
	case RENDER_TASK_BASIS:
		MakeDebugRenderBasis(m_defaultLifetime, drawPos, Matrix44::IDENTITY, m_defaultStartColor, m_defaultEndColor, m_currentMode);
		break;
	case RENDER_TASK_QUAD:
		MakeDebugRenderQuad(m_defaultLifetime, drawPos, Vector2::ONE * .3f, RIGHT, UP, m_defaultStartColor, m_defaultEndColor, m_currentMode);
		break;
	case RENDER_TASK_SPHERE:
		MakeDebugRenderSphere(m_defaultLifetime, drawPos, .5f, 10, 10, m_defaultStartColor, m_defaultEndColor, m_currentMode);
		break;
	case RENDER_TASK_AABB3:
		MakeDebugRenderWireAABB3(m_defaultLifetime, drawPos, .2f, m_defaultStartColor, m_defaultEndColor, m_currentMode);
		break;
	case RENDER_TASK_TEXT3:
		MakeDebugRender3DText("OwO", m_defaultLifetime, drawPos, .1f, m_camera->GetUp(), m_camera->GetRight(), m_defaultStartColor, m_defaultEndColor, m_currentMode);
		break;
	case RENDER_TASK_TAG:
		MakeDebugRenderTag("Tag!", m_defaultLifetime, drawPos, .1f,  m_defaultStartColor, m_defaultEndColor, m_currentMode);
		break;
	case RENDER_TASK_LINE3:
	case RENDER_TASK_RULER:			//when drawing line3D, press x for startPoint, and x for endpoint to draw a ruler
		if (nullptr == m_currentRuler){
			StartRuler();
		} else {
			EndRuler();
		}
	}
}

void DebugRenderSystem::AddCurrent2DTask()
{
	switch (m_current2DTask){
	case RENDER_TASK_AABB2:
		MakeDebugRender2DQuad(m_defaultLifetime, AABB2::ZERO_TO_ONE, m_defaultStartColor, m_defaultEndColor);
		break;
	case RENDER_TASK_LINE2:
		MakeDebugRender2DLine(m_defaultLifetime, Vector2::ZERO, RGBA::WHITE, Vector2::ONE, RGBA::WHITE, m_defaultStartColor, m_defaultEndColor);
		break;
	case RENDER_TASK_TEXT2:
		MakeDebugRender2DText(m_defaultLifetime, ":^)", Vector2::ZERO, .1f, m_defaultStartColor, m_defaultEndColor);
		break;
	}
}

void DebugRenderSystem::SetCurrentTask(RenderTaskType newType)
{
	if (newType < NUM_2D_TASKS){
		m_current2DTask = newType;
	} else {
		m_current3DTask = newType;
	}
}

void DebugRenderSystem::SetDrawMode(DebugRenderMode newMode)
{
	m_currentMode = newMode;
}

void DebugRenderSystem::SetDefaultColor(RGBA start, RGBA end)
{
	m_defaultStartColor = start;
	m_defaultEndColor = end;
}

void DebugRenderSystem::SetDefaultLifetime(float lifetime)
{
	m_defaultLifetime = lifetime;
}

void DebugRenderSystem::ToggleRendering()
{
	m_isRendering = !m_isRendering;
}

void DebugRenderSystem::Clear()
{
	//kills all tasks effectively
	for(DebugRenderTask* task : m_tasks3D){
		task->m_info.m_age = task->m_info.m_lifetime + 1.f;
	}
}

void DebugRenderSystem::MakeDebugRender2DQuad(float lifetime, AABB2 const & bounds, RGBA const & start_color, RGBA const & end_color)
{
	DebugRenderTask_AABB2* box = new DebugRenderTask_AABB2(bounds, lifetime, start_color, end_color);
	m_tasks2D.push_back((DebugRenderTask*) box);
}

void DebugRenderSystem::MakeDebugRender2DLine(float lifetime, Vector2 const & p0, RGBA const & p0_color, Vector2 const & p1, RGBA const & p1_color, RGBA const & start_color, RGBA const & end_color)
{
	DebugRenderTask_LINE2* line = new DebugRenderTask_LINE2(p0, p0_color, p1, p1_color, lifetime, start_color, end_color);
	m_tasks2D.push_back((DebugRenderTask*) line);
}

void DebugRenderSystem::MakeDebugRender2DText(float lifetime, std::string text, Vector2 position, float scale, RGBA const & start_color, RGBA const & end_color)
{
	DebugRenderTask_TEXT2* task = new DebugRenderTask_TEXT2(text, position, scale, lifetime, start_color, end_color);
	m_tasks2D.push_back((DebugRenderTask*) task);
}

void DebugRenderSystem::MakeDebugRender2DText(float lifetime, float scale, Vector2 alignment, RGBA const & start_color, RGBA const & end_color, char const * format, va_list args)
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, args );	
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string text =  std::string( textLiteral );

	DebugRenderTask_TEXT2* task = new DebugRenderTask_TEXT2(text, alignment, scale, lifetime, start_color, end_color);
	m_tasks2D.push_back((DebugRenderTask*) task);
}

void DebugRenderSystem::MakeDebugRender2DText(float lifetime, float scale, Vector2 alignment, RGBA const & start_color, RGBA const & end_color, char const * format, ...)
{
	va_list args;
	va_start( args, format );
	MakeDebugRender2DText(lifetime, scale, alignment, start_color, end_color, format, args);
	va_end( args );
}

void DebugRenderSystem::MakeDebugRender2DText(float lifetime, float scale, Vector2 alignment, RGBA const & color, char const * format, ...)
{
	va_list args;
	va_start( args, format );
	MakeDebugRender2DText(lifetime, scale, alignment, color, color, format, args);
	va_end( args );
}

void DebugRenderSystem::MakeDebugRender2DText(float lifetime, float scale, Vector2 alignment, char const * format, ...)
{
	va_list args;
	va_start( args, format );
	MakeDebugRender2DText(lifetime, scale, alignment, RGBA::WHITE, RGBA::WHITE, format, args);
	va_end( args );
}

void DebugRenderSystem::MakeDebugRender2DText(char const * format, ...)
{
	va_list args;
	va_start( args, format );
	MakeDebugRender2DText(0.f, .02f, Vector2(.99f,.8f), RGBA::WHITE, RGBA::WHITE, format, args);
	va_end( args );
}

void DebugRenderSystem::MakeDebugRenderPoint(float lifetime, Vector3 position, RGBA const & start_color, RGBA const & end_color, DebugRenderMode const mode)
{
	DebugRenderTask_Point* point = new DebugRenderTask_Point(position, lifetime, start_color, end_color, mode);
	m_tasks3D.push_back((DebugRenderTask*) point);
}

void DebugRenderSystem::MakeDebugRenderLineSegment(Vector3 const & p0, Vector3 const & p1, RGBA const & p0_color, RGBA const & p1_color, float lifetime, RGBA const & start_color, RGBA const & end_color, DebugRenderMode const mode)
{
	DebugRenderTask_Line3* line = new DebugRenderTask_Line3(p0, p1, p0_color, p1_color, lifetime, start_color, end_color, mode);
	m_tasks3D.push_back((DebugRenderTask*) line);
}

void DebugRenderSystem::MakeDebugRenderBasis(float lifetime, Vector3 position, Matrix44 const & basis, RGBA const & start_color, RGBA const & end_color, DebugRenderMode const mode)
{
	UNUSED(basis);
	TODO("Make basis into any basis draw instead of just world basis, and also change point to be not that");
	DebugRenderTask_Basis* point = new DebugRenderTask_Basis(position, lifetime, start_color, end_color, mode);
	m_tasks3D.push_back((DebugRenderTask*) point);
}

void DebugRenderSystem::MakeDebugRenderSphere(float lifetime, Vector3 pos, float const radius, int const wedges, int const slices, RGBA const & start_color, RGBA const & end_color, DebugRenderMode const mode)
{
	DebugRenderTask_Sphere* sphere = new DebugRenderTask_Sphere(pos, radius, wedges, slices,  lifetime, start_color, end_color, mode);
	m_tasks3D.push_back((DebugRenderTask*) sphere);
}


void DebugRenderSystem::MakeDebugRenderWireAABB3(float lifetime, Vector3 pos, float const size, RGBA const & start_color, RGBA const & end_color, DebugRenderMode const mode)
{
	DebugRenderTask_AABB3* cube = new DebugRenderTask_AABB3(pos, size, lifetime, start_color, end_color, mode);
	m_tasks3D.push_back((DebugRenderTask*) cube);
}

void DebugRenderSystem::MakeDebugRenderQuad(float lifetime, Vector3 center, Vector2 size,Vector3 right, Vector3 up, RGBA startColor, RGBA endColor, DebugRenderMode mode)
{

	DebugRenderTask_Quad* quad = new DebugRenderTask_Quad(center, size, right, up,lifetime, startColor, endColor,mode);
	m_tasks3D.push_back((DebugRenderTask*) quad);
}

void DebugRenderSystem::MakeDebugRender3DText(std::string text, float lifetime, Vector3 centerPos, float scale, Vector3 up, Vector3 right, RGBA const & start_color, RGBA const & end_color, DebugRenderMode mode)
{

	DebugRenderTask_Text3* task = new DebugRenderTask_Text3(text, centerPos, scale, lifetime, start_color, end_color, up, right, mode) ;
	m_tasks3D.push_back((DebugRenderTask*) task);
}

void DebugRenderSystem::MakeDebugRenderTag(std::string text, float lifetime, Vector3 centerPos, float scale, RGBA const & start_color, RGBA const & end_color, DebugRenderMode mode)
{
	DebugRenderTask_Tag* task = new DebugRenderTask_Tag(text, centerPos, scale, lifetime, start_color, end_color, mode) ;
	m_tasks3D.push_back((DebugRenderTask*) task);
}

void DebugRenderSystem::Update()
{
	float ds = m_debugRenderClock->GetDeltaSeconds();
	for(DebugRenderTask* task : m_tasks3D){
		task->Age(ds);
	}
	for(DebugRenderTask* task : m_tasks2D){
		task->Age(ds);
	}

	if (m_currentRuler != nullptr){
		Vector3 pos = m_camera->GetPosition() + m_camera->GetForward();
		m_currentRuler->SetEndpoint(pos);
	}
}

void DebugRenderSystem::Render()
{
	
	if (m_isRendering){
		Render3D();
		Render2D();
		
	} else {
		g_theRenderer->ReleaseShader();
		ResetDepth();
	}
	
	
	
}

void DebugRenderSystem::Render3D()
{
	
	TODO("Make a debug render rendering path");
		if (m_isDetached){
			//draw the camera
			
			g_theRenderer->UseShader("wireframe");
			g_theRenderer->BindModel(m_gameCamera->m_transform.GetWorldMatrix());
			g_theRenderer->DrawMesh(m_gameCamera->GetDebugMesh());
				
			g_theRenderer->ReleaseShader();

			g_theRenderer->BindModel(Matrix44::IDENTITY);
			RenderScene2D* scene = g_theGame->GetScene();
			if (scene != nullptr){
			//	for(int i = 0; i < (int) scene->m_lights.size(); i++){
			//		//if (i >= (int) g_theGame->m_scene->m_lights.size()){
			//		//	break;
			//		//}
			//		/*if (scene->m_lights[i] != g_theGame->m_cameraLight){
			//			scene->m_lights[i]->RenderAsPoint(g_theRenderer);
			//			MakeDebugRenderTag("L", 0.f, scene->m_lights[i]->GetPosition(), .5f, RGBA::WHITE);
			//		}*/
			//	}
			}
		}
		for(DebugRenderTask* task : m_tasks3D){
			SetRendererDrawMode(task);
			task->Render();

		}

		if (m_currentRuler != nullptr){
			m_currentRuler->Render();
		}
	

	ResetDepth();
}

void DebugRenderSystem::Render2D()
{
	AABB2 bounds = g_theGame->SetUICamera();
	for(DebugRenderTask* task : m_tasks2D){
		task->Render();
	}

	RenderInfo();

	
}

void DebugRenderSystem::RenderInfo()
{
	AABB2 screenBox = g_theGame->GetUIBounds();
	//renders instructions
	if (m_showInfo){
		g_theRenderer->DrawTextInBox2D("wasd to move, arrow keys to rotate\n	\
			e/q - move camera straight up/down\n\
			x - add current 3D render task\n\
			c - add current 2D render task\n\
			L - create point light in front of camera\n\
			F1 - toggle help text"
			, screenBox,Vector2(1.f, 0.f), .02f, TEXT_DRAW_SHRINK_TO_FIT );
	}

	//render screen ruler
	if (m_showScreenSpace){
		float height = screenBox.GetHeight();
		float width = screenBox.GetWidth();
		float gridSize = .1f;
		float textSize = .01f;
		float x = 0.f;
		float y = 0.f;
		int tick = 0;
		while (x <= width){		//draw a grid on the borders
			if (tick %2 == 1){
				//draw the number
				std::string floatText = std::to_string(x);
				Strip(floatText, '0');
				g_theRenderer->DrawText2D("|", Vector2(x, height - (textSize * 1.f)), textSize);
				g_theRenderer->DrawText2D(floatText, Vector2(x - textSize, height - (textSize * 2.1f)), textSize);
			} else {
				//draw a line
				g_theRenderer->DrawText2D("|", Vector2(x, height - (textSize * 1.1f)), textSize);
			}
			x+=gridSize;
			tick++;
		}

		tick = 0;
		while (y <= height){		//draw a grid on the borders
			if (tick %2 == 1){
				//draw the number
				std::string floatText = std::to_string(y);
				Strip(floatText, '0');
				g_theRenderer->DrawText2D("-" + floatText,  Vector2(0.f, y), textSize);
			} else {
				//draw a line
				g_theRenderer->DrawText2D("-", Vector2(0.f, y), textSize);
			}
			y+=gridSize;
			tick++;
		}
		g_theRenderer->DrawTextInBox2D("+", screenBox, Vector2::HALF, .02f, TEXT_DRAW_OVERRUN, RGBA(255,255,255,100));
	}
}

void DebugRenderSystem::DestroyTasks()
{
	for (int i = m_tasks3D.size()-1; i >= 0; i--){
		if (m_tasks3D[i]->IsDead()){
			m_tasks3D[i] = m_tasks3D[m_tasks3D.size() - 1];
			m_tasks3D.pop_back();
			
		}
	}

	for (int i = m_tasks2D.size()-1; i >= 0; i--){
		if (m_tasks2D[i]->IsDead()){
			m_tasks2D[i] = m_tasks2D[m_tasks2D.size() - 1];
			m_tasks2D.pop_back();
		}
	}
}

void DebugRenderSystem::SetRendererDrawMode(DebugRenderTask* task)
{
	switch (task->m_info.m_mode){
	case DEBUG_RENDER_USE_DEPTH:
		g_theRenderer->EnableDepth(COMPARE_LESS, true);
		break;
	case DEBUG_RENDER_HIDDEN:
		g_theRenderer->EnableDepth(COMPARE_GREATER, false);
		break;
	case DEBUG_RENDER_IGNORE_DEPTH:
		//g_theRenderer->EnableDepth(COMPARE_ALWAYS, false);
		g_theRenderer->DisableDepth();
		break;
	case DEBUG_RENDER_XRAY:
		g_theRenderer->EnableDepth(COMPARE_GREATER, false);
		g_theRenderer->BindShaderProgram("inverse");
		task->Render();
		g_theRenderer->ReleaseShaderProgram();
		g_theRenderer->EnableDepth(COMPARE_LESS, true);
		break;
	}
}

void DebugRenderSystem::ResetDepth()
{
	g_theRenderer->EnableDepth(COMPARE_LESS, true);
}

void DebugRenderSystem::StartRuler()
{
	Vector3 pos = m_camera->GetPosition() + m_camera->GetForward();
	m_currentRuler = new DebugRenderTask_Ruler(pos, pos, RGBA::WHITE, RGBA::WHITE, -1.f, m_defaultStartColor, m_defaultEndColor);
}

void DebugRenderSystem::EndRuler()
{
	Vector3 pos = m_camera->GetPosition() + m_camera->GetForward();
	m_currentRuler->SetEndpoint(pos);
	m_currentRuler->SetLifetimeAndReset(m_defaultLifetime);
	m_tasks3D.push_back(m_currentRuler);
	m_currentRuler = nullptr;
}

DebugRenderInfo::DebugRenderInfo()
{
	m_age = 0.f;
	m_lifetime = 0.f;
	m_startColor = RGBA::WHITE;
	m_endColor = RGBA::WHITE;
	m_mode = DEBUG_RENDER_USE_DEPTH;
}

DebugRenderInfo::DebugRenderInfo(float lifetime, RGBA startColor, RGBA endColor, DebugRenderMode mode)
{
	m_age = 0.f;
	m_lifetime = lifetime;
	m_startColor = startColor;
	m_endColor = endColor;
	m_mode = mode;
}

DebugRenderMode StringToDebugRenderMode(std::string mode)
{
	mode = ToLower(mode);
	if (mode == "ignore_depth"){
		return DEBUG_RENDER_IGNORE_DEPTH;	
	}
	if (mode == "use_depth"){
		return DEBUG_RENDER_USE_DEPTH	;	
	}
	if (mode == "hidden"){
		return DEBUG_RENDER_HIDDEN		;	
	}
	if (mode == "xray"){
		return DEBUG_RENDER_XRAY		;
	}

	ConsolePrintf(RGBA::RED, "%s is not a valid mode. Debug Render system set to use_depth.\nModes:\n  use_depth, ignore_depth, hidden, xray",  mode.c_str());

	return DEBUG_RENDER_USE_DEPTH;		//default
}

RenderTaskType StringToDebugRenderTask(std::string task)
{
	task = ToLower(task);
	if (task == "point"){
		return RENDER_TASK_POINT	;
	}
	if (task == "basis"){
		return RENDER_TASK_BASIS	;
	}
	if (task == "quad"){
		return RENDER_TASK_QUAD	;
	}
	if (task == "sphere"){
		return RENDER_TASK_SPHERE	;
	}
	if (task == "aabb3"){
		return RENDER_TASK_AABB3;
	}
	if (task == "text3"){
		return RENDER_TASK_TEXT3;
	}
	if (task == "line3"){
		return RENDER_TASK_LINE3;
	}
	if (task == "ruler"){
		return RENDER_TASK_RULER;
	}
	if (task == "tag"){
		return RENDER_TASK_TAG;
	}


	if (task == "line2"){
		return RENDER_TASK_LINE2;
	}
	if (task == "aabb2"){
		return RENDER_TASK_AABB2;
	}
	if (task == "text2"){
		return RENDER_TASK_TEXT2;
	}

	//ConsolePrintf(RGBA::RED, "No task called: %s. Task set to DebugPoint.",  task.c_str());

	return NUM_RENDER_TASKS;	//default
}

const char* GetTaskName(RenderTaskType type)
{
	switch (type){
		case RENDER_TASK_AABB2	:		//2D tasks
			return "Quad2D";
		case RENDER_TASK_LINE2	:
			return "Line2D";
		case RENDER_TASK_TEXT2	:
			return "Text2D";

		case RENDER_TASK_POINT	:
			return "Point";
		case RENDER_TASK_BASIS	:
			return "Basis";
		case RENDER_TASK_QUAD	:
			return "Quad3D";
		case RENDER_TASK_SPHERE	:
			return "Sphere";
		case RENDER_TASK_AABB3	:
			return "Cube";
		case RENDER_TASK_LINE3	:
			return "Line3D";
		case RENDER_TASK_RULER	:		//line3D but with endpoints/midpoint render, and distance rendered as text
			return "Ruler";
		case RENDER_TASK_TEXT3	:
			return "Text3D";
		case RENDER_TASK_TAG	:
			return "Tag";

	}
	return "NO_NAME";
}

const char* GetTaskID(RenderTaskType type)
{
	switch (type){
	case RENDER_TASK_AABB2	:		//2D tasks
		return "aabb2";
	case RENDER_TASK_LINE2	:
		return "line2";
	case RENDER_TASK_TEXT2	:
		return "text2";

	case RENDER_TASK_POINT	:
		return "point";
	case RENDER_TASK_BASIS	:
		return "basis";
	case RENDER_TASK_QUAD	:
		return "quad";
	case RENDER_TASK_SPHERE	:
		return "sphere";
	case RENDER_TASK_AABB3	:
		return "aabb3";
	case RENDER_TASK_LINE3	:
		return "line3";
	case RENDER_TASK_RULER	:		//line3D but with endpoints/midpoint render, and distance rendered as text
		return "ruler";
	case RENDER_TASK_TEXT3	:
		return "Text3D";
	case RENDER_TASK_TAG	:
		return "tag";

	}
	return "NO_ID";
}

const char* GetTaskDescription(RenderTaskType type)
{
	switch (type){
	case RENDER_TASK_AABB2	:		//2D tasks
		return "2D quad rendered in real screen space (0,0 -> aspect,1)";
	case RENDER_TASK_LINE2	:
		return "2D line rendered in real screen space (0,0 -> aspect,1)";
	case RENDER_TASK_TEXT2	:
		return "2D text rendered in normalized screen space (0,0 -> 1,1)";

	case RENDER_TASK_POINT	:
		return "3D point rendered as 3 intersecting lines";
	case RENDER_TASK_BASIS	:
		return "3D world basis with tinted axes (r = x, g = y, b = z)";
	case RENDER_TASK_QUAD	:
		return "3D quad defined with center point, right, and up";
	case RENDER_TASK_SPHERE	:
		return "3D Wireframe sphere defined with center point, radius, numWedges, and numSlices";
	case RENDER_TASK_AABB3	:
		return "3D Wireframe cube defined with center point and size";
	case RENDER_TASK_LINE3	:
		return "3D line defined with 2 endpoints. Drawing line3 in detached camera mode draws as ruler.";
	case RENDER_TASK_RULER	:		//line3D but with endpoints/midpoint render, and distance rendered as text
		return "3D line which displays midpoint and line length. Drawing ruler in detached camera mode sets p0 on first press, p1 on second press.";
	case RENDER_TASK_TEXT3	:
		return "3D text rendered in world space on a flat plane.";
	case RENDER_TASK_TAG	:
		return "3D text billboarded to the camera";

	}
	return "NO_DESC";
}
