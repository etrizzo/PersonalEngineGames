#include "ClothingSetDefinition.hpp"
#include "ClothingSet.hpp"

std::map<std::string, ClothingSetDefinition*>	ClothingSetDefinition::s_definitions;

ClothingSetDefinition::ClothingSetDefinition(tinyxml2::XMLElement * setElement)
{
	m_name = ParseXmlAttribute(*setElement, "name", "NO_NAME");
	m_texturesByClothingType = std::vector<std::vector<Texture*>>();
	m_texturesByClothingType.resize(NUM_RENDER_SLOTS);
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

	if ((int)  m_texturesByClothingType[CHEST_SLOT].size() > 0){
		//load chest clothing
		int torsoIndex = GetRandomIntLessThan((int) m_texturesByClothingType[CHEST_SLOT].size());
		set->InitTexture(CHEST_SLOT, GetTexture(CHEST_SLOT, torsoIndex), RGBA::GetRandomRainbowColor());
		if (m_torsoUsesLegs[torsoIndex]){
			set->InitTexture(LEGS_SLOT, GetRandomOfType(LEGS_SLOT));
		}
	}
	if ((int)  m_texturesByClothingType[BODY_SLOT].size() > 0){
		//load body
		int bodyIndex = GetRandomIntLessThan((int) m_texturesByClothingType[BODY_SLOT].size());
		set->InitTexture(BODY_SLOT,		GetTexture(BODY_SLOT, bodyIndex));		//init body and ears of same skin tone
		set->InitTexture(EARS_SLOT,		GetTexture(EARS_SLOT, bodyIndex));
	}
	set->InitTexture(HEAD_SLOT,		GetRandomOfType(HEAD_SLOT));
	set->InitTexture(HAT_SLOT,		GetRandomOfType(HAT_SLOT));
	set->InitTexture(WEAPON_SLOT,	GetRandomOfType(WEAPON_SLOT));
	return set;
}

Texture * ClothingSetDefinition::GetRandomOfType(RENDER_SLOT slot) const
{
	std::vector<Texture*> typeList = m_texturesByClothingType[slot];
	if (typeList.size() > 0){
		int index = GetRandomIntLessThan((int) typeList.size());
		return typeList[index];
	}
	return nullptr;
}

Texture * ClothingSetDefinition::GetRandomTorso() const
{
	return nullptr;
}

Texture * ClothingSetDefinition::GetRandomLegs() const
{
	return nullptr;
}

Texture * ClothingSetDefinition::GetRandomHair() const
{
	return nullptr;
}

Texture * ClothingSetDefinition::GetRandomHat() const
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

Texture * ClothingSetDefinition::GetTexture(RENDER_SLOT slot, int index) const
{
	return m_texturesByClothingType[slot][index];
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
			m_texturesByClothingType[CHEST_SLOT].push_back(torsoTexture);
			//store whether that torso texture can use a leg texture too (i.e., dresses shouldn't have leg textures)
			bool usesLegs = ParseXmlAttribute(*torsoElement, "useLegs", true);
			m_torsoUsesLegs.resize(i+1);
			m_torsoUsesLegs[i] = usesLegs;
			i++;
		}
	}
}

void ClothingSetDefinition::ParseLegs(tinyxml2::XMLElement * setElement)
{
	tinyxml2::XMLElement* allLegsElements = setElement->FirstChildElement("Legs");
	if (allLegsElements != nullptr){
		//parse all body elements
		for(tinyxml2::XMLElement* legElement = allLegsElements->FirstChildElement("Leg"); legElement != nullptr; legElement = legElement->NextSiblingElement("Leg")){
			Texture* legTexture = ParseXmlAttribute(*legElement, "texture", (Texture*) nullptr);
			m_texturesByClothingType[LEGS_SLOT].push_back(legTexture);
		}
	}
}

void ClothingSetDefinition::ParseHairs(tinyxml2::XMLElement * setElement)
{
	tinyxml2::XMLElement* allHairElements = setElement->FirstChildElement("Hairs");
	if (allHairElements != nullptr){
		//parse all body elements
		for(tinyxml2::XMLElement* hairElement = allHairElements->FirstChildElement("Hair"); hairElement != nullptr; hairElement = hairElement->NextSiblingElement("Hair")){
			Texture* hairTexture = ParseXmlAttribute(*hairElement, "texture", (Texture*) nullptr);
			m_texturesByClothingType[HEAD_SLOT].push_back(hairTexture);
		}
	}
}

void ClothingSetDefinition::ParseHats(tinyxml2::XMLElement * setElement)
{
	tinyxml2::XMLElement* allHatElements = setElement->FirstChildElement("Hats");
	if (allHatElements != nullptr){
		//parse all body elements
		for(tinyxml2::XMLElement* hatElement = allHatElements->FirstChildElement("Hat"); hatElement != nullptr; hatElement = hatElement->NextSiblingElement("Hat")){
			Texture* hatTexture = ParseXmlAttribute(*hatElement, "texture", (Texture*) nullptr);
			m_texturesByClothingType[HAT_SLOT].push_back(hatTexture);
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
		std::string baseTexName = ParseXmlAttribute(*baseElement, "texture", "NO_BASE");
		baseTexture = Texture::CreateOrGetTexture(baseTexName);
	} else {
		ERROR_AND_DIE("Must specify texture for body base!");
	}
	
	if (earsElement != nullptr){
		std::string earTexName = ParseXmlAttribute(*earsElement, "texture", "NO_EARS");
		earTexture = Texture::CreateOrGetTexture(earTexName);
	} else {
		earTexture = nullptr;
	}
	m_texturesByClothingType[BODY_SLOT].push_back(baseTexture);
	m_texturesByClothingType[EARS_SLOT].push_back(earTexture);
}
