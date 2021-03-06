#include "Game/ActorDefinition.hpp"
#include "Game/Stats.hpp"
#include "Game/ItemDefinition.hpp"
// Sample XML: 

//	<ActorDefinition name="Player" faction="good">
//		<Size physicsRadius="0.25" drawRadius =".3"/>
//		<Movement>
//			<Walk speed="1.0"/>
//			<Swim speed=".5"/>
//			<Sight rotateSpeed="180" viewAngle="90" range="10" />
//		</Movement>
//		<Stats baseSpeed="2" baseStrength="1~2" baseDefense="0" />
//		<Health max="100" start="100"/>
//		<Inventory>
//			<Item name="Bow" chanceToSpawn="1.0"/>
//		</Inventory>
//		<SpriteAnimSet spriteSheet="Kushnariova_12x53.png" spriteLayout="12,53" fps="10">
//			<SpriteAnim name="MoveEast"		spriteIndexes="3,4,5,4"/>
//			<SpriteAnim name="MoveWest"		spriteIndexes="9,10,11,10"/>
//			<SpriteAnim name="MoveNorth"	spriteIndexes="0,1,2,1"/>
//			<SpriteAnim name="MoveSouth"	spriteIndexes="6,7,8,7"/>
//			<SpriteAnim name="Idle"		spriteIndexes="6,7,8,7" fps="3"/>
//		</SpriteAnimSet>
//	</ActorDefinition>

std::map<std::string, ActorDefinition*>	ActorDefinition::s_definitions;

ActorDefinition::ActorDefinition(tinyxml2::XMLElement * actorElement): EntityDefinition(actorElement)
{
	m_layerTextures = std::vector<Texture*>();
	m_layerTextures.resize(NUM_RENDER_SLOTS);
	m_startingFaction = ParseXmlAttribute(*actorElement, "faction", "evil");
	m_isAggressive = ParseXmlAttribute(*actorElement, "aggressive", false);

	m_nameSourceFile = ParseXmlAttribute(*actorElement, "namesFile", m_nameSourceFile);
	ReadActorNamesFromSourceFile();
	LoadUnusedActorNamesFromBaseAndShuffle();

	std::string behavior = ParseXmlAttribute(*actorElement, "defaultBehavior", "Wander");
	m_defaultBehavior = GetBehaviorFromString(behavior);

	ParseStats(actorElement->FirstChildElement("Stats"));			//maybe should be in entity? Will apply to actors, projectiles, and items, most likely
	ParseLayersElement(actorElement->FirstChildElement("Layers"));
	tinyxml2::XMLElement* clothingElement = actorElement->FirstChildElement("Clothing");
	while (clothingElement != nullptr){
		std::string setName = ParseXmlAttribute(*clothingElement, "set", "NONE");
		m_clothingSetDefs.push_back(ClothingSetDefinition::GetDefinition(setName));

		clothingElement = clothingElement->NextSiblingElement("Clothing");
	}
	/*if (clothingElement != nullptr){
		std::string setName = ParseXmlAttribute(*clothingElement, "set", "NONE");
		m_clothingSetDef = ClothingSetDefinition::GetDefinition(setName);
	} else {
		m_clothingSetDef = nullptr;
	}*/

	tinyxml2::XMLElement* dialogueElement = actorElement->FirstChildElement("DialogueSet");
	m_dialogueDefinitions = std::vector<DialogueSetDefinition*>();
	while(dialogueElement != nullptr){
		m_dialogueDefinitions.push_back(new DialogueSetDefinition(dialogueElement));
		dialogueElement = dialogueElement->NextSiblingElement("DialogueSet");
	}
}

ActorDefinition::~ActorDefinition()
{
}

DialogueSetDefinition * ActorDefinition::GetRandomDialogueDefinition() const
{
	if (m_dialogueDefinitions.size() == 0 ){
		return nullptr;
	}
	int index = GetRandomIntLessThan(m_dialogueDefinitions.size());
	return m_dialogueDefinitions[index];
}

