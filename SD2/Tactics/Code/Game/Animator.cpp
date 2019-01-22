#include "Animator.hpp"
#include "Game/IsoAnimation.hpp"
#include "Game/AnimSet.hpp"

Animator::Animator(AnimSet * animSet, IsoSprite* defaultSprite)
{
	m_animSet = animSet;
	m_defaultSprite = defaultSprite;
}

void Animator::Update(float deltaSeconds)
{
	m_timeIntoAnimation+=deltaSeconds;
	if (m_currentAnimation != nullptr && m_currentAnimation->IsFinished()){
		FinishAnimation();
	}
	
}

void Animator::Play(std::string name, ePlaybackMode loop_override)
{
	UNUSED(loop_override);
	if (m_animSet != nullptr) {
		std::string newName = m_animSet->GetAnimName( name ); 
		if (newName != "") {
			name = newName; 
		}
	}
	m_currentAnimation = IsoAnimation::GetAnimation( name ); 
	m_timeIntoAnimation = 0.0f; 

}

void Animator::FinishAnimation()
{
	m_currentAnimation = nullptr;
}

IsoSprite * Animator::GetCurrentIsoSprite()
{
	if (m_currentAnimation != nullptr) {
		return m_currentAnimation->Evaluate( m_timeIntoAnimation ); 
	} else {
		return m_defaultSprite; 
	}
}
