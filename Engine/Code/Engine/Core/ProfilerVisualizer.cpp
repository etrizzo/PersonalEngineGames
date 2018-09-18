#include "Engine/Core/ProfilerVisualizer.hpp"
#include "Engine/Core/ProfilerReport.hpp"
#include "Engine/Math/Renderer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"

ProfilerVisualizer::ProfilerVisualizer(Renderer * renderer, InputSystem* input, AABB2 uiBounds)
{
	m_currentReport = new ProfilerReport();
	m_renderer = renderer;
	m_input = input;
	m_bounds = uiBounds;
	
	float minx = .01f;
	float maxx = .99f;

	m_reportArea = m_bounds.GetPercentageBox(minx,.1f, maxx, .6f);
	m_graphArea = m_bounds.GetPercentageBox(.47f,.7f,maxx,.9f);
	m_infoArea = m_bounds.GetPercentageBox(minx,.8f,.35f,.9f);
	m_hotkeyArea = m_bounds.GetPercentageBox(minx, .7f, .35f, .8f);
}

void ProfilerVisualizer::Update()
{
	profileMeasurement_t* frame;
	if (m_currentFrameSelectionIndex >=0){
		frame = Profiler::GetInstance()->ProfileGetPreviousFrame(m_currentFrameSelectionIndex);
	} else {
		frame = Profiler::GetInstance()->ProfileGetPreviousFrame(0);
	}

	if (frame != nullptr){
		//ProfilerReport* report = new ProfilerReport();
		if (m_isTree){
			m_currentReport->GenerateReportTreeFromFrame(frame);
		} else {
			m_currentReport->GenerateReportFlatFromFrame(frame);
		}

	} 
}

void ProfilerVisualizer::HandleInput(InputSystem* input)
{
	m_mousePos = input->GetMouseNormalizedScreenPosition(m_bounds);
	if (input->WasKeyJustPressed('V')){
		m_isTree = !m_isTree;
	}

	if (input->WasKeyJustPressed('L')){
		m_selfSort = !m_selfSort;
	}

	if (input->WasKeyJustPressed('M')){
		m_controllingInput = !m_controllingInput;
		input->ToggleCursor();
		input->ToggleMouseLock();
		if (m_controllingInput){
			SetCursor(g_Window->m_cursor);
		} else {
			SetCursor(NULL);
		}
	}

	if (input->WasKeyJustPressed('P')){
		Profiler::GetInstance()->TogglePause();
		//if (!Profiler::GetInstance()->IsPaused()){
		m_currentFrameSelectionIndex = -1;
		//}
	}

	
	if (m_graphArea.IsPointInside(m_mousePos)){
		m_currentFrameHoverIndex = GetFrameIndexForMousePos();
		if (input->WasMouseButtonJustPressed(MOUSE_BUTTON_LEFT)){
			//m_graphColor = RGBA::GetRandomRainbowColor();
			m_currentFrameSelectionIndex = (int) m_currentFrameHoverIndex;
			Profiler::GetInstance()->Pause();
		}
	} else {
		m_currentFrameHoverIndex = 0;
	}
}

void ProfilerVisualizer::Render()
{
	m_renderer->DrawAABB2(m_bounds, m_bgColor);
	m_renderer->DrawAABB2Outline(m_bounds, m_outlineColor);
	m_renderer->DrawAABB2Outline(m_reportArea, m_outlineColor);
	m_renderer->DrawAABB2Outline(m_graphArea, m_outlineColor);
	m_renderer->DrawAABB2Outline(m_infoArea, m_outlineColor);
	m_renderer->DrawAABB2Outline(m_hotkeyArea, m_outlineColor);

	RenderGraph();
	RenderInfo();
	RenderHotkeys();
	RenderReport();
}

void ProfilerVisualizer::ToggleOpen()
{
	m_isOpen = !m_isOpen;
	if (!m_isOpen){
		m_controllingInput = false;
	}
}

std::string ProfilerVisualizer::GetNodeString(ProfilerReportEntry * root, int depth)
{
	std::string text = FormatProfilerReport(root, depth) + "\n";
	for ( ProfilerReportEntry* child : root->m_children){
		text= text + GetNodeString(child, depth + 1);
	}
	return text;
}

void ProfilerVisualizer::RenderReport()
{
	if (m_selfSort){
		m_currentReport->SortBySelfTime();
	} else {
		m_currentReport->SortByTotalTime();
	}
	std::string header = Stringf( "%-*s %-8s %8s %12s %8s %12s\n", 
		64, "ID", 
		"Calls",
		"Total %",
		"Total ms", 
		"Self %", 
		"Self ms");
	std::string reportText = header + GetNodeString(m_currentReport->m_root);

	m_renderer->DrawTextInBox2D(reportText, m_reportArea, Vector2(0.f,.92f), .05f, TEXT_DRAW_SHRINK_TO_FIT, m_outlineColor);
}

