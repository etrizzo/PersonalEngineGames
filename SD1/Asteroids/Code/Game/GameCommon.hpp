#pragma once
#include "Engine/Core/EngineCommon.hpp"

#define SCREEN_MAX 1000.f
#define SCREEN_MIN 0.f

class Renderer;
class InputSystem;
class AudioSystem;

extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;