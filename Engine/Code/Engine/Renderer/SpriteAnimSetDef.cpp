#include "SpriteAnimSetDef.hpp"
#include "Engine/Renderer/SpriteAnimDef.hpp"
#include "Engine/Math/Renderer.hpp"

SpriteAnimSetDef::SpriteAnimSetDef(tinyxml2::XMLElement * animSetElement, Renderer & renderer)
{
	m_defaultAnimName = ParseXmlAttribute(*animSetElement, "defaultAnim", m_defaultAnimName);
	LoadAnimDefs(animSetElement, renderer);
}

SpriteAnimSetDef::~SpriteAnimSetDef()
{
}

SpriteSheet * SpriteAnimSetDef::GetSpriteSheet()
{
	return m_spriteSheet;
}

// SAMPLE XML:
//	<SpriteAnimSet spriteSheet="Kushnariova_12x53.png" spriteLayout="12,53" fps="10">
//		<SpriteAnim name="MoveEast"		spriteIndexes="3,4,5,4"/>
//		<SpriteAnim name="MoveWest"		spriteIndexes="9,10,11,10"/>
//		<SpriteAnim name="MoveNorth"	spriteIndexes="0,1,2,1"/>
//		<SpriteAnim name="MoveSouth"	spriteIndexes="6,7,8,7"/>
//		<SpriteAnim name="Idle"		spriteIndexes="6,7,8,7" fps="3"/>
//	</SpriteAnimSet>

void SpriteAnimSetDef::LoadAnimDefs(tinyxml2::XMLElement * animSetElement, Renderer& renderer)
{
	std::string spriteSheetName = ParseXmlAttribute(*animSetElement, "spriteSheet", (std::string) " ");
	if (!spriteSheetName.compare(" ")){
		ERROR_AND_DIE("Must Specify A Sprite Sheet for Anim Sets! " + spriteSheetName);
	} else {
		Texture* sheetTexture = renderer.CreateOrGetTexture(spriteSheetName);
		IntVector2 spriteLayout = ParseXmlAttribute(*animSetElement, "spriteLayout", IntVector2(0,0));
		m_spriteSheet = new SpriteSheet(*sheetTexture, spriteLayout.x, spriteLayout.y);
		float defaultFPS = ParseXmlAttribute(*animSetElement, "fps", 10.f);
		
		//fill m_spriteAnimDefs
		for (tinyxml2::XMLElement* animElement = animSetElement->FirstChildElement("SpriteAnim"); animElement != NULL; animElement = animElement->NextSiblingElement("SpriteAnim")){
			SpriteAnimDef* newDefinition = new SpriteAnimDef(animElement, m_spriteSheet, defaultFPS, renderer);
			m_spriteAnimDefs.insert(std::pair<std::string, SpriteAnimDef*>(newDefinition->m_name, newDefinition));
		}
	}
	
}
