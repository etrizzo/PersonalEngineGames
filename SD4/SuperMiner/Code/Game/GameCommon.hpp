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
#include "Engine/Renderer/Material.hpp"


#define SMALL_VALUE .05f
#define MAX_STEPS 16

const int TILE_WIDTH = 1;

class Renderer;
class InputSystem;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
//extern Window* g_theWindow;

extern SpriteSheet* g_blockSpriteSheet;


//vector definitions
const static Vector3 RIGHT		= Vector3(0.f,-1.f, 0.f);
const static Vector3 UP			= Vector3(0.f, 0.f, 1.f);
const static Vector3 FORWARD	= Vector3(1.f, 0.f, 0.f);

const static RGBA s_blockNorthSouthColor	= RGBA(200,200,200,255);
const static RGBA s_blockEastWestColor		= RGBA(225,225,225,255);
const static RGBA s_blockTopBottomColor		= RGBA::WHITE;

constexpr int   CHUNK_BITS_X = 3;
constexpr int   CHUNK_BITS_Y = 3;
constexpr int   CHUNK_BITS_Z = 4;

constexpr int   CHUNK_SIZE_X = (1 << CHUNK_BITS_X);
constexpr int   CHUNK_SIZE_Y = (1 << CHUNK_BITS_Y);
constexpr int   CHUNK_SIZE_Z = (1 << CHUNK_BITS_Z);
constexpr int   CHUNK_LAYER_DIMS_XY = CHUNK_SIZE_X * CHUNK_SIZE_Y;
constexpr int	BLOCKS_PER_CHUNK = CHUNK_SIZE_X  * CHUNK_SIZE_Y * CHUNK_SIZE_Z;


//this is the matrix to apply to convert world coordinates to engine coordinates 
const static Matrix44 g_worldToEngine = Matrix44(Vector3(0.f, 0.f, 1.f), 		//right
							Vector3(-1.f, 0.f, 0.f), 		//up
							Vector3(0.f, 1.f, 0.f));		//forward


enum eBlockType : unsigned char{
	BLOCK_AIR = 0,			//air is always 0!! we make assumptions on this.
	BLOCK_GRASS,
	BLOCK_STONE,
	BLOCK_SNOW,
	NUM_BLOCKS
};

//const static Matrix44 WORLD_BASIS = Matrix44(RIGHT, UP, FORWARD);


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
