#pragma once
#include "Engine/Core/EngineCommon.hpp"

class SpriteAnimDef;
class SpriteSheet;

class SpriteAnimSetDef{
	friend class SpriteAnimSet;
public:
	//SpriteAnimSetDef(SpriteAnimSetDef* animSetDef);
	SpriteAnimSetDef(tinyxml2::XMLElement* animSetElement);
	~SpriteAnimSetDef();


	SpriteSheet* GetSpriteSheet();
	void SetSpriteSheetTexture(std::string tex);
	std::string m_defaultAnimName = "Idle";

	static std::map<std::string, SpriteAnimSetDef*> s_animSetDefs;
	static SpriteAnimSetDef* GetSpriteAnimSetDef(std::string name);
	static void LoadSetsFromFile(std::string file);

private:
	void LoadAnimDefs(tinyxml2::XMLElement* animSetElement);
	std::map<std::string, SpriteAnimDef*> m_spriteAnimDefs;
	SpriteSheet* m_spriteSheet;
	std::string m_name;
	

};