std::string ActorDefinition::GetRandomActorNameAndCrossOff()
{
	if (m_unusedActorNames.size() == 0){
		LoadUnusedActorNamesFromBaseAndShuffle();
		if (m_unusedActorNames.size() == 0){
			return "NoName";
		}
	}
	std::string nameToUse = m_unusedActorNames.back();
	m_unusedActorNames.pop_back();
	return nameToUse;
}

ActorDefinition * ActorDefinition::GetActorDefinition(std::string definitionName)
{
	auto containsDef = s_definitions.find((std::string)definitionName); 
	ActorDefinition* actorDef = nullptr;
	if (containsDef != s_definitions.end()){
		actorDef = containsDef->second;
	} 
	return actorDef;
}

void ActorDefinition::ParseStats(tinyxml2::XMLElement * statsElement)
{
	m_minStats = Stats();
	m_maxStats = Stats();

	if (statsElement != nullptr){
		IntRange movementRange	= ParseXmlAttribute(*statsElement, "baseMovement", IntRange(0));
		IntRange strengthRange	= ParseXmlAttribute(*statsElement, "baseStrength", IntRange(0));
		IntRange defenseRange	= ParseXmlAttribute(*statsElement, "baseDefense", IntRange(0));
		m_minStats.SetStat(STAT_MOVEMENT, movementRange.min);
		m_maxStats.SetStat(STAT_MOVEMENT, movementRange.max);
		m_minStats.SetStat(STAT_STRENGTH, strengthRange.min);
		m_maxStats.SetStat(STAT_STRENGTH, strengthRange.max);
		m_minStats.SetStat(STAT_DEFENSE, defenseRange.min);
		m_maxStats.SetStat(STAT_DEFENSE, defenseRange.max);
	}
}

void ActorDefinition::ParseLayersElement(tinyxml2::XMLElement * layersElement)
{
	if (layersElement != nullptr){
		ParseLayer(layersElement->FirstChildElement("Body"), BODY_SLOT);
		ParseLayer(layersElement->FirstChildElement("Chest"), CHEST_SLOT);
		ParseLayer(layersElement->FirstChildElement("Legs"), LEGS_SLOT);
		ParseLayer(layersElement->FirstChildElement("Head"), HEAD_SLOT);
		ParseLayer(layersElement->FirstChildElement("Weapon"), WEAPON_SLOT);
	}
}

void ActorDefinition::ParseLayer(tinyxml2::XMLElement * layer, RENDER_SLOT slot)
{
	if (layer != nullptr){
		std::string name = ParseXmlAttribute(*layer, "name", "NO_TEXTURE");
		ASSERT_OR_DIE(name != "NO_TEXTURE", "Must specify layer texture");
		m_layerTextures[slot] = Texture::CreateOrGetTexture(name);
		//m_spriteSetDefs[slot]->SetSpriteSheetTexture(name);
	}
}

void ActorDefinition::ReadActorNamesFromSourceFile()
{
	FILE *fp = nullptr;
	fopen_s( &fp, m_nameSourceFile.c_str(), "r" );
	char lineCSTR [1000];
	std::string line;
	int MAX_LINE_LENGTH = 1000;

	ASSERT_OR_DIE(fp != nullptr, "NO VILLAGE NAME FILE FOUND");
	while (fgets( lineCSTR, MAX_LINE_LENGTH, fp ) != NULL)
	{
		line = "";
		line.append(lineCSTR);
		Strip(line, '\n');
		m_actorNames.push_back(line);
	}
}

void ActorDefinition::LoadUnusedActorNamesFromBaseAndShuffle()
{
	//copy base actor names into the vector of unused village names
	m_unusedActorNames = Strings(m_actorNames);

	//shuffle the list
	for (int i = 0; i < (int) m_unusedActorNames.size(); i++){
		int randomSwapIndex = GetRandomIntLessThan(m_unusedActorNames.size());
		std::swap(m_unusedActorNames[i], m_unusedActorNames[randomSwapIndex]);
	}
}


