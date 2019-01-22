#include "Game/GameCommon.hpp"

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_theAudio = nullptr;
SpriteSheet* g_tileSpriteSheet = nullptr;
SpriteSheet* g_portraitSpriteSheet = nullptr;
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

RGBA GetRandomLipColor()
{
	float redChance = .7f;
	RGBA color;
	//chance for a reddish lip
	if (CheckRandomChance(redChance)){
		//basically just randomly add shit to the values a few times to fluctuate from pure red
		int r = 255;
		int g = 64;
		int b = 68;
		for (int i = 0; i < GetRandomIntInRange(3,10); i++){
			if (CheckRandomChance(.33f)){
				r += GetRandomIntInRange(-15,10);
			}
			if (CheckRandomChance(.33f)){
				g+= GetRandomIntInRange(-10, 10);
			}
			if (CheckRandomChance(.33f)){
				b+= GetRandomIntInRange(-10, 10);
			}
		}
		r = ClampInt(r, 100, 255);
		g = ClampInt(g, 0, 220);
		b = ClampInt(b, 0, 220);
		color = RGBA(r,g,b);
	} else {
		//chance for a bright ass lip
		color = RGBA::GetRandomRainbowColor();
	}
	return color;
}

RGBA GetRandomEyeColor()
{
	return RGBA::GetRandomMutedColor();
}
