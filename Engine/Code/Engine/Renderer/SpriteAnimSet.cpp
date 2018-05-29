#include "SpriteAnimSet.hpp"
#include "SpriteAnim.hpp"
#include "SpriteAnimSetDef.hpp"

SpriteAnimSet::SpriteAnimSet(SpriteAnimSetDef * spriteAnimSetDef)
{
	m_definition = spriteAnimSetDef;
	LoadAnims();
	SetCurrentAnim(m_definition->m_defaultAnimName);
}

SpriteAnimSet::~SpriteAnimSet()
{
	for (std::pair<std::string, SpriteAnim*> animPair : m_namedAnims){
		delete (animPair.second);
	}
}

void SpriteAnimSet::Update(float deltaSeconds)
{
	m_currentAnim->Update(deltaSeconds);
}

void SpriteAnimSet::SetCurrentAnim(const std::string & animName)
{

	if (m_currentAnim == nullptr || m_currentAnim->GetName().compare(animName)){
		SpriteAnim* anim = GetAnim(animName);
		if (anim == nullptr){
			ERROR_AND_DIE("No anim named: " + animName);
		} else {
			m_currentAnim = anim;
			m_currentAnim->Reset();
		}
	}
	
}

const Texture * SpriteAnimSet::GetCurrentTexture() const
{

	return m_currentAnim->GetTexture();
}

const Texture * SpriteAnimSet::GetTextureForAnim(std::string animName)
{
	SpriteAnim* animToShow = GetAnim(animName);
	return animToShow->GetTexture();
}

AABB2 SpriteAnimSet::GetCurrentUVs() const
{
	return m_currentAnim->GetCurrentTexCoords();
}

AABB2 SpriteAnimSet::GetUVsForAnim(std::string animName, float time)
{
	SpriteAnim* animToShow = GetAnim(animName);
	return animToShow->GetTexCoordsAtTime(time);
}

bool SpriteAnimSet::IsCurrentAnimFinished()
{
	return m_currentAnim->IsFinished();
}

void SpriteAnimSet::LoadAnims()
{
	//make animations based on definition
	std::string animDefName = " ";
	SpriteAnimDef* animDef = nullptr;
	for( const auto& animDefPair : m_definition->m_spriteAnimDefs )
	{
		animDefName = animDefPair.first;
		animDef = animDefPair.second;
		SpriteAnim* anim = new SpriteAnim(animDef);
		m_namedAnims.insert(std::pair<std::string, SpriteAnim*> (animDefName, anim));

	}
}

SpriteAnim * SpriteAnimSet::GetAnim(const std::string & animName)
{
	auto containsDef = m_namedAnims.find((std::string)animName); 
	SpriteAnim* anim = nullptr;
	if (containsDef != m_namedAnims.end()){
		anim = containsDef->second;
	}
	return anim;
}
