#pragma once
#include "Engine/Core/RGBA.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Renderer.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/SpriteAnimSetDef.hpp"
#include "Engine/Renderer/SpriteRenderPath.hpp"

#include "Engine/DataTypes/DirectedGraph.hpp"

#define SCREEN_MAX 1000.f
#define SCREEN_MIN 0.f

const Vector3 RIGHT		= Vector3(1.f, 0.f, 0.f);
const Vector3 UP		= Vector3(0.f, 1.f, 0.f);
const Vector3 FORWARD	= Vector3(0.f, 0.f, 1.f);

const int TILE_WIDTH = 1;
//const float WINDOW_ASPECT = 1.f;
const float ZOOM_FACTOR = 10.f;

const float DEFAULT_FORWARD_VIEW_ANGLE = 360.f;

typedef std::vector<std::string> Strings;

enum GAME_STATE{
	NO_STATE = -1,
	STATE_ATTRACT,
	STATE_PLAYING,
	STATE_PAUSED,
	STATE_INVENTORY,
	STATE_DEFEAT,
	STATE_VICTORY,
	STATE_MAPMODE,
	NUM_STATES
};

enum EQUIPMENT_SLOT{
	NOT_EQUIPPABLE = -1,
	EQUIP_SLOT_LEGS,
	EQUIP_SLOT_CHEST,
	EQUIP_SLOT_HEAD,
	EQUIP_SLOT_WEAPON,
	NUM_EQUIP_SLOTS
};

//slots for rendering portraits
enum ePortraitSlot{
	HAIR_BACK_PORTRAIT,
	FACE_PORTRAIT,
	NOSE_PORTRAIT,
	MOUTH_PORTRAIT,			//1-2
	BROWS_PORTRAIT,
	FEATURES_PORTRAIT,		//1-3
	EYES_PORTRAIT,			//2
	HAIR_FRONT_PORTRAIT,
	EAR_PORTRAIT,
	NUM_PORTRAIT_SLOTS
};

enum RENDER_SLOT{
	BODY_SLOT,		//for rendering actors
	LEGS_SLOT,
	CHEST_SLOT,
	HEAD_SLOT,
	EARS_SLOT,
	HAT_SLOT,
	WEAPON_SLOT,
	NUM_RENDER_SLOTS
};


enum STAT_ID{
	STAT_STRENGTH,
	STAT_MOVEMENT,
	STAT_DEFENSE,
	NUM_STAT_IDS
};

enum eAIBehavior{
	BEHAVIOR_NONE,
	BEHAVIOR_WANDER,
	BEHAVIOR_ATTACK,
	BEHAVIOR_FOLLOW,
	NUM_BEHAVIORS
};


eAIBehavior GetBehaviorFromString(std::string behavior);

RGBA GetRandomLipColor();
RGBA GetRandomEyeColor();
RGBA GetRandomHairColor();


class Renderer;
class InputSystem;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern XboxController* g_primaryController;

extern SpriteSheet* g_tileSpriteSheet;
extern SpriteSheet* g_portraitSpriteSheet;


/*==========================
Thesis Stuff
============================*/

#define NUM_NODE_PAIRS_TO_GENERATE (7)
#define NUM_PLOT_NODES_TO_GENERATE (6 + 2)
#define NUM_DETAIL_NODES_TO_GENERATE (3)
#define DEFAULT_BRANCH_CHANCE_ON_FAIL (0.0f)
#define MAX_ACTS (20)


class Character;
class StoryData;
class StoryState;

typedef Node<StoryData*, StoryState*> StoryNode ;
typedef DirectedEdge<StoryData*, StoryState*> StoryEdge;

enum eNodeType{
	PLOT_NODE,
	DETAIL_NODE,
	DEFAULT_NODE,		//for start and end nodes
	NUM_NODE_TYPES
};


enum eProgressionType {
	PROGRESSION_TIME,
	PROGRESSION_INTERACT,
	PROGRESSION_CHOICE,
	NUM_PROGRESSION_TYPES
};