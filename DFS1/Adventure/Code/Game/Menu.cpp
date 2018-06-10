#include "Game/Menu.hpp"
#include "Game/Game.hpp"

MenuOption::MenuOption(std::string optText, int optValue, bool isActive)
{
	m_text = optText;
	m_value = optValue;
	m_isActive = isActive;

}

void Menu::HandleInput()
{
	if (WasSelectJustPressed()){
		m_selectedOption = true;
		
	}
	if (WasDownJustPressed()){
		MoveSelection(1);
	}
	if (WasUpJustPressed()){
		MoveSelection(-1);
	}

	
}

void Menu::Render()
{
	float textHeight = m_menuBox.GetHeight() / m_options.size();
	AABB2 bgBox = m_menuBox;
	float padding = m_menuBox.GetWidth() * .05f;
	bgBox.AddPaddingToSides(padding, padding);
	g_theRenderer->DrawAABB2(bgBox, RGBA(0,64,64,128));
	g_theRenderer->DrawAABB2Outline(bgBox, RGBA::WHITE);

	//render options
	AABB2 textBox = AABB2(m_menuBox.mins.x, m_menuBox.maxs.y - textHeight, m_menuBox.maxs.x, m_menuBox.maxs.y);
	for (int i = 0; i < (int) m_options.size(); i++){
		MenuOption option = m_options[i];
		if (i == m_currentIndex){
			g_theRenderer->DrawTextInBox2D(option.m_text, textBox, Vector2::HALF, textHeight, TEXT_DRAW_SHRINK_TO_FIT, RGBA::GREEN);
		} else if (!option.m_isActive){
			g_theRenderer->DrawTextInBox2D(option.m_text, textBox, Vector2::HALF, textHeight, TEXT_DRAW_SHRINK_TO_FIT, RGBA::WHITE.GetColorWithAlpha(128));
		} else {
			g_theRenderer->DrawTextInBox2D(option.m_text, textBox, Vector2::HALF, textHeight, TEXT_DRAW_SHRINK_TO_FIT, RGBA::WHITE);
		}
		textBox.Translate(0.f, -textHeight);
	}


}

void Menu::AddOption(char const * text, int value, bool isActive)
{
	m_options.push_back(MenuOption(text,value,isActive));
}

void Menu::DisableOption(int value)
{
	for (int i = 0; i < (int)m_options.size() ; i++){
		if (m_options[i].m_value == value){
			m_options[i].m_isActive = false;
			break;
		}
	}
	if (!m_options[m_currentIndex].m_isActive){
		MoveSelection(1);
	}
}

void Menu::EnableOption(int value)
{
	for (int i = 0; i < (int)m_options.size() ; i++){
		if (m_options[i].m_value == value){
			m_options[i].m_isActive = true;
			break;
		}
	}
}

int Menu::GetFrameSelection()
{
	if (m_selectedOption){
		m_selectedOption = false;
		return m_options[m_currentIndex].m_value;
	}
	return -1;
}

void Menu::MoveSelection(int direction)
{
	int numOptions = m_options.size();
	int newPos = m_currentIndex + direction;
	if (newPos < 0){
		newPos = numOptions - 1;
	}
	newPos = newPos % numOptions;
	while (!m_options[newPos].m_isActive){
		newPos = newPos + direction;
		if (newPos < 0){
			newPos = numOptions - 1;
		}
		newPos = newPos % numOptions;
	}
	m_currentIndex = newPos;
}
