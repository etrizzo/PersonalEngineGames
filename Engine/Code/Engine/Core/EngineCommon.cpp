#include "Engine/Core/EngineCommon.hpp"

Blackboard g_gameConfigBlackboard = Blackboard();
Window* g_Window = nullptr;
DevConsole* g_devConsole = nullptr;
ProfilerVisualizer* g_profilerVisualizer = nullptr;





eCullMode StringToCullMode(std::string text)
{
	if (text == "front"){
		return CULLMODE_FRONT;
	}
	if (text == "back"){
		return CULLMODE_BACK;
	}
	if (text == "none"){
		return CULLMODE_NONE;
	}

	return CULLMODE_BACK;		//default
}

eFillMode StringToFillMode(std::string text)
{
	if (text == "fill"){
		return FILL_MODE_FILL;
	}
	if (text == "wire"){
		return FILL_MODE_WIRE;
	}

	return FILL_MODE_FILL;		//default
}

eWindOrder StringToFrontFace(std::string text)
{
	if (text == "ccw"){
		return WIND_COUNTER_CLOCKWISE;
	}
	if (text == "cw"){
		return WIND_CLOCKWISE;
	}

	return WIND_COUNTER_CLOCKWISE;		//default
}

eBlendMode StringToBlendMode(std::string text)
{
	if (text == "alpha"){
		return BLEND_MODE_ALPHA;
	}
	if (text == "additive"){
		return BLEND_MODE_ADDITIVE;
	}
	if (text == "multiply"){
		return BLEND_MODE_MULTIPLY;
	}
	if (text == "none"){
		return BLEND_MODE_NONE;
	}

	return BLEND_MODE_ALPHA;			//default
}

eCompare StringToDepthCompare(std::string text)
{
	if (text == "never"){
		return COMPARE_NEVER    ;  // GL_NEVER
	}
	if (text == "less"){
		return COMPARE_LESS     ;   // GL_LESS
	}
	if (text == "lequal"){
		return COMPARE_LEQUAL   ;   // GL_LEQUAL
	}
	if (text == "greater"){
		return COMPARE_GREATER  ;   // GL_GREATER
	}
	if (text == "gequal"){
		return COMPARE_GEQUAL   ;  // GL_GEQUAL
	}
	if (text == "equal"){
		return COMPARE_EQUAL    ;  // GL_EQUAL
	}
	if (text == "not"){
		return COMPARE_NOT_EQUAL;   // GL_NOTEQUAL
	}
	if (text == "always"){
		return COMPARE_ALWAYS   ;   // GL_ALWAYS
	}

	return COMPARE_LESS;		//default

}
