#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/RGBA.hpp"
#include <string>

class Renderer;
class InputSystem;
class ProfilerReport;
class ProfilerReportEntry;

//pulls from profiler instance auto-magically
class ProfilerVisualizer
{
public:
	ProfilerVisualizer(Renderer* renderer, InputSystem* input, AABB2 uiBounds);

	void Update();
	void HandleInput(InputSystem* input);
	void Render();
	void ToggleOpen();
	inline bool IsOpen() const { return m_isOpen; };
	inline bool IsControllingInput() const { return m_controllingInput; };

	std::string GetNodeString(ProfilerReportEntry* root, int depth=0);		//recursively gets string for a report entry

	static ProfilerVisualizer* GetInstance(Renderer* renderer = nullptr, InputSystem* input = nullptr, AABB2 uiBounds = AABB2::ZERO_TO_ONE);
	static ProfilerVisualizer* s_profilerVisualizerInstance;

public:
	bool m_isTree = true;
	bool m_isOpen = false;
	bool m_controllingInput = false;
	bool m_selfSort = false;

	ProfilerReport* m_currentReport;

	Renderer* m_renderer;
	AABB2 m_bounds;

	AABB2 m_reportArea;		//for the text tree/flat view of the report
	AABB2 m_graphArea;		//for the graph
	AABB2 m_infoArea;		//for any other profiler info (FPS, etc.)
	AABB2 m_hotkeyArea;

	RGBA m_bgColor = RGBA(128,0,200,100);
	RGBA m_outlineColor = RGBA::WHITE;

	Vector2 m_mousePos;
	RGBA m_graphBestColor = RGBA::GREEN;
	RGBA m_graphWorstColor = RGBA::RED;
	float m_bestMS = 10.f;
	float m_worstMS = 50.f;
	//RGBA m_graphColor = RGBA::YELLOW;
	InputSystem* m_input;

	unsigned int m_currentFrameHoverIndex = 0;
	int m_currentFrameSelectionIndex = -1;

protected:
	void RenderReport();
	void RenderGraph();
	void RenderInfo();
	void RenderHotkeys();
	unsigned int GetFrameIndexForMousePos();
};





//============
// HELPERS!!!
//============

//formats a profilerreportentry to be pretty
std::string FormatProfilerReport(ProfilerReportEntry* entry, int parentCount);

