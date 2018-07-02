#include "ClothingSetDefinition.hpp"
#include "ClothingSet.hpp"

std::map<std::string, ClothingSetDefinition*>	ClothingSetDefinition::s_definitions;

ClothingSetDefinition::ClothingSetDefinition(tinyxml2::XMLElement * setElement)
{
	m_name = ParseXmlAttribute(*setElement, "name", "NO_NAME");
	//m_texturesByClothingType = std::vector<std::vector<Texture*>>();
	//m_texturesByClothingType.resize(NUM_RENDER_SLOTS);
	//m_tintsByClothingType = std::vector<std::vector<RGBA>>();
	//m_tintsByClothingType.resize(NUM_RENDER_SLOTS);
	m_layersByClothingType = std::vector< std::vector<ClothingLayer*>>();
	m_layersByClothingType.resize(NUM_RENDER_SLOTS);

	ParseBodys(setElement);
	ParseTorsos(setElement);
	ParseLegs(setElement);
	ParseHairs(setElement);
	ParseHats(setElement);
	//ParseWeapons(setElement);
}

ClothingSet * ClothingSetDefinition::GetRandomSet() const
{
	ClothingSet* set = new ClothingSet();
	TODO("Combine tint and texture")
	if ((int)  m_layersByClothingType[CHEST_SLOT].size() > 0){
		//load chest clothing
		int torsoIndex = GetRandomIntLessThan((int) m_layersByClothingType[CHEST_SLOT].size());
		set->InitLayer(CHEST_SLOT, GetLayer(CHEST_SLOT, torsoIndex));
		//if that chest clothing should render pants, get random pants
		if ( GetLayer(CHEST_SLOT, torsoIndex)->ShouldRenderLegs()){
			set->InitLayer(LEGS_SLOT, GetRandomOfType(LEGS_SLOT));
		}
	}
	if ((int)  m_layersByClothingType[BODY_SLOT].size() > 0){
		//load body
		int bodyIndex = GetRandomIntLessThan((int) m_layersByClothingType[BODY_SLOT].size());
		set->InitLayer(BODY_SLOT,		GetLayer(BODY_SLOT, bodyIndex));		//init body and ears of same skin tone
		set->InitLayer(EARS_SLOT,		GetLayer(EARS_SLOT, bodyIndex));
	}
	if (m_layersByClothingType[HEAD_SLOT].size() > 0){
		int hairIndex = GetRandomIntLessThan((int) m_layersByClothingType[HEAD_SLOT].size());
		set->InitLayer(HEAD_SLOT,		GetLayer(HEAD_SLOT, hairIndex));
	}
	set->InitLayer(HAT_SLOT,		GetRandomOfType(HAT_SLOT));
	set->InitLayer(WEAPON_SLOT,		GetRandomOfType(WEAPON_SLOT));
	return set;
}

ClothingLayer * ClothingSetDefinition::GetRandomOfType(RENDER_SLOT slot) const
{
	std::vector<ClothingLayer*> typeList = m_layersByClothingType[slot];
	if (typeList.size() > 0){
		int index = GetRandomIntLessThan((int) typeList.size());
		return typeList[index];
	}
	return nullptr;
}

ClothingLayer * ClothingSetDefinition::GetRandomTorso() const
{
	return nullptr;
}

ClothingLayer * ClothingSetDefinition::GetRandomLegs() const
{
	return nullptr;
}

ClothingLayer * ClothingSetDefinition::GetRandomHair() const
{
	return nullptr;
}

ClothingLayer * ClothingSetDefinition::GetRandomHat() const
{
	return nullptr;
}

ClothingSetDefinition * ClothingSetDefinition::GetDefinition(std::string definitionName)
{
	auto containsDef = s_definitions.find((std::string)definitionName); 
	ClothingSetDefinition* def = nullptr;
	if (containsDef != s_definitions.end()){
		def = containsDef->second;
	} else {
		ERROR_AND_DIE("No clothing set named: " + definitionName);
	}
	return def;
}

RGBA ClothingSetDefinition::GetColorFromXML(std::string text)
{
	if (text == "randomBright"){
		return RGBA::GetRandomRainbowColor();
	}
	if (text == "random"){
		return RGBA::GetRandomColor();
	}
	return RGBA::WHITE;
}

ClothingLayer * ClothingSetDefinition::GetLayer(RENDER_SLOT slot, int index) const
{
	return m_layersByClothingType[slot][index];
}

Texture * ClothingSetDefinition::GetTexture(RENDER_SLOT slot, int index) const
{
	return GetLayer(slot, index)->GetTexture();
}

