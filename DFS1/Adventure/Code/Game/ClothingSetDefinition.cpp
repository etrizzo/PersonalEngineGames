#include "ClothingSetDefinition.hpp"
#include "ClothingSet.hpp"

std::map<std::string, ClothingSetDefinition*>	ClothingSetDefinition::s_definitions;

SpawnColorCB GetColorCallbackFromXML(std::string text)
{
	if (text == "randomBright"){
		return RGBA::GetRandomRainbowColor;
	}
	if (text == "random"){
		return RGBA::GetRandomColor;
	}
	if (text == "randomMuted"){
		return RGBA::GetRandomMutedColor;
	}
	if (text == "randomLip"){
		return GetRandomLipColor;
	}
	if (text == "randomEye"){
		return GetRandomEyeColor;
	}
	return nullptr;
}


ClothingSetDefinition::ClothingSetDefinition(tinyxml2::XMLElement * setElement)
{
	m_name = ParseXmlAttribute(*setElement, "name", "NO_NAME");
	m_equipmentIndex = ParseXmlAttribute(*setElement, "equipmentIndex", m_equipmentIndex);	//what equipment sprite this set should use (f or m)
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
	ParseWeapons(setElement);

	tinyxml2::XMLElement* portraitElement = setElement->FirstChildElement("Portrait");
	if (portraitElement != nullptr){
		m_portraitDef = new PortraitDefinition(portraitElement);
	}
}

ClothingSet * ClothingSetDefinition::GetSetAtEquipmentIndex(int equipIndex) const
{
	ClothingSet* set = new ClothingSet();
	if ((int)  m_layersByClothingType[CHEST_SLOT].size() > 0){
		//load chest clothing
		int torsoIndex = equipIndex;
		ASSERT_OR_DIE(torsoIndex < m_layersByClothingType[CHEST_SLOT].size(), "Asked for an equipment index that doesn't exist.");
		set->InitLayer(CHEST_SLOT, GetLayer(CHEST_SLOT, torsoIndex));
		//if that chest clothing should render pants, get random pants
		if ( GetLayer(CHEST_SLOT, torsoIndex)->ShouldRenderLegs()){
			int legIndex = equipIndex;
			ASSERT_OR_DIE(legIndex < m_layersByClothingType[LEGS_SLOT].size(), "Asked for an equipment index that doesn't exist.");
			set->InitLayer(LEGS_SLOT,		GetLayer(LEGS_SLOT, legIndex));		//init body and ears of same skin tone
		}
	}
	if ((int)  m_layersByClothingType[BODY_SLOT].size() > 0){
		//load body
		int bodyIndex = equipIndex;
		ASSERT_OR_DIE(bodyIndex < m_layersByClothingType[BODY_SLOT].size(), "Asked for an equipment index that doesn't exist.");
		set->InitLayer(BODY_SLOT,		GetLayer(BODY_SLOT, bodyIndex));		//init body and ears of same skin tone
		set->InitLayer(EARS_SLOT,		GetLayer(EARS_SLOT, bodyIndex));
		//set->InitPortraitBody();

	}
	if (m_layersByClothingType[HEAD_SLOT].size() > 0){
		int hairIndex = GetRandomIntLessThan((int) m_layersByClothingType[HEAD_SLOT].size());
		set->InitLayer(HEAD_SLOT,		GetLayer(HEAD_SLOT, hairIndex));
		//set->InitPortraitHair();
	}

	if ((int)  m_layersByClothingType[HAT_SLOT].size() > 0){
		//load body
		int bodyIndex = equipIndex;
		ASSERT_OR_DIE(bodyIndex < m_layersByClothingType[HAT_SLOT].size(), "Asked for an equipment index that doesn't exist.");
		set->InitLayer(HAT_SLOT,		GetLayer(HAT_SLOT, bodyIndex));		//init body and ears of same skin tone

	}
	if ((int)  m_layersByClothingType[WEAPON_SLOT].size() > 0){
		//load body
		int bodyIndex = equipIndex;
		ASSERT_OR_DIE(bodyIndex < m_layersByClothingType[WEAPON_SLOT].size(), "Asked for an equipment index that doesn't exist.");
		set->InitLayer(WEAPON_SLOT,		GetLayer(WEAPON_SLOT, bodyIndex));		//init body and ears of same skin tone

	}

	//AFTER all the layers for the set have been determined, make the portrait layers.
	if (m_portraitDef != nullptr){
		set->m_portraitDef = m_portraitDef;
		set->InitPortrait();
	}
	set->m_equipmentIndex = m_equipmentIndex;
	return set;
}

