#include "Engine/Input/KeyButtonState.hpp"

KeyButtonState::KeyButtonState()
{
	m_isDown = false;
	m_justPressed = false;
	m_justReleased = false;
}

void KeyButtonState::Press()
{ 
	m_isDown = true;
	m_justPressed = true;
	m_justReleased = false;		//is this necessary?
};

void KeyButtonState::Release()
{
	m_isDown = false;
	m_justPressed = false;		//is this necessary?
	m_justReleased = true;
};

void KeyButtonState::PassFrame()
{
	if (m_justPressed){
		m_justPressed = false;
	}
	if (m_justReleased){
		m_justReleased = false;
	}
}