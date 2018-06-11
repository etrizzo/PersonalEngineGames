#include "EntityDefinition.hpp"
#include "Engine/Renderer/SpriteAnimSetDef.hpp"
#include "Game/ItemDefinition.hpp"

EntityDefinition::EntityDefinition(tinyxml2::XMLElement * entityDefElement)
{
	//m_drawingRadius			= .5f;
	//m_physicsRadius			= .35f;
	//m_aspectRatio			= 1.f;
	//m_boundingBox			= AABB2(0.f,0.f,0.f,0.f);
	//m_drawWithBounds		= false;
	//m_pivot					= Vector2::HALF;


	m_name = ParseXmlAttribute(*entityDefElement, "name", m_name);
	tinyxml2::XMLElement* sizeElement = entityDefElement->FirstChildElement("Size");
	tinyxml2::XMLElement* movementElement = entityDefElement->FirstChildElement("Movement");
	tinyxml2::XMLElement* healthElement = entityDefElement->FirstChildElement("Health");
	tinyxml2::XMLElement* animSetElement = entityDefElement->FirstChildElement("SpriteAnimSet");
	tinyxml2::XMLElement* inventoryElement = entityDefElement->FirstChildElement("Inventory");
	ParseSize(sizeElement);
	ParseMovement(movementElement);
	ParseHealth(healthElement);
	ParseStartingInventory(inventoryElement);
	m_spriteAnimSetDef = new SpriteAnimSetDef(animSetElement, *g_theRenderer);		//why is this by reference?
}

EntityDefinition::~EntityDefinition()
{
	delete m_spriteAnimSetDef;
}

void EntityDefinition::ParseMovement(tinyxml2::XMLElement* movementElement)
{
	if (movementElement != nullptr){
		tinyxml2::XMLElement* walkElement = movementElement->FirstChildElement("Walk");
		tinyxml2::XMLElement* swimElement = movementElement->FirstChildElement("Swim");
		tinyxml2::XMLElement* flyElement = movementElement->FirstChildElement("Fly");
		tinyxml2::XMLElement* sightElement = movementElement->FirstChildElement("Sight");
		if (walkElement != nullptr){
			m_canWalk = true;
			m_walkSpeed = ParseXmlAttribute(*walkElement, "speed", 1.f);
		}
		if (swimElement != nullptr){
			m_canSwim = true;
			m_swimSpeed = ParseXmlAttribute(*swimElement, "speed", 1.f);
		}
		if (flyElement != nullptr){
			m_canFly = true;
			m_flySpeed = ParseXmlAttribute(*flyElement, "speed", 1.f);
		}
		if (sightElement != nullptr){
			m_spinDegreesPerSecond = ParseXmlAttribute(*sightElement, "rotateSpeed", 180.f);
			m_forwardViewAngle = ParseXmlAttribute(*sightElement, "viewAngle", 90.f);
			m_range = ParseXmlAttribute(*sightElement, "range", 5.f);
		}
	}
}

void EntityDefinition::ParseSize(tinyxml2::XMLElement* sizeElement)
{
	if (sizeElement != nullptr){
		m_physicsRadius = ParseXmlAttribute(*sizeElement, "physicsRadius", m_physicsRadius);
		m_drawingRadius = ParseXmlAttribute(*sizeElement, "drawRadius", m_drawingRadius);	//width
		m_aspectRatio	= ParseXmlAttribute(*sizeElement, "aspect", 1.f);
		m_pivot			= ParseXmlAttribute(*sizeElement, "pivot", m_pivot);


		//std::string bounds = ParseXmlAttribute(*sizeElement, "drawBounds", " ");
		//if (bounds != " "){
		//	m_drawWithBounds = true;
		//	//m_boundingBox.SetFromText(bounds.c_str());
		//}
	} 

	//draw radius is width
	float height = m_drawingRadius / m_aspectRatio;
	m_boundingBox.mins = Vector2(-m_drawingRadius * m_pivot.x, -height * m_pivot.y);
	m_boundingBox.maxs = Vector2(m_drawingRadius * (1.f - m_pivot.x), height * (1.f - m_pivot.y));
	m_drawWithBounds = true;
}

void EntityDefinition::ParseHealth(tinyxml2::XMLElement * healthElement)
{
	if (healthElement!= nullptr){
		m_maxHealth = ParseXmlAttribute(*healthElement, "max", 1);
		m_startingHealth = ParseXmlAttribute(*healthElement, "start", 1);
	}
}

void EntityDefinition::ParseStartingInventory(tinyxml2::XMLElement * inventoryElement)
{
	if (inventoryElement != nullptr){
		m_startingInventory = std::vector<ItemDefinition*>();
		m_inventoryProbabilities = std::vector<float>();
		for (tinyxml2::XMLElement* itemElement = inventoryElement->FirstChildElement("Item"); itemElement != NULL; itemElement = itemElement->NextSiblingElement("Item")){
			std::string itemName = ParseXmlAttribute(*itemElement, "name", "NONAME");
			float chanceToSpawn = ParseXmlAttribute(*itemElement, "chanceToSpawn", 1.f);
			ItemDefinition* itemToSpawn = ItemDefinition::GetItemDefinition(itemName);
			m_startingInventory.push_back(itemToSpawn);
			m_inventoryProbabilities.push_back(chanceToSpawn);
		}
	}
}
