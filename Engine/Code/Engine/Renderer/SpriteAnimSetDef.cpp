#include "SpriteAnimSetDef.hpp"
#include "Engine/Renderer/SpriteAnimDef.hpp"
#include "Engine/Math/Renderer.hpp"

//SpriteAnimSetDef::SpriteAnimSetDef(SpriteAnimSetDef * animSetDef)
//{
//	m_defaultAnimName = animSetDef->m_defaultAnimName;
//	m_spriteAnimDefs = animSetDef->m_spriteAnimDefs;
//	m_spriteSheet = animSetDef->m_spriteSheet;
//}

std::map<std::string, SpriteAnimSetDef*>	SpriteAnimSetDef::s_animSetDefs;

SpriteAnimSetDef::SpriteAnimSetDef(tinyxml2::XMLElement * animSetElement)
{
	m_defaultAnimName = ParseXmlAttribute(*animSetElement, "defaultAnim", m_defaultAnimName);
	
	LoadAnimDefs(animSetElement);
	m_name = ParseXmlAttribute(*animSetElement, "name", m_name);
}

SpriteAnimSetDef::~SpriteAnimSetDef()
{
}

SpriteSheet * SpriteAnimSetDef::GetSpriteSheet()
{
	return m_spriteSheet;
}

void SpriteAnimSetDef::SetSpriteSheetTexture(std::string tex)
{
	Texture* sheetTexture = Texture::CreateOrGetTexture(tex);
	m_spriteSheet = new SpriteSheet(*sheetTexture, m_spriteSheet->GetDimensions().x, m_spriteSheet->GetDimensions().y);
}

// SAMPLE XML:
//	<SpriteAnimSet spriteSheet="Kushnariova_12x53.png" spriteLayout="12,53" fps="10">
//		<SpriteAnim name="MoveEast"		spriteIndexes="3,4,5,4"/>
//		<SpriteAnim name="MoveWest"		spriteIndexes="9,10,11,10"/>
//		<SpriteAnim name="MoveNorth"	spriteIndexes="0,1,2,1"/>
//		<SpriteAnim name="MoveSouth"	spriteIndexes="6,7,8,7"/>
//		<SpriteAnim name="Idle"		spriteIndexes="6,7,8,7" fps="3"/>
//	</SpriteAnimSet>

SpriteAnimSetDef * SpriteAnimSetDef::GetSpriteAnimSetDef(std::string name)
{
	auto contains = s_animSetDefs.find((std::string)name); 
	SpriteAnimSetDef* def = nullptr;
	if (contains != s_animSetDefs.end()){
		def = contains->second;
	} 
	return def;
}

void SpriteAnimSetDef::LoadSetsFromFile(std::string file)
{
	tinyxml2::XMLDocument animDoc;
	std::string filePath = "Data/Data/" + file;
	animDoc.LoadFile(filePath.c_str());

	for (tinyxml2::XMLElement* animDefElement = animDoc.FirstChildElement("SpriteAnimSet"); animDefElement != NULL; animDefElement = animDefElement->NextSiblingElement("SpriteAnimSet")){
		SpriteAnimSetDef* animsetdef = new SpriteAnimSetDef(animDefElement);
		SpriteAnimSetDef::s_animSetDefs.insert(std::pair<std::string, SpriteAnimSetDef*>(animsetdef->m_name, animsetdef));
	}
	return;
}

void SpriteAnimSetDef::LoadAnimDefs(tinyxml2::XMLElement * animSetElement)
{
	std::string spriteSheetName = ParseXmlAttribute(*animSetElement, "spriteSheet", (std::string) " ");
	if (!spriteSheetName.compare(" ")){
		ERROR_AND_DIE("Must Specify A Sprite Sheet for Anim Sets! " + spriteSheetName);
	} else {
		m_name = spriteSheetName;		//default to sprite sheeet name
		Texture* sheetTexture = Texture::CreateOrGetTexture(spriteSheetName);
		IntVector2 spriteLayout = ParseXmlAttribute(*animSetElement, "spriteLayout", IntVector2(0,0));
		m_spriteSheet = new SpriteSheet(*sheetTexture, spriteLayout.x, spriteLayout.y);
		float defaultFPS = ParseXmlAttribute(*animSetElement, "fps", 10.f);
		
		//fill m_spriteAnimDefs
		for (tinyxml2::XMLElement* animElement = animSetElement->FirstChildElement("SpriteAnim"); animElement != NULL; animElement = animElement->NextSiblingElement("SpriteAnim")){
			SpriteAnimDef* newDefinition = new SpriteAnimDef(animElement, m_spriteSheet, defaultFPS);
			m_spriteAnimDefs.insert(std::pair<std::string, SpriteAnimDef*>(newDefinition->m_name, newDefinition));
		}
	}
	
}
