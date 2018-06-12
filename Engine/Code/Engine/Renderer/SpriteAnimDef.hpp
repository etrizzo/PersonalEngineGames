#pragma once
#include "Engine/Core/EngineCommon.hpp"


//class SpriteAnimSetDef;
class SpriteSheet;



class SpriteAnimDef{
	friend class SpriteAnim;
	friend class SpriteAnimSetDef;
public:
	SpriteAnimDef(tinyxml2::XMLElement* spriteAnimDef, SpriteSheet* defaultSpriteSheet, float defaultFPS);
	~SpriteAnimDef();

	SpriteSheet* GetSpriteSheet();
	float GetDuration() const {return (float) m_spriteIndexes.size() / m_framesPerSecond;}

	SpriteSheet* m_spriteSheet = nullptr;
	ePlaybackMode m_playbackMode;
	float m_framesPerSecond = 10.f;
	std::string m_name;
	std::vector<int> m_spriteIndexes;

	SpriteAnimSetDef* m_animSetDef;


private:
	void ParseSpriteIndices(tinyxml2::XMLElement* spriteAnimDef);

};