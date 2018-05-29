#pragma once
#include "Engine/Core/RGBA.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Renderer.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Clock.hpp"


#define SCREEN_MAX 1000.f
#define SCREEN_MIN 0.f

const int TILE_WIDTH = 1;

class Renderer;
class InputSystem;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
//extern Window* g_theWindow;

extern SpriteSheet* g_tileSpriteSheet;