ClothingSet * ClothingSetDefinition::GetRandomSet() const
{
	ClothingSet* set = new ClothingSet();
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
		//set->InitPortraitBody();

	}
	if (m_layersByClothingType[HEAD_SLOT].size() > 0){
		int hairIndex = GetRandomIntLessThan((int) m_layersByClothingType[HEAD_SLOT].size());
		set->InitLayer(HEAD_SLOT,		GetLayer(HEAD_SLOT, hairIndex));
		//set->InitPortraitHair();
	}
	set->InitLayer(HAT_SLOT,		GetRandomOfType(HAT_SLOT));
	set->InitLayer(WEAPON_SLOT,		GetRandomOfType(WEAPON_SLOT));
	
	//AFTER all the layers for the set have been determined, make the portrait layers.
	if (m_portraitDef != nullptr){
		set->m_portraitDef = m_portraitDef;
		set->InitPortrait();
	}
	set->m_equipmentIndex = m_equipmentIndex;
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
			//get callback function for tint of this layer
			SpawnColorCB cb = GetColorCallbackFromXML(colorType);
			RGBA tint = RGBA::WHITE;
			if (cb != nullptr){
				tint = cb();
			}
			bool usesLegs = ParseXmlAttribute(*torsoElement, "useLegs", true);
			bool usesHair = ParseXmlAttribute(*torsoElement, "usesHair", true);

			ClothingLayer* layer = new ClothingLayer(CHEST_SLOT, torsoTexture, tint, usesLegs, usesHair, cb);
			
			int index = ParseXmlAttribute(*torsoElement, "index", -1);
			if (index >= 0){
				int size = m_layersByClothingType[CHEST_SLOT].size();
				//insert at the right spot
				if (index == size){
					m_layersByClothingType[CHEST_SLOT].push_back(layer);
				} else if (index < size){
					m_layersByClothingType[CHEST_SLOT][index] = layer;
				} else {
					m_layersByClothingType[CHEST_SLOT].resize(index);
					m_layersByClothingType[CHEST_SLOT][index] = layer;
				}
			} else {
				//just push back.
				m_layersByClothingType[CHEST_SLOT].push_back(layer);
			}
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
			//get callback function for tint of this layer
			SpawnColorCB cb = GetColorCallbackFromXML(colorType);
			RGBA tint = RGBA::WHITE;
			if (cb != nullptr){
				tint = cb();
			}
			bool usesLegs = ParseXmlAttribute(*legElement, "useLegs", true);
			bool usesHair = ParseXmlAttribute(*legElement, "usesHair", true);

			ClothingLayer* layer = new ClothingLayer(LEGS_SLOT, torsoTexture, tint, usesLegs, usesHair, cb);
			int index = ParseXmlAttribute(*legElement, "index", -1);
			if (index >= 0){
				int size = m_layersByClothingType[LEGS_SLOT].size();
				//insert at the right spot
				if (index == size){
					m_layersByClothingType[LEGS_SLOT].push_back(layer);
				} else if (index < size){
					m_layersByClothingType[LEGS_SLOT][index] = layer;
				} else {
					m_layersByClothingType[LEGS_SLOT].resize(index);
					m_layersByClothingType[LEGS_SLOT][index] = layer;
				}
			} else {
				//just push back.
				m_layersByClothingType[LEGS_SLOT].push_back(layer);
			}
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
			//get callback function for tint of this layer
			SpawnColorCB cb = GetColorCallbackFromXML(colorType);
			RGBA tint = RGBA::WHITE ;
			if (cb != nullptr){
				tint = cb();
			}
			bool usesLegs = ParseXmlAttribute(*hairElement, "useLegs", true);
			bool usesHair = ParseXmlAttribute(*hairElement, "usesHair", true);

			ClothingLayer* layer = new ClothingLayer(HEAD_SLOT, torsoTexture, tint, usesLegs, usesHair, cb);
			layer->m_portraitCoords = ParseXmlAttribute(*hairElement, "portraitCoords", IntVector2(0,0));
			if (cb != nullptr){
				layer->m_portraitTint = tint;
			} else {
				layer->m_portraitTint = ParseXmlAttribute(*hairElement, "portraitTint", RGBA::WHITE);
			
			}
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
			Texture* hatTexture = ParseXmlAttribute(*hatElement, "texture", (Texture*) nullptr);
			std::string colorType = ParseXmlAttribute(*hatElement, "color", "NONE");
			//get callback function for tint of this layer
			SpawnColorCB cb = GetColorCallbackFromXML(colorType);
			RGBA tint = RGBA::WHITE;
			if (cb != nullptr){
				tint = cb();
			}
			bool usesLegs = ParseXmlAttribute(*hatElement, "useLegs", true);
			bool usesHair = ParseXmlAttribute(*hatElement, "usesHair", true);

			ClothingLayer* layer = new ClothingLayer(HAT_SLOT, hatTexture, tint, usesLegs, usesHair);
			int index = ParseXmlAttribute(*hatElement, "index", -1);
			if (index >= 0){
				int size = m_layersByClothingType[HAT_SLOT].size();
				//insert at the right spot
				if (index == size){
					m_layersByClothingType[HAT_SLOT].push_back(layer);
				} else if (index < size){
					m_layersByClothingType[HAT_SLOT][index] = layer;
				} else {
					m_layersByClothingType[HAT_SLOT].resize(index);
					m_layersByClothingType[HAT_SLOT][index] = layer;
				}
			} else {
				//just push back.
				m_layersByClothingType[HAT_SLOT].push_back(layer);
			}
		}
	}
}

