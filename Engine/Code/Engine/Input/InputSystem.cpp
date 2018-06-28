#include "Engine/Input/InputSystem.hpp"
#include <windows.h>

InputSystem::InputSystem()
{
	for(int i = 0; i < 4; i++){
		m_controllers[i] = XboxController(i);
	}
}

InputSystem::~InputSystem()
{

	//delete [] &m_keys;
	//delete [] &m_controllers;
}

void InputSystem::ToggleMouseLock()
{
	m_mouseLocked = !m_mouseLocked;
}

void InputSystem::LockMouse()
{
	m_mouseLocked = true;
}

void InputSystem::UnlockMouse()
{
	m_mouseLocked = false;
}

void InputSystem::SetMouseLocked(bool locked)
{
	m_mouseLocked = locked;
}

void InputSystem::ShowCursor(bool show)
{
	m_cursorShown = show;
	::ShowCursor(m_cursorShown);
}

void InputSystem::ToggleCursor()
{
	m_cursorShown = !m_cursorShown;
	::ShowCursor(m_cursorShown);
}

void InputSystem::BeginFrame()
{
	//poll and set xbox controller states
	for (int i = 0; i < 4; i++){
		m_controllers[i].SetStates();
	}
	

}

void InputSystem::EndFrame()
{
	for (int i = 0; i < 256; i++){
		m_keys[i].PassFrame();
	}

	for (int i = 0; i < 4; i++){
		m_controllers[i].PassFrame();
	}


	m_mouse.Update();
	if (m_mouseLocked){
		m_mouse.LockToCenter();
	}
}

Vector2 InputSystem::GetMouseDirection() const
{
	return m_mouse.GetDirection();
}

bool InputSystem::IsMouseButtonDown(eMouseButton button)
{
	return m_mouse.IsButtonDown(button);
}

bool InputSystem::WasMouseButtonJustPressed(eMouseButton button)
{
	return m_mouse.WasButtonJustPressed(button);
}

bool InputSystem::WasMouseButtonJustReleased(eMouseButton button)
{
	return m_mouse.WasButtonJustReleased(button);
}

bool InputSystem::IsKeyDown(unsigned char keyCode) const
{
	return m_keys[static_cast<int>(keyCode)].IsDown();
}

bool InputSystem::WasKeyJustPressed(unsigned char keyCode) const
{
	return m_keys[static_cast<int>(keyCode)].WasJustPressed();
}

bool InputSystem::WasKeyJustReleased(unsigned char keyCode) const
{
	return m_keys[static_cast<int>(keyCode)].WasJustReleased();
}

XboxController* InputSystem::GetController(int controllerID)
{
	// TODO: insert return statement here
	XboxController* controller = nullptr;
	if (controllerID >= 0 && controllerID < 4){
		controller = &m_controllers[controllerID];
		
	} 
	return controller;
}

void InputSystem::PressKey(unsigned char keyCode)
{
	if (!m_keys[static_cast<int>(keyCode)].IsDown())
	{
		m_keys[static_cast<int>(keyCode)].Press();
	}
}

void InputSystem::ReleaseKey(unsigned char keyCode)
{
	if (m_keys[static_cast<int>(keyCode)].IsDown())
	{
		m_keys[static_cast<int>(keyCode)].Release();
	}
}

void InputSystem::HandleMouseButton(eMouseButtonAction wparam)
{
	switch(wparam){
	case (MOUSE_LEFT_BUTTON_UP):
	case (MOUSE_LEFT_BUTTON_DOWN):
		m_mouse.SetLeftMouseButton(wparam);
		break;
	case (MOUSE_RIGHT_BUTTON_UP):
	case (MOUSE_RIGHT_BUTTON_DOWN):
		m_mouse.SetRightMouseButton(wparam);
		break;
		
	}
}


