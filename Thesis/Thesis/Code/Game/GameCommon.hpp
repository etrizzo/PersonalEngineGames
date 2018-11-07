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

#include "Engine/DataTypes/DirectedGraph.hpp"


#define SMALL_VALUE .05f
#define MAX_STEPS 16

#define NUM_PLOT_NODES_TO_GENERATE (6 + 2)
#define NUM_DETAIL_NODES_TO_GENERATE (3)
#define DEFAULT_BRANCH_CHANCE_ON_FAIL (0.0f)


const int TILE_WIDTH = 1;

class Renderer;
class InputSystem;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
//extern Window* g_theWindow;

extern SpriteSheet* g_tileSpriteSheet;


enum eCharacterTrait{
	TRAIT_SOCIAL,			// Introversion --- Extroversion
	TRAIT_INFORMATION,		//		Sensing --- Intuition
	TRAIT_DECISIONS,		//		feeling --- thinking
	TRAIT_STRUCTURE,		//		judging --- perceiving
	NUM_CHARACTER_TRAITS
};


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



eCharacterTrait GetTraitFromString(std::string traitString);
std::string GetStringFromTrait(eCharacterTrait trait);




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