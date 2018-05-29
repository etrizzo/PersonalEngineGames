#include "Engine/Input/InputSystem.hpp"

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
