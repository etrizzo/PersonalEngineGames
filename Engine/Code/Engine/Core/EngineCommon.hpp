#pragma once
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/ProfilerReport.hpp"
#include "Engine/Core/ProfilerVisualizer.hpp"
#include "Engine/Core/LogSystem.hpp"
#include "Engine/FileUtilities.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AreaMask.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Sphere.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/RawNoise.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Ray.hpp"


#include "Engine/Core/Blackboard.hpp"
#include "Engine/Renderer/Texture.hpp"
//#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Heatmap.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/DevConsole.hpp"

#include "Engine/Core/BytePacker.hpp"


#include <vector>

#define MAX_LIGHTS 8


#define UNUSED(x) (void)(x);

#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "


#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )

// THE IMPORANT BITS
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
        " --------------------------------------------------------------------------------------\n" \
        "|  TODO :   " ##x "\n" \
        " --------------------------------------------------------------------------------------\n" )


#define UNIMPLEMENTED()  TODO( "IMPLEMENT: " QUOTE(__FILE__) " (" QUOTE(__LINE__) ")" ); GUARANTEE_RECOVERABLE(0, "UNIMPLEMENTED");


//networking
#define INVALID_PACKET_ACK (0xffff)
#define INVALID_RELIABLE_ID (0xffff)
#define INVALID_CONNECTION_INDEX (0xff)
#define MAX_MESSAGE_CHANNELS (8)





class Window;
class DevConsole;


extern Blackboard g_gameConfigBlackboard; // declared in EngineCommon.hpp, defined in .cpp



extern Window* g_Window;		//should maybe return this to singleton in window.cpp at some point
extern DevConsole* g_devConsole;
//extern ProfilerVisualizer* g_profilerVisualizer;




enum eManhattanDir{
	MANHATTAN_NORTH,
	MANHATTAN_WEST,
	MANHATTAN_EAST,
	MANHATTAN_SOUTH,
	NUM_MANHATTAN_DIRS
};

enum ePlaybackMode
{
	PLAYBACK_MODE_PLAY_TO_END,
	PLAYBACK_MODE_LOOP,
	PLAYBACK_MODE_PINGPONG,
	NUM_PLAYBACK_MODES
};

enum eDrawPrimitiveType{
	PRIMITIVE_LINES,
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_QUADS,
	NUM_PRIMITIVE_TYPES
};


enum eTextDrawMode{
	TEXT_DRAW_SHRINK_TO_FIT,
	TEXT_DRAW_WORD_WRAP,
	TEXT_DRAW_OVERRUN,
	NUM_TEXT_DRAW_MODES
};

enum eCompare
{
	COMPARE_NEVER,       // GL_NEVER
	COMPARE_LESS,        // GL_LESS
	COMPARE_LEQUAL,      // GL_LEQUAL
	COMPARE_GREATER,     // GL_GREATER
	COMPARE_GEQUAL,      // GL_GEQUAL
	COMPARE_EQUAL,       // GL_EQUAL
	COMPARE_NOT_EQUAL,   // GL_NOTEQUAL
	COMPARE_ALWAYS,      // GL_ALWAYS
};

enum eFillMode
{
	FILL_MODE_WIRE,
	FILL_MODE_FILL,
	NUM_FILL_MODES

};

enum eCullMode 
{
	CULLMODE_BACK,          // GL_BACK     glEnable(GL_CULL_FACE); glCullFace(GL_BACK); 
	CULLMODE_FRONT,         // GL_FRONT    glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); 
	CULLMODE_NONE,          // GL_NONE     glDisable(GL_CULL_FACE)
};

enum eWindOrder 
{
	WIND_CLOCKWISE,         // GL_CW       glFrontFace( GL_CW ); 
	WIND_COUNTER_CLOCKWISE, // GL_CCW      glFrontFace( GL_CCW );  
};

enum eBlendMode
{
	BLEND_MODE_ALPHA,
	BLEND_MODE_ADDITIVE,		
	BLEND_MODE_MULTIPLY,
	BLEND_MODE_NONE,
	NUM_BLEND_MODES

};


enum eRenderDataType 
{
	RDT_FLOAT,           // GL_FLOAT
	RDT_UNSIGNED_BYTE,   // GL_UNSIGNED_BYTE
						 // ... add more as you need them
};



struct RenderState
{
	// Raster State Control
	eCullMode m_cullMode	= CULLMODE_BACK;      // CULL_BACK
	eFillMode m_fillMode	= FILL_MODE_FILL;      // FILL_SOLID
	eWindOrder m_frontFace	= WIND_COUNTER_CLOCKWISE;    // WIND_COUNTER_CLOCKWISE

