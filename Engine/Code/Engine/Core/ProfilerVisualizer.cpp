#include "Engine/Core/ProfilerVisualizer.hpp"
#include "Engine/Core/ProfilerReport.hpp"
#include "Engine/Math/Renderer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"

ProfilerVisualizer::ProfilerVisualizer(Renderer * renderer, AABB2 uiBounds)
{
	m_currentReport = new ProfilerReport();
	m_renderer = renderer;
	m_bounds = uiBounds;

	m_reportArea = m_bounds.GetPercentageBox(.05f,.1f, .95f, .6f);
	m_graphArea = m_bounds.GetPercentageBox(.42f,.7f,.9f,.9f);
	m_infoArea = m_bounds.GetPercentageBox(.1f,.8f,.4f,.9f);
	m_hotkeyArea = m_bounds.GetPercentageBox(.1f, .7f, .4f, .8f);
}

void ProfilerVisualizer::Update()
{
	profileMeasurement_t* frame = Profiler::GetInstance()->ProfileGetPreviousFrame();

	if (frame != nullptr){
		ProfilerReport* report = new ProfilerReport();
		if (m_isTree){
			m_currentReport->GenerateReportTreeFromFrame(frame);
		} else {
			m_currentReport->GenerateReportFlatFromFrame(frame);
		}

	} else {
		m_renderer->DrawTextInBox2D("No profiler frame found- profiling may be disabled in EngineBuildPreferences.hpp", m_reportArea, Vector2::HALF, .05f, TEXT_DRAW_WORD_WRAP, RGBA::RED);
	}
}

void ProfilerVisualizer::HandleInput(InputSystem* input)
{
	if (input->WasKeyJustPressed('V')){
		m_isTree = !m_isTree;
	}
	if (input->WasKeyJustPressed('M')){
		m_controllingInput = !m_controllingInput;
		input->ToggleCursor();
		input->ToggleMouseLock();
		/*input->ShowCursor(m_controllingInput);
		if (m_controllingInput){
			input->UnlockMouse();
		} else {
			input->LockMouse();
		}*/
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
	for (std::pair<std::string, ProfilerReportEntry* >child : root->m_children){
		text= text + GetNodeString(child.second, depth + 1);
	}
	return text;
}

void ProfilerVisualizer::RenderReport()
{
	std::string reportText = GetNodeString(m_currentReport->m_root);

	m_renderer->DrawTextInBox2D(reportText, m_reportArea, Vector2(0.f,.92f), .05f, TEXT_DRAW_SHRINK_TO_FIT, m_outlineColor);
}

void ProfilerVisualizer::RenderGraph()
{
	Profiler* instance = Profiler::GetInstance();
	float maxFrameTime = 100.f;
	float sliceWidth = m_graphArea.GetWidth() / (float) PROFILER_MAX_FRAME_COUNT;
	MeshBuilder mb;
	mb.Begin(PRIMITIVE_TRIANGLES, true);

	Vector3 botLeft;
	Vector3 botRight;
	Vector3 topLeft;
	Vector3 topRight;
	float z = 0.f;
	
	//0 is the oldest frame, PROFILER_MAX_FRAME_COUNT is the newest frame
	for (int i = 0; i < PROFILER_MAX_FRAME_COUNT; i++){
		//for each frame
		profileMeasurement_t* frame = instance->ProfileGetPreviousFrame(PROFILER_MAX_FRAME_COUNT - i);
		float frameTime = frame->GetMilliseconds();
		float percentageX = (float) i / (float) PROFILER_MAX_FRAME_COUNT;
		float percentageY = frameTime / maxFrameTime;
		//add a quad to a mesh-builder
		Vector2 height = m_graphArea.GetPointAtNormalizedCoord(percentageX, percentageY);

		botLeft = Vector3(height.x, m_graphArea.mins.y, z);
		botRight = Vector3(height.x + sliceWidth, m_graphArea.mins.y, z);
		topLeft = Vector3(height.x, height.y, z);
		topRight = Vector3(height.x + sliceWidth, height.y, z);

		mb.AppendQuad(botLeft,botRight,topLeft,topRight, RGBA::YELLOW);
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
	std::string theThing = fpsText + "\n" + frameTime;
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






std::string FormatProfilerReport(ProfilerReportEntry * entry, int parentCount)
{
	//std::string s = 
	//	Stringf("%*s %-*s %*u %*f ms",
	//		parentCount,"", 32, entry->m_id.data(), 5, entry->m_callCount, 5, (float) entry->m_totalTime);
	////std::string s = Stringf("%s", entry->m_id.data());

	std::string fancy = Stringf( "%*s%-*s %-8u %-8s %-14fms %-8s %-14fms", 
		parentCount, "", 
		48 - parentCount, entry->m_id.data(), 
		entry->m_callCount,
		entry->GetTotalPercentTime().c_str(),
		(float) entry->GetTotalElapsedTime(), 
		entry->GetSelfPercentTime().c_str(), 
		(float) entry->GetSelfElapsedTime());
	return fancy;
}