RGBA ClothingSetDefinition::GetTint(RENDER_SLOT slot, int index) const
{
	return GetLayer(slot, index)->GetTint();
}

void ClothingSetDefinition::ParseBodys(tinyxml2::XMLElement * setElement)
{
	tinyxml2::XMLElement* allBodysElement = setElement->FirstChildElement("Bodys");
	if (allBodysElement != nullptr){
		//parse all body elements
		for(tinyxml2::XMLElement* bodyElement = allBodysElement->FirstChildElement("Body"); bodyElement != nullptr; bodyElement = bodyElement->NextSiblingElement("Body")){
			ParseBody(bodyElement);
		}
	}
}

void ClothingSetDefinition::ParseTorsos(tinyxml2::XMLElement * setElement)
{
	tinyxml2::XMLElement* allTorsosElement = setElement->FirstChildElement("Torsos");
	if (allTorsosElement != nullptr){
		//parse all body elements
		int i = 0;
		for(tinyxml2::XMLElement* torsoElement = allTorsosElement->FirstChildElement("Torso"); torsoElement != nullptr; torsoElement = torsoElement->NextSiblingElement("Torso")){
			Texture* torsoTexture = ParseXmlAttribute(*torsoElement, "texture", (Texture*) nullptr);
			std::string colorType = ParseXmlAttribute(*torsoElement, "color", "NONE");
			RGBA tint = GetColorFromXML(colorType);
			bool usesLegs = ParseXmlAttribute(*torsoElement, "useLegs", true);
			bool usesHair = ParseXmlAttribute(*torsoElement, "usesHair", true);

			ClothingLayer* layer = new ClothingLayer(CHEST_SLOT, torsoTexture, tint, usesLegs, usesHair);
			m_layersByClothingType[CHEST_SLOT].push_back(layer);
			//m_texturesByClothingType[CHEST_SLOT].push_back(torsoTexture);
			////store whether that torso texture can use a leg texture too (i.e., dresses shouldn't have leg textures)
			//
			//m_torsoUsesLegs.resize(i+1);
			//m_torsoUsesLegs[i] = usesLegs;

			//
			//m_tintsByClothingType[CHEST_SLOT].push_back(tint);

			//i++;
		}
	}
}

void ClothingSetDefinition::ParseLegs(tinyxml2::XMLElement * setElement)
{
	tinyxml2::XMLElement* allLegsElements = setElement->FirstChildElement("Legs");
	if (allLegsElements != nullptr){
		//parse all body elements
		for(tinyxml2::XMLElement* legElement = allLegsElements->FirstChildElement("Leg"); legElement != nullptr; legElement = legElement->NextSiblingElement("Leg")){
			//Texture* legTexture = ParseXmlAttribute(*legElement, "texture", (Texture*) nullptr);
			//m_texturesByClothingType[LEGS_SLOT].push_back(legTexture);

			//std::string colorType = ParseXmlAttribute(*legElement, "color", "NONE");
			//RGBA tint = GetColorFromXML(colorType);
			//m_tintsByClothingType[LEGS_SLOT].push_back(tint);
			Texture* torsoTexture = ParseXmlAttribute(*legElement, "texture", (Texture*) nullptr);
			std::string colorType = ParseXmlAttribute(*legElement, "color", "NONE");
			RGBA tint = GetColorFromXML(colorType);
			bool usesLegs = ParseXmlAttribute(*legElement, "useLegs", true);
			bool usesHair = ParseXmlAttribute(*legElement, "usesHair", true);

			ClothingLayer* layer = new ClothingLayer(LEGS_SLOT, torsoTexture, tint, usesLegs, usesHair);
			m_layersByClothingType[LEGS_SLOT].push_back(layer);
		}
	}
}

