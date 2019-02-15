#include "Game/Dialogue.hpp"



Dialogue::Dialogue(std::string content)
{
	m_content = content;
}

void Dialogue::Render(const AABB2& box, std::string speakerName)
{
	AABB2 textBox = box.GetPercentageBox(.03f,.03f, .97f,.97f);
	float textHeight = textBox.GetHeight() * .13f;

	AABB2 nameBox ;
	AABB2 contentBox ;
	textBox.SplitAABB2Horizontal(.8f, nameBox, contentBox);
	float nameHeight = nameBox.GetHeight() * .75f;
	float nameLength = speakerName.size() * nameHeight * 1.1f;
	//draw the name
	nameBox.maxs.x = nameBox.mins.x + nameLength;	//shrink horizontally
	g_theRenderer->DrawAABB2(nameBox, RGBA::NICEBLACK.GetColorWithAlpha(200));
	g_theRenderer->DrawAABB2Outline(nameBox, RGBA::WHITE);
	float namePadding = nameBox.GetHeight() * -.1f;
	nameBox.AddPaddingToSides(namePadding,namePadding);
	g_theRenderer->DrawTextInBox2D(speakerName, nameBox, Vector2::HALF, nameHeight, TEXT_DRAW_SHRINK_TO_FIT);

	contentBox.AddPaddingToSides(0.f, -contentBox.GetHeight() * .05f);
	g_theRenderer->DrawTextInBox2D(m_content, contentBox, Vector2(0.f, 1.f), textHeight, TEXT_DRAW_WORD_WRAP);
}
