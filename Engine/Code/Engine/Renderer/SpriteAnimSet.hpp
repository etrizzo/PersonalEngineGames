#pragma once
#include "Engine/Core/EngineCommon.hpp"

class SpriteAnim;
class SpriteAnimSetDef;

class SpriteAnimSet{
public:
	SpriteAnimSet(SpriteAnimSetDef* spriteAnimSetDef);
	~SpriteAnimSet();

	void Update(float deltaSeconds);
	void SetCurrentAnim(const std::string& animName);		//sets new anim and resets it
	Texture* GetCurrentTexture() const;
	Texture* GetTextureForAnim(std::string animName);
	AABB2 GetCurrentUVs() const;
	AABB2 GetUVsForAnim(std::string animName, float time = 0.f);

	bool IsCurrentAnimFinished();

private:
	void LoadAnims();
	SpriteAnim* GetAnim(const std::string& animName);

	SpriteAnimSetDef* m_definition = nullptr;
	std::map<std::string, SpriteAnim*>	m_namedAnims;
	SpriteAnim* m_currentAnim = nullptr;
	SpriteAnim* m_nextAnim = nullptr;

};