	// Depth State Control
	eCompare m_depthCompare	= COMPARE_LESS;   // COMPARE_LESS
	bool m_depthWrite		= true;         // true

	// Blend
	eBlendMode m_blendMode	= BLEND_MODE_ALPHA;
}; 



// everything for the light UBO
struct light_t
{
	Vector4 color;

	Vector3 pos;
	float usesShadows;
	
	Vector3 direction;				//for a point light, this isn't really used in the final value, but still used in calculations so don't fuck it!
	float is_point_light;		//is a directional light = 0.f? or point light = 1.f?
	
	Vector3 attenuation;
	float dot_inner_angle;			//for cone light;
	
	Vector3 spec_attenuation;		//if you want it to be different :)
	float dot_outer_angle;		//for cone light

	Matrix44 shadowVP;
	
};

// UBO: uboLights
struct light_buffer_t
{
	light_t m_lights[MAX_LIGHTS]; 
};


const static float s_goldenRatioConjugate = 0.618033988749895f;
const static float s_goldenAngleDegrees = 137.5077640500378546463487f;

typedef unsigned char byte_t;

const unsigned long KB = 1024;

const float SQRT_2_OVER_2 = (float)(sqrt(2))/2.f;

const IntVector2	STEP_EAST				= IntVector2(+1,0);
const IntVector2	STEP_WEST				= IntVector2(-1,0);
const IntVector2	STEP_NORTH				= IntVector2(+0,+1);
const IntVector2	STEP_SOUTH				= IntVector2(+0,-1);
const IntVector2	STEP_NORTHEAST			= IntVector2(+1,+1);
const IntVector2	STEP_NORTHWEST			= IntVector2(-1,+1);
const IntVector2	STEP_SOUTHWEST			= IntVector2(-1,-1);
const IntVector2	STEP_SOUTHEAST			= IntVector2(+1,-1);


const Vector2		DIRECTION_EAST			= Vector2(+1.f,0.f);
const Vector2		DIRECTION_WEST			= Vector2(-1.F,0.f);
const Vector2		DIRECTION_NORTH			= Vector2(+0.f,+1.f);
const Vector2		DIRECTION_SOUTH			= Vector2(+0.f,-1.f);
const Vector2		DIRECTION_NORTHEAST		= Vector2(+SQRT_2_OVER_2, +SQRT_2_OVER_2);
const Vector2		DIRECTION_NORTHWEST		= Vector2(-SQRT_2_OVER_2, +SQRT_2_OVER_2);
const Vector2		DIRECTION_SOUTHWEST		= Vector2(-SQRT_2_OVER_2, -SQRT_2_OVER_2);
const Vector2		DIRECTION_SOUTHEAST		= Vector2(SQRT_2_OVER_2, -SQRT_2_OVER_2);



template <typename T>
void RemoveAt(std::vector<T*>& array, unsigned int index);

template <typename T>
void RemoveAt(std::vector<T*>& array, unsigned int index)
{
	array.erase(array.begin() + index);
}

template <typename T>
void RemoveAtFast(std::vector<T*>& array, unsigned int index);

template <typename T>
void RemoveAtFast(std::vector<T*>& array, unsigned int index)
{
	if (array.size() > 1){
		array[index] = array[array.size() - 1];
	}
	array.pop_back();
}

template <typename T>
bool Contains(std::vector<T*>& array, T* obj){
	for (T* element: array){
		if (element == obj){
			return true;
		}
	}
	return false;
}

template <typename T>
void Shuffle(std::vector<T*>& array, unsigned int numTimesToShuffle = 2)
{
	for (unsigned int i = 0; i < numTimesToShuffle; i++)
	{
		for (int arrayIndex = 0; arrayIndex < (int) array.size(); arrayIndex++)
		{
			int randomIndex = GetRandomIntLessThan((int) array.size());
			std::swap(array[arrayIndex], array[randomIndex]);
		}
	}
}


//template specialization! neat!
// https://stackoverflow.com/questions/14466620/c-template-specialization-calling-methods-on-types-that-could-be-pointers-or/14466705
template<typename T>
T * ptr(T & obj) { return &obj; }; //turn reference into pointer!

template<typename T>
T * ptr(T * obj) { return obj; }; //obj is already pointer, return it!




eCullMode	StringToCullMode	( std::string text);
eFillMode	StringToFillMode	( std::string text);
eWindOrder	StringToFrontFace	( std::string text);
eBlendMode	StringToBlendMode	( std::string text);
eCompare	StringToDepthCompare( std::string text);