void ClothingSetDefinition::ParseWeapons(tinyxml2::XMLElement * setElement)
{
	tinyxml2::XMLElement* allWeapons = setElement->FirstChildElement("Weapons");
	if (allWeapons != nullptr){
		//parse all body elements
		for(tinyxml2::XMLElement* weaponElement = allWeapons->FirstChildElement("Weapon"); weaponElement != nullptr; weaponElement = weaponElement->NextSiblingElement("Weapon")){
			Texture* texture = ParseXmlAttribute(*weaponElement, "texture", (Texture*) nullptr);
			std::string colorType = ParseXmlAttribute(*weaponElement, "color", "NONE");
			//get callback function for tint of this layer
			SpawnColorCB cb = GetColorCallbackFromXML(colorType);
			RGBA tint = RGBA::WHITE;
			if (cb != nullptr){
				tint = cb();
			}
			bool usesLegs = ParseXmlAttribute(*weaponElement, "useLegs", true);
			bool usesHair = ParseXmlAttribute(*weaponElement, "usesHair", true);

			ClothingLayer* layer = new ClothingLayer(WEAPON_SLOT, texture, tint, usesLegs, usesHair);
			m_layersByClothingType[WEAPON_SLOT].push_back(layer);
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
	//get callback function for tint of this layer
	SpawnColorCB cb = GetColorCallbackFromXML(colorType);
	RGBA tint = RGBA::WHITE;
	if (cb != nullptr){
		tint = cb();
	}
	bool usesLegs = ParseXmlAttribute(*baseElement, "useLegs", true);
	bool usesHair = ParseXmlAttribute(*baseElement, "usesHair", true);

	ClothingLayer* bodyLayer = new ClothingLayer(BODY_SLOT, baseTexture, tint, usesLegs, usesHair);
	ClothingLayer* earLayer = new ClothingLayer(EARS_SLOT, earTexture, tint, usesLegs, usesHair);

	//parse relevant portrait information
	bodyLayer->m_portraitTint = ParseXmlAttribute(*baseElement, "portraitTint", RGBA::CYAN);
	earLayer->m_portraitCoords = IntVector2(7,8);
	if (earsElement != nullptr){
		earLayer->m_portraitCoords = ParseXmlAttribute(*earsElement, "portraitCoords", earLayer->m_portraitCoords);
		earLayer->m_portraitTint = ParseXmlAttribute(*baseElement, "portraitTint", RGBA::CYAN);
	} 

	m_layersByClothingType[BODY_SLOT].push_back(bodyLayer);
	m_layersByClothingType[EARS_SLOT].push_back(earLayer);

	//m_texturesByClothingType[BODY_SLOT].push_back(baseTexture);
	//m_texturesByClothingType[EARS_SLOT].push_back(earTexture);

	//std::string colorType = ParseXmlAttribute(*bodyElement, "color", "NONE");
	//RGBA tint = GetColorFromXML(colorType);
	//m_tintsByClothingType[BODY_SLOT].push_back(tint);
	//m_tintsByClothingType[EARS_SLOT].push_back(tint);

}

RGBA sPortraitPieceDefinition::GetColorForInstance() const
{
	if (m_colorCallback == nullptr){
		return m_defaultTint;
	} else {
		return m_colorCallback();
	}
}

PortraitDefinition::PortraitDefinition(tinyxml2::XMLElement * portraitElement)
{
	m_lipColorChance = ParseXmlAttribute(*portraitElement, "lipColorChance", m_lipColorChance);
	m_numFeaturesPossible = ParseXmlAttribute(*portraitElement, "numFeatures", m_numFeaturesPossible);
	ParseFaces(portraitElement);
	ParseMouths(portraitElement);
	ParseNoses(portraitElement);
	ParseBrows(portraitElement);
	ParseEyes(portraitElement);
	ParseFeatures(portraitElement);
}

IntVector2 PortraitDefinition::GetRandomFaceCoords() const
{
	int index = GetRandomIntLessThan(m_faces.size());
	return m_faces[index]->m_spriteCoords;
}

IntVector2 PortraitDefinition::GetRandomNoseCoords() const
{
	int index = GetRandomIntLessThan(m_noses.size());
	return m_noses[index]->m_spriteCoords;
}

IntVector2 PortraitDefinition::GetRandomBrowCoords() const
{
	int index = GetRandomIntLessThan(m_brows.size());
	return m_brows[index]->m_spriteCoords;
}

IntVector2 PortraitDefinition::GetRandomFeatureCoords() const
{
	int index = GetRandomIntLessThan(m_features.size());
	return m_features[index]->m_spriteCoords;
}


void PortraitDefinition::ParseFaces(tinyxml2::XMLElement * portraitElement)
{
	tinyxml2::XMLElement* faceElement = portraitElement->FirstChildElement("Face");
	while (faceElement != nullptr){
		sPortraitPieceDefinition* facePiece = new sPortraitPieceDefinition();
		facePiece->m_name = ParseXmlAttribute(*faceElement, "name", "NO_NAME");
		facePiece->m_defaultTint = ParseXmlAttribute(*faceElement, "tint", RGBA::WHITE);
		facePiece->m_spriteCoords = ParseXmlAttribute(*faceElement, "portraitCoords", IntVector2(0,0));
		std::string callbackName = ParseXmlAttribute(*faceElement, "color", "");
		facePiece->m_colorCallback = GetColorCallbackFromXML(callbackName);
		m_faces.push_back(facePiece);
		faceElement = faceElement->NextSiblingElement("Face");
	}
}

void PortraitDefinition::ParseMouths(tinyxml2::XMLElement * portraitElement)
{
	tinyxml2::XMLElement* mouthElement = portraitElement->FirstChildElement("Mouth");
	while (mouthElement != nullptr){
		//read info for the mouth lines coordinates
		sPortraitPieceDefinition* linePiece = new sPortraitPieceDefinition();
		linePiece->m_name = ParseXmlAttribute(*mouthElement, "name", "NO_NAME");
		linePiece->m_defaultTint = ParseXmlAttribute(*mouthElement, "tint", RGBA::WHITE);
		linePiece->m_spriteCoords = ParseXmlAttribute(*mouthElement, "portraitCoords", IntVector2(0,0));
		m_mouthLines.push_back(linePiece);

		//read info for the lip piece (as being 1 below the mouth lines);
		sPortraitPieceDefinition* lipPiece = new sPortraitPieceDefinition();
		lipPiece->m_name = linePiece->m_name;
		lipPiece->m_defaultTint = linePiece->m_defaultTint;
		lipPiece->m_spriteCoords = linePiece->m_spriteCoords + IntVector2(0,1);
		std::string callbackName = ParseXmlAttribute(*mouthElement, "color", "");
		lipPiece->m_colorCallback = GetColorCallbackFromXML(callbackName);
		m_lipMasks.push_back(lipPiece);

		mouthElement = mouthElement->NextSiblingElement("Mouth");
	}
}

void PortraitDefinition::ParseNoses(tinyxml2::XMLElement * portraitElement)
{
	tinyxml2::XMLElement* faceElement = portraitElement->FirstChildElement("Nose");
	while (faceElement != nullptr){
		sPortraitPieceDefinition* facePiece = new sPortraitPieceDefinition();
		facePiece->m_name = ParseXmlAttribute(*faceElement, "name", "NO_NAME");
		facePiece->m_defaultTint = ParseXmlAttribute(*faceElement, "tint", RGBA::WHITE);
		facePiece->m_spriteCoords = ParseXmlAttribute(*faceElement, "portraitCoords", IntVector2(0,0));
		std::string callbackName = ParseXmlAttribute(*faceElement, "color", "");
		facePiece->m_colorCallback = GetColorCallbackFromXML(callbackName);
		m_noses.push_back(facePiece);
		faceElement = faceElement->NextSiblingElement("Nose");
	}
}

void PortraitDefinition::ParseBrows(tinyxml2::XMLElement * portraitElement)
{
	tinyxml2::XMLElement* faceElement = portraitElement->FirstChildElement("Brow");
	while (faceElement != nullptr){
		sPortraitPieceDefinition* facePiece = new sPortraitPieceDefinition();
		facePiece->m_name = ParseXmlAttribute(*faceElement, "name", "NO_NAME");
		facePiece->m_defaultTint = ParseXmlAttribute(*faceElement, "tint", RGBA::WHITE);
		facePiece->m_spriteCoords = ParseXmlAttribute(*faceElement, "portraitCoords", IntVector2(0,0));
		std::string callbackName = ParseXmlAttribute(*faceElement, "color", "");
		facePiece->m_colorCallback = GetColorCallbackFromXML(callbackName);
		m_brows.push_back(facePiece);
		faceElement = faceElement->NextSiblingElement("Brow");
	}
}

void PortraitDefinition::ParseEyes(tinyxml2::XMLElement * portraitElement)
{
	tinyxml2::XMLElement* mouthElement = portraitElement->FirstChildElement("Eyes");
	while (mouthElement != nullptr){
		//read info for the eye lines coordinates
		sPortraitPieceDefinition* linePiece = new sPortraitPieceDefinition();
		linePiece->m_name = ParseXmlAttribute(*mouthElement, "name", "NO_NAME");
		linePiece->m_defaultTint = ParseXmlAttribute(*mouthElement, "tint", RGBA::WHITE);
		linePiece->m_spriteCoords = ParseXmlAttribute(*mouthElement, "portraitCoords", IntVector2(0,0));
		m_eyeWhites.push_back(linePiece);

		//read info for the pupil piece (as being 1 to the right the eye lines);
		sPortraitPieceDefinition* pupilPiece = new sPortraitPieceDefinition();
		pupilPiece->m_name = linePiece->m_name;
		pupilPiece->m_defaultTint = linePiece->m_defaultTint;
		pupilPiece->m_spriteCoords = linePiece->m_spriteCoords + IntVector2(1,0);
		std::string callbackName = ParseXmlAttribute(*mouthElement, "color", "");
		pupilPiece->m_colorCallback = GetColorCallbackFromXML(callbackName);
		m_eyePupils.push_back(pupilPiece);

		mouthElement = mouthElement->NextSiblingElement("Eyes");
	}
}

void PortraitDefinition::ParseFeatures(tinyxml2::XMLElement * portraitElement)
{
	tinyxml2::XMLElement* faceElement = portraitElement->FirstChildElement("Feature");
	while (faceElement != nullptr){
		sPortraitPieceDefinition* facePiece = new sPortraitPieceDefinition();
		facePiece->m_name = ParseXmlAttribute(*faceElement, "name", "NO_NAME");
		facePiece->m_defaultTint = ParseXmlAttribute(*faceElement, "tint", RGBA::WHITE);
		facePiece->m_spriteCoords = ParseXmlAttribute(*faceElement, "portraitCoords", IntVector2(0,0));
		std::string callbackName = ParseXmlAttribute(*faceElement, "color", "");
		facePiece->m_colorCallback = GetColorCallbackFromXML(callbackName);
		m_features.push_back(facePiece);
		faceElement = faceElement->NextSiblingElement("Feature");
	}
}
