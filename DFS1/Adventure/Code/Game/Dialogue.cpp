#include "Game/Dialogue.hpp"



Dialogue::Dialogue(std::string content)
{
	m_content = content;
}

void Dialogue::Render(const AABB2& box)
{
	

	AABB2 textBox = box.GetPercentageBox(.03f,.03f, .97f,.97f);
	g_theRenderer->DrawTextInBox2D(m_content, textBox, Vector2(0.f, 1.f), textBox.GetHeight() * .2f, TEXT_DRAW_WORD_WRAP);
}
