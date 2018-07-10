#include "Game/GameCommon.hpp"

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_theAudio = nullptr;
SpriteSheet* g_tileSpriteSheet = nullptr;
XboxController* g_primaryController = nullptr;

eAIBehavior GetBehaviorFromString(std::string behavior)
{
	if (behavior == "Wander"){
		return BEHAVIOR_WANDER;
	}
	if (behavior == "Follow"){
		return BEHAVIOR_FOLLOW;
	}
	if (behavior == "Attack"){
		return BEHAVIOR_ATTACK;
	}
	return BEHAVIOR_NONE;
}
