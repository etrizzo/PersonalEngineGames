#include "Game/Dialogue.hpp"



Dialogue::Dialogue(std::string content)
{
	m_content = content;
}

void Dialogue::Render(const AABB2& box)
{
	g_theRenderer->DrawAABB2(box, RGBA::BLACK.GetColorWithAlpha(200));
	g_theRenderer->DrawAABB2Outline(box, RGBA::WHITE);

	AABB2 textBox = box.GetPercentageBox(.01f,.01f, .99f,.99f);
	g_theRenderer->DrawTextInBox2D(m_content, textBox, Vector2(0.f, 1.f), textBox.GetHeight() * .3f, TEXT_DRAW_WORD_WRAP);
}
