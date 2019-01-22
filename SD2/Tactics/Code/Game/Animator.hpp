#pragma once
#include "Game/GameCommon.hpp" 

class IsoSprite;
class AnimSet;
class IsoAnimation;

class Animator 
{
public:
	Animator(AnimSet* animSet, IsoSprite* defaultSprite);

	void Update(float deltaSeconds);

	void Play( std::string name, ePlaybackMode loop_override = PLAYBACK_MODE_LOOP ) ;
	void FinishAnimation();
	
	IsoSprite* GetCurrentIsoSprite();

public:
	AnimSet *m_animSet; 

	float m_timeIntoAnimation; 
	IsoAnimation *m_currentAnimation; 

	IsoSprite *m_defaultSprite; 


};