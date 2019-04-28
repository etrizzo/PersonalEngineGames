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


#define SMALL_VALUE .05f
#define MAX_STEPS 16

constexpr unsigned int MAX_ENEMIES_SPAWNED = 50;
constexpr float MIN_ENEMY_SPAWN_TIMER = 4.5f;
constexpr float MAX_ENEMY_SPAWN_TIMER = 6.5f;
constexpr float ENEMY_RATE_OF_ATTACK = 2.f;
constexpr float ASTEROID_SPAWN_RATE = 2.f;


constexpr float PLAYER_MAX_HEALTH = 6.f;
constexpr float RELOAD_HEALTH_COST = 2.f;
constexpr float HIT_HEALTH_COST = .5f;

constexpr float PLAYER_REGEN_RATE = 1.f;
constexpr float TURRET_RATE_OF_FIRE = 4.f;

enum eEnemyBehavior
{
	BEHAVIOR_FOLLOW_TARGET,
	BEHAVIOR_ATTACK,
	NUM_BEHAVIORS
};

class Renderer;
class InputSystem;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
//extern Window* g_theWindow;

extern SpriteSheet* g_tileSpriteSheet;


enum eDebugShaders{
	SHADER_LIT,			//default
	SHADER_NORMAL,
	SHADER_TANGENT,
	SHADER_BITANGENT,
	SHADER_NORMALMAP,
	SHADER_WORLDNORMAL,
	SHADER_DIFFUSE,
	SHADER_SPECULAR,
	NUM_DEBUG_SHADERS
};

//vector definitions
const static Vector3 RIGHT		= Vector3(1.f, 0.f, 0.f);
const static Vector3 UP			= Vector3(0.f, 1.f, 0.f);
const static Vector3 FORWARD	= Vector3(0.f, 0.f, 1.f);


//determines if arrow keys or d pad buttons were pressed
bool RightJustPressed();
bool LeftJustPressed();
bool UpJustPressed();
bool DownJustPressed();

bool IsRightDown();
bool IsLeftDown();
bool IsUpDown();
bool IsDownDown();		//down down alpha

bool AcceptJustPressed();	//true if enter or ControllerA is pressed
bool BackJustPressed();		//true if backspace or ControllerB is pressed