void ClothingSetDefinition::ParseHairs(tinyxml2::XMLElement * setElement)
{
	tinyxml2::XMLElement* allHairElements = setElement->FirstChildElement("Hairs");
	if (allHairElements != nullptr){
		//parse all body elements
		for(tinyxml2::XMLElement* hairElement = allHairElements->FirstChildElement("Hair"); hairElement != nullptr; hairElement = hairElement->NextSiblingElement("Hair")){
			//Texture* hairTexture = ParseXmlAttribute(*hairElement, "texture", (Texture*) nullptr);
			//m_texturesByClothingType[HEAD_SLOT].push_back(hairTexture);

			//std::string colorType = ParseXmlAttribute(*hairElement, "color", "NONE");
			//RGBA tint = GetColorFromXML(colorType);
			//m_tintsByClothingType[HEAD_SLOT].push_back(tint);
			Texture* torsoTexture = ParseXmlAttribute(*hairElement, "texture", (Texture*) nullptr);
			std::string colorType = ParseXmlAttribute(*hairElement, "color", "NONE");
			RGBA tint = GetColorFromXML(colorType);
			bool usesLegs = ParseXmlAttribute(*hairElement, "useLegs", true);
			bool usesHair = ParseXmlAttribute(*hairElement, "usesHair", true);

			ClothingLayer* layer = new ClothingLayer(HEAD_SLOT, torsoTexture, tint, usesLegs, usesHair);
			m_layersByClothingType[HEAD_SLOT].push_back(layer);
		}
	}
}

void ClothingSetDefinition::ParseHats(tinyxml2::XMLElement * setElement)
{
	tinyxml2::XMLElement* allHatElements = setElement->FirstChildElement("Hats");
	if (allHatElements != nullptr){
		//parse all body elements
		for(tinyxml2::XMLElement* hatElement = allHatElements->FirstChildElement("Hat"); hatElement != nullptr; hatElement = hatElement->NextSiblingElement("Hat")){
			/*Texture* hatTexture = ParseXmlAttribute(*hatElement, "texture", (Texture*) nullptr);
			m_texturesByClothingType[HAT_SLOT].push_back(hatTexture);

			std::string colorType = ParseXmlAttribute(*hatElement, "color", "NONE");
			RGBA tint = GetColorFromXML(colorType);
			m_tintsByClothingType[HAT_SLOT].push_back(tint);*/
			Texture* hatTexture = ParseXmlAttribute(*hatElement, "texture", (Texture*) nullptr);
			std::string colorType = ParseXmlAttribute(*hatElement, "color", "NONE");
			RGBA tint = GetColorFromXML(colorType);
			bool usesLegs = ParseXmlAttribute(*hatElement, "useLegs", true);
			bool usesHair = ParseXmlAttribute(*hatElement, "usesHair", true);

			ClothingLayer* layer = new ClothingLayer(HAT_SLOT, hatTexture, tint, usesLegs, usesHair);
			m_layersByClothingType[HAT_SLOT].push_back(layer);
		}
	}
}

void ClothingSetDefinition::ParseBody(tinyxml2::XMLElement * bodyElement)
{
	tinyxml2::XMLElement* baseElement = bodyElement->FirstChildElement("Base");
	tinyxml2::XMLElement* earsElement = bodyElement->FirstChildElement("Ears");
	//could also do face stuff here later

	Texture* baseTexture;
	Texture* earTexture;

	if (baseElement != nullptr){
		//std::string baseTexName = ParseXmlAttribute(*baseElement, "texture", "NO_BASE");
		//baseTexture = Texture::CreateOrGetTexture(baseTexName);
		baseTexture = ParseXmlAttribute(*baseElement, "texture", (Texture*) nullptr);

	} else {
		ERROR_AND_DIE("Must specify texture for body base!");
	}
	
	if (earsElement != nullptr){
		std::string earTexName = ParseXmlAttribute(*earsElement, "texture", "NO_EARS");
		earTexture = Texture::CreateOrGetTexture(earTexName);
	} else {
		earTexture = nullptr;
	}
	std::string colorType = ParseXmlAttribute(*baseElement, "color", "NONE");
	RGBA tint = GetColorFromXML(colorType);
	bool usesLegs = ParseXmlAttribute(*baseElement, "useLegs", true);
	bool usesHair = ParseXmlAttribute(*baseElement, "usesHair", true);

	ClothingLayer* bodyLayer = new ClothingLayer(BODY_SLOT, baseTexture, tint, usesLegs, usesHair);
	ClothingLayer* earLayer = new ClothingLayer(EARS_SLOT, earTexture, tint, usesLegs, usesHair);

	m_layersByClothingType[BODY_SLOT].push_back(bodyLayer);
	m_layersByClothingType[EARS_SLOT].push_back(earLayer);

	//m_texturesByClothingType[BODY_SLOT].push_back(baseTexture);
	//m_texturesByClothingType[EARS_SLOT].push_back(earTexture);

	//std::string colorType = ParseXmlAttribute(*bodyElement, "color", "NONE");
	//RGBA tint = GetColorFromXML(colorType);
	//m_tintsByClothingType[BODY_SLOT].push_back(tint);
	//m_tintsByClothingType[EARS_SLOT].push_back(tint);

}