void ProfilerVisualizer::RenderGraph()
{
	Profiler* instance = Profiler::GetInstance();
	//float maxFrameTime = 100.f;
	float sliceWidth = m_graphArea.GetWidth() / (float) PROFILER_MAX_FRAME_COUNT;
	MeshBuilder mb;
	mb.Begin(PRIMITIVE_TRIANGLES, true);

	Vector3 botLeft;
	Vector3 botRight;
	Vector3 topLeft;
	Vector3 topRight;
	float z = 0.f;

	float maxMS = 0.f;
	for (int i = 0; i < PROFILER_MAX_FRAME_COUNT; i++){
		profileMeasurement_t* frame = instance->ProfileGetPreviousFrame(PROFILER_MAX_FRAME_COUNT - i);
		if (maxMS < frame->GetMilliseconds()){
			maxMS = frame->GetMilliseconds();
		}
	}

	//add padding to the max ms so it's not always right at the top
	maxMS +=5.f;

	//draw ms marker at top and bottom of graph
	m_renderer->DrawTextInBox2D(Stringf("%3.1fms", maxMS), m_graphArea, Vector2(-.08f, 1.f), .01f, TEXT_DRAW_OVERRUN);
	m_renderer->DrawTextInBox2D(Stringf("%3.1fms", 0.f), m_graphArea, Vector2(-.08f, 0.f), .01f, TEXT_DRAW_OVERRUN);

	//0 is the oldest frame, PROFILER_MAX_FRAME_COUNT is the newest frame
	for (int i = 0; i < PROFILER_MAX_FRAME_COUNT; i++){
		//for each frame
		unsigned int frameOffset = PROFILER_MAX_FRAME_COUNT - i;
		profileMeasurement_t* frame = instance->ProfileGetPreviousFrame(PROFILER_MAX_FRAME_COUNT - i);
		float frameTime = frame->GetMilliseconds();
		float percentageX = (float) i / (float) PROFILER_MAX_FRAME_COUNT;
		float percentageY = frameTime / maxMS;
		//add a quad to a mesh-builder
		Vector2 height = m_graphArea.GetPointAtNormalizedCoord(percentageX, percentageY);

		botLeft = Vector3(height.x, m_graphArea.mins.y, z);
		botRight = Vector3(height.x + sliceWidth, m_graphArea.mins.y, z);
		topLeft = Vector3(height.x, height.y, z);
		topRight = Vector3(height.x + sliceWidth, height.y, z);

		//get the lerped frame color
		float t = RangeMapFloat(frameTime, m_bestMS, m_worstMS, 0.f, 1.f);
		t = ClampFloatZeroToOne(t);
		RGBA colorAtFrame = Interpolate(m_graphBestColor, m_graphWorstColor, t);

		//append frame quad to graph (highlighting selected and hovered frames)
		if (frameOffset == m_currentFrameHoverIndex){
			mb.AppendQuad(botLeft,botRight,topLeft,topRight, RGBA::WHITE);
		} else if ((int) frameOffset == m_currentFrameSelectionIndex) {
			mb.AppendQuad(botLeft,botRight,topLeft,topRight, RGBA::CYAN);
		} else{
			mb.AppendQuad(botLeft,botRight,topLeft,topRight, colorAtFrame.GetColorWithAlpha(200));
		}
	}
	mb.End();

	SubMesh* m = mb.CreateSubMesh(VERTEX_TYPE_3DPCU);
	m_renderer->DrawMesh(m);
	delete m;
}

void ProfilerVisualizer::RenderInfo()
{
	float ds = GetMasterClock()->GetDeltaSeconds();
	float fps = 1.f / ds;
	
	std::string fpsText = Stringf("FPS: %.2f", fps);
	std::string frameTime = Stringf("Frame time: %.2f ms", ds * 1000.f);
	std::string mousePos = Stringf("Mouse pos: (%.3f, %.3f)", m_mousePos.x, m_mousePos.y); 
	std::string theThing = fpsText + "\n" + frameTime + "\n" +  mousePos;
	m_renderer->DrawTextInBox2D(theThing, m_infoArea, Vector2(0.0f, .5f), .05f, TEXT_DRAW_SHRINK_TO_FIT);
}

void ProfilerVisualizer::RenderHotkeys()
{
	std::string hotkeys = R"(
Hotkeys:
'V' - Toggle Tree/Flat view
'L' - Toggle Flat View Sorting
'P' - Toggle Pause
'M' - Enable mouse (disables game input) )";
	m_renderer->DrawTextInBox2D(hotkeys, m_hotkeyArea, Vector2(0.0f, .5f), .05f, TEXT_DRAW_SHRINK_TO_FIT);
}

unsigned int ProfilerVisualizer::GetFrameIndexForMousePos()
{
	Vector2 pos = m_graphArea.GetPercentageOfPoint(m_mousePos);
	
	float frame = RangeMapFloat(m_mousePos.x, m_graphArea.mins.x, m_graphArea.maxs.x, (float) PROFILER_MAX_FRAME_COUNT,  0.f );
	return (unsigned int) frame;
}






std::string FormatProfilerReport(ProfilerReportEntry * entry, int parentCount)
{
	//std::string s = 
	//	Stringf("%*s %-*s %*u %*f ms",
	//		parentCount,"", 32, entry->m_id.data(), 5, entry->m_callCount, 5, (float) entry->m_totalTime);
	////std::string s = Stringf("%s", entry->m_id.data());

	std::string fancy = Stringf( "%*s%-*s %-8u %-8s %-12s %-8s %-12s", 
		parentCount, "", 
		64 - parentCount, entry->m_id.data(), 
		entry->m_callCount,
		entry->GetTotalPercentTime().c_str(),
		entry->GetTotalMillisecondsAsString().c_str(), 
		entry->GetSelfPercentTime().c_str(), 
		entry->GetSelfMillisecondsAsString().c_str());
	return fancy;
}

