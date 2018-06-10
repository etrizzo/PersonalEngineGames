#include "Item.hpp"
#include "ItemDefinition.hpp"
#include "Map.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"

Item::Item(ItemDefinition * definition, Map * entityMap, Vector2 initialPos, float initialRotation, std::string faction)
	:Entity((EntityDefinition*)definition, entityMap, initialPos, initialRotation)
{
	m_definition = definition;
	m_faction = faction;
	m_facing = Vector2::MakeDirectionAtDegrees(initialRotation);
	m_localDrawingBox = AABB2(-definition->m_drawingRadius, -definition->m_drawingRadius, definition->m_drawingRadius, definition->m_drawingRadius);
	GetRandomStatsFromDefinition();
}

void Item::Update(float deltaSeconds)
{
	Entity::Update(deltaSeconds);
}

void Item::Render()
{
	Entity::Render();
}

void Item::RenderInMenu(AABB2 boxToRenderIn, bool selected)
{
	float heightOfBox = boxToRenderIn.GetHeight();
	float textHeight = heightOfBox * .7f;
	AABB2 selectBox = AABB2(boxToRenderIn.mins, Vector2(boxToRenderIn.mins.x + heightOfBox, boxToRenderIn.maxs.y));			//square for select arrow
	selectBox.Translate(.05f, 0.f);
	AABB2 textBox = AABB2(selectBox.maxs.x, selectBox.mins.y, boxToRenderIn.maxs.x - (heightOfBox + .05f), boxToRenderIn.maxs.y);		//middle bit for text
	AABB2 equipBox = AABB2(textBox.maxs.x, textBox.mins.y, boxToRenderIn.maxs.x - .05f, boxToRenderIn.maxs.y);			//square for equipment icon
	
	RGBA outlineColor = RGBA(255,255,255,32);
	g_theRenderer->DrawAABB2Outline(boxToRenderIn, outlineColor);
	//g_theRenderer->DrawAABB2Outline(selectBox, outlineColor);
	//g_theRenderer->DrawAABB2Outline(textBox, outlineColor);
	//g_theRenderer->DrawAABB2Outline(equipBox, outlineColor);

	if (selected){
		g_theGame->RenderSelectArrow(selectBox);
	}
	g_theRenderer->DrawTextInBox2D(m_definition->m_name, textBox, Vector2(0.01f, .5f), textHeight, TEXT_DRAW_SHRINK_TO_FIT);
	RenderEquipmentIcon(equipBox);
}

void Item::RenderImageInBox(AABB2 boxToRenderIn)
{
	const Texture* entityTexture = m_animSet->GetCurrentTexture();
	AABB2 texCoords = m_animSet->GetCurrentUVs();
	g_theRenderer->DrawTexturedAABB2(boxToRenderIn, *entityTexture,texCoords.mins, texCoords.maxs, RGBA());
}

void Item::RenderStats(AABB2 boxToRenderIn)
{
	float widthOfBox = boxToRenderIn.GetWidth();
	float heightOfBox = boxToRenderIn.GetHeight();
	float fontSize = heightOfBox * .1f;
	Vector2 padding = Vector2(fontSize * .2f, fontSize * .2f);
	g_theRenderer->DrawTextInBox2D(m_definition->m_name, boxToRenderIn, Vector2(.5f, .97f),fontSize, TEXT_DRAW_SHRINK_TO_FIT);

	//AABB2 pictureBox = AABB2(boxToRenderIn.mins + padding, Vector2(boxToRenderIn.maxs.x - (widthOfBox*.5f), boxToRenderIn.maxs.y - fontSize) - padding);
	AABB2 pictureBox = boxToRenderIn.GetPercentageBox(.05f, .25f, .45f, .75f);
	pictureBox.TrimToSquare();
	//pictureBox.AddPaddingToSides(-.15f, -.15f);
	g_theRenderer->DrawAABB2Outline(pictureBox, RGBA(255,255,255,64));
	//pictureBox.AddPaddingToSides(-.15f, -.15f);
	RenderImageInBox(pictureBox);

	//AABB2 statsBox = pictureBox;
	AABB2 statsBox = boxToRenderIn.GetPercentageBox(.55f, .25f, .95f, .75f);
	//statsBox.Translate((widthOfBox * .5f) - .15f, 0.f);
	//statsBox.AddPaddingToSides(.2f,0.15f);
	//AABB2 statsBox = AABB2(Vector2(boxToRenderIn.mins.x + (widthOfBox*.5f), 0.f)+ padding, boxToRenderIn.maxs - padding);
	std::string statsString = "Bonus Stats \n\n  ";
	for ( int statIDNum = 0; statIDNum < NUM_STAT_IDS; statIDNum++){
		STAT_ID statID = (STAT_ID)statIDNum;
		statsString = statsString + Stats::GetNameForStatID(statID) + " : " + std::to_string(m_stats.GetStat(statID)) + "\n\n  ";
	}
	g_theRenderer->DrawTextInBox2D(statsString, statsBox, Vector2(0.f, 0.9f), fontSize * .6f, TEXT_DRAW_SHRINK_TO_FIT);
	//g_theRenderer->DrawAABB2Outline(statsBox, RGBA(255,255,255,64));

}

void Item::RunCorrectivePhysics()
{
}

void Item::RenderEquipmentIcon(AABB2 boxToRenderIn)
{
	std::string letterToRender = "";
	switch (m_definition->m_equipSlot){
	case EQUIP_SLOT_HEAD:
		letterToRender = "H";
		break;
	case EQUIP_SLOT_CHEST:
		letterToRender = "C";
		break;
	case EQUIP_SLOT_LEGS:
		letterToRender = "L";
		break;
	case EQUIP_SLOT_WEAPON:
		letterToRender = "W";
		break;
	default:
		letterToRender="";
		break;
	}
	if (letterToRender != ""){
		RGBA tint = RGBA::WHITE;
		if (!m_currentlyEquipped){
			tint = RGBA(255,255,255,100); 
		}
		g_theRenderer->DrawTextInBox2D(letterToRender, boxToRenderIn, Vector2(.5f,.5f), boxToRenderIn.GetHeight() * .6f, TEXT_DRAW_OVERRUN, tint);
	}
}

void Item::GetRandomStatsFromDefinition()
{
	m_stats = Stats();
	for (int i = 0; i < NUM_STAT_IDS; i++){
		STAT_ID statID = (STAT_ID) i;
		m_stats.SetStat(statID, GetRandomIntInRange(m_definition->m_minStats.GetStat(statID), m_definition->m_maxStats.GetStat(statID)));
	}
}
