#include "Engine/Renderer/SpriteAnimDef.hpp"
#include "Engine/Renderer/SpriteAnimSetDef.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/Renderer.hpp"

SpriteAnimDef::SpriteAnimDef(tinyxml2::XMLElement* spriteAnimDefElement, SpriteSheet* defaultSpriteSheet, float defaultFPS)
{
	m_name = ParseXmlAttribute(*spriteAnimDefElement, "name", (std::string) "NONAME");
	
	//parse spritesheet
	std::string spriteSheetName = ParseXmlAttribute(*spriteAnimDefElement, "spriteSheet", (std::string)"");
	if (!spriteSheetName.compare((std::string)"")){
		m_spriteSheet = defaultSpriteSheet;
	} else {
		Texture* sheetTexture = Texture::CreateOrGetTexture(spriteSheetName);
		IntVector2 sheetDimensions = ParseXmlAttribute(*spriteAnimDefElement, "spriteLayout", IntVector2(0,0));
		m_spriteSheet =  new SpriteSheet(*sheetTexture, sheetDimensions.x, sheetDimensions.y);
	}
	
	m_framesPerSecond = ParseXmlAttribute(*spriteAnimDefElement, "fps", defaultFPS);
	m_playbackMode = (ePlaybackMode) ParseXmlAttribute(*spriteAnimDefElement, "playbackMode", PLAYBACK_MODE_LOOP);
	
	ParseSpriteIndices(spriteAnimDefElement);
	



	/*m_durationSeconds = durationSeconds;
	m_playbackMode = playbackMode;
	m_startFrameSpriteIndex = startSpriteIndex;
	m_numFrames = endSpriteIndex - startSpriteIndex;*/
}


SpriteAnimDef::~SpriteAnimDef()
{
	delete m_spriteSheet;
}

void SpriteAnimDef::ParseSpriteIndices(tinyxml2::XMLElement* spriteAnimElement){
	std::string stringText = ParseXmlAttribute(*spriteAnimElement, "spriteIndexes", (std::string) " " );
	unsigned int tildeIndex = (unsigned int) stringText.find('~');
	if (tildeIndex != std::string::npos){
		m_indices = IntRange(0);
		m_indices.SetFromText(stringText.c_str());
		//determine how to read the sprite indices: in order, or choose one randomly from the range
		std::string type = ParseXmlAttribute(*spriteAnimElement, "type", "inOrder");
		if (type == "inOrder"){
			for(int i = m_indices.min; i < m_indices.max; i++){
				m_spriteIndexes.push_back(i);
			}
		} else if (type == "oneInRange"){
			m_chooseOneIndexInRange = true;
		}
	} else {
		unsigned int oldIndex = 0;
		unsigned int commaIndex = (unsigned int) stringText.find(',');
		if (stringText.compare((std::string) " ")){
			while (commaIndex != std::string::npos){
				std::string indexString = stringText.substr(oldIndex,commaIndex);
				m_spriteIndexes.push_back(atoi(indexString.c_str())); 

				oldIndex = commaIndex + 1;
				commaIndex = (unsigned int) stringText.find(',', oldIndex);
			}
			std::string indexString = stringText.substr(oldIndex);
			m_spriteIndexes.push_back(atoi(indexString.c_str())); 
		} else {
			ERROR_AND_DIE("Must specify sprite indices for sprite anim: " + m_name);
		}
	}
}


SpriteSheet * SpriteAnimDef::GetSpriteSheet()
{
	return m_spriteSheet;
}
