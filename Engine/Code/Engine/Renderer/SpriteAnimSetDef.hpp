#pragma once
#include "Engine/Core/EngineCommon.hpp"

class SpriteAnimDef;
class SpriteSheet;

class SpriteAnimSetDef{
	friend class SpriteAnimSet;
public:
	SpriteAnimSetDef(tinyxml2::XMLElement* animSetElement, Renderer& renderer);
	~SpriteAnimSetDef();


	SpriteSheet* GetSpriteSheet();
	std::string m_defaultAnimName = "Idle";

private:
	void LoadAnimDefs(tinyxml2::XMLElement* animSetElement, Renderer& renderer);
	std::map<std::string, SpriteAnimDef*> m_spriteAnimDefs;
	SpriteSheet* m_spriteSheet;
	

};