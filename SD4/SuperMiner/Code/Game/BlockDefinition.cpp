#include "BlockDefinition.hpp"

std::vector<BlockDefinition*> BlockDefinition::s_blockDefinitions = std::vector<BlockDefinition*>();

BlockDefinition::BlockDefinition(tinyxml2::XMLElement * blockElement)
{
	m_name = ParseXmlAttribute(*blockElement, "name", "NO_NAME");
	m_blockID = ParseXmlAttribute(*blockElement, "id", 0);
	//error checking that we have a block ID and that it's not a duplicate.
	std::string errorNoID = Stringf("%s must specify a block ID in data!", m_name.c_str());
	ASSERT_OR_DIE(m_blockID != 0, errorNoID.c_str());
	BlockDefinition* def = BlockDefinition::GetBlockDefinitionFromID(m_blockID);
	if (def != nullptr)
	{
		std::string errorDuplicate = Stringf("%s was already defined with id %i before %s with the same ID! Each block must have a unique ID!", def->m_name, def->m_blockID, m_name);
		ASSERT_OR_DIE(def == nullptr, errorDuplicate.c_str());
	}

	//continue parsing if we're gucci
	//parse spritecoords
	IntVector2 topSpriteCoords		= ParseXmlAttribute(*blockElement, "topSpriteCoords", IntVector2(1, 2));
	IntVector2 sideSpriteCoords		= ParseXmlAttribute(*blockElement, "sideSpriteCoords", topSpriteCoords);
	IntVector2 bottomSpriteCoords	= ParseXmlAttribute(*blockElement, "bottomSpriteCoords", topSpriteCoords);
	m_topUVs = g_blockSpriteSheet->GetTexCoordsForSpriteCoords(topSpriteCoords);
	m_sideUVs = g_blockSpriteSheet->GetTexCoordsForSpriteCoords(sideSpriteCoords);
	m_bottomUVs = g_blockSpriteSheet->GetTexCoordsForSpriteCoords(bottomSpriteCoords);

	//parse block attributes
	m_isFullyOpaque = ParseXmlAttribute(*blockElement, "isFullyOpaque", true);
	m_internalLightLevel = ParseXmlAttribute(*blockElement, "lightLevel", 0);
	

	//register yourself
	s_blockDefinitions.push_back(this);
}

BlockDefinition::BlockDefinition(uchar ID, bool isFullyOpaque, AABB2 topUV, AABB2 sideUV, AABB2 botUVs)
{
	m_blockID = ID;
	m_isFullyOpaque = isFullyOpaque;
	m_topUVs = topUV;
	m_sideUVs = sideUV;
	m_bottomUVs = botUVs;

	s_blockDefinitions.push_back(this);
}

BlockDefinition * BlockDefinition::GetBlockDefinitionFromID(unsigned char blockID)
{
	for (BlockDefinition* definition : BlockDefinition::s_blockDefinitions)
	{
		if (definition->m_blockID == blockID){
			return definition;
		}
	}
	return nullptr;
}

uchar BlockDefinition::GetBlockIDFromName(const std::string & name)
{
	for (BlockDefinition* definition : BlockDefinition::s_blockDefinitions)
	{
		if (definition->m_name == name) {
			return definition->m_blockID;
		}
	}
	ConsolePrintf(RGBA::RED, "No block definition called: %s. Returning air.", name.c_str());
	return BLOCK_AIR;
}
