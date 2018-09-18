#include <stdio.h>
#include <Windows.h>
#include <Xinput.h> // include the Xinput API
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use 9_1_0 explicitly for broadest compatibility


#include "Engine/Input/XboxController.hpp"

void XboxController::SetButtons(unsigned short wButtons)
{
	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_A)){
		PressXboxButton(XBOX_A);
	} else {
		ReleaseXboxButton(XBOX_A);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_B)){
		PressXboxButton(XBOX_B);
	} else {
		ReleaseXboxButton(XBOX_B);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_X)){
		PressXboxButton(XBOX_X);
	} else {
		ReleaseXboxButton(XBOX_X);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_Y)){
		PressXboxButton(XBOX_Y);
	} else {
		ReleaseXboxButton(XBOX_Y);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_DPAD_DOWN)){
		PressXboxButton(XBOX_D_DOWN);
	}else {
		ReleaseXboxButton(XBOX_D_DOWN);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_DPAD_LEFT)){
		PressXboxButton(XBOX_D_LEFT);
	} else {
		ReleaseXboxButton(XBOX_D_LEFT);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_DPAD_RIGHT)){
		PressXboxButton(XBOX_D_RIGHT);
	} else {
		ReleaseXboxButton(XBOX_D_RIGHT);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_DPAD_UP)){
		PressXboxButton(XBOX_D_UP);
	} else {
		ReleaseXboxButton(XBOX_D_UP);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_START)){
		PressXboxButton(XBOX_START);
	} else {
		ReleaseXboxButton(XBOX_START);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_BACK)){
		PressXboxButton(XBOX_BACK);
	} else {
		ReleaseXboxButton(XBOX_BACK);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER)){
		PressXboxButton(XBOX_BUMPER_LEFT);
	} else {
		ReleaseXboxButton(XBOX_BUMPER_LEFT);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER)){
		PressXboxButton(XBOX_BUMPER_RIGHT);
	} else {
		ReleaseXboxButton(XBOX_BUMPER_RIGHT);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_LEFT_THUMB)){
		PressXboxButton(XBOX_STICK_LEFT);
	} else {
		ReleaseXboxButton(XBOX_STICK_LEFT);
	}

	if (CheckButtonWithMask(wButtons, XINPUT_GAMEPAD_RIGHT_THUMB)){
		PressXboxButton(XBOX_STICK_RIGHT);
	} else {
		ReleaseXboxButton(XBOX_STICK_RIGHT);
	}
}

bool XboxController::CheckButtonWithMask(unsigned short wButtons, short mask)
{
	return ((wButtons & mask) == mask);

}

void XboxController::PressXboxButton(int button)
{
	if (!m_buttons[button].IsDown()){
		m_buttons[button].Press();
	}
}

void XboxController::ReleaseXboxButton(int button)
{
	if (m_buttons[button].IsDown()){
		m_buttons[button].Release();
	}
}

void XboxController::SetTriggers(unsigned char bLeftTrigger, unsigned char bRightTrigger)
{
	m_leftTrigger = RangeMapFloat(bLeftTrigger, 0.f, 255.f, 0.f, 1.f);
	m_rightTrigger = RangeMapFloat(bRightTrigger, 0.f, 255.f, 0.f, 1.f);

	//set virtual buttons
	if (m_leftTrigger > TRIGGER_PRESS_THRESHOLD){
		PressXboxButton(XBOX_TRIGGER_PRESS_LEFT);
	}
	if (m_rightTrigger > TRIGGER_PRESS_THRESHOLD){
		PressXboxButton(XBOX_TRIGGER_PRESS_RIGHT);
	}

	if (m_leftTrigger < TRIGGER_RELEASE_THRESHOLD){
		PressXboxButton(XBOX_TRIGGER_PRESS_LEFT);
	}
	if (m_rightTrigger < TRIGGER_RELEASE_THRESHOLD){
		PressXboxButton(XBOX_TRIGGER_PRESS_RIGHT);
	}

}

void XboxController::SetJoysticks(short sThumbLX, short sThumbLY, short sThumbRX, short sThumbRY)
{
	m_leftStick.SetJoystick(sThumbLX, sThumbLY);
	m_rightStick.SetJoystick(sThumbRX, sThumbRY);

	SetJoystickVirtualButtons();
}

XboxController::XboxController()
{
	m_controllerID = -1;
	m_leftStick = AnalogJoystick();
	m_rightStick = AnalogJoystick();
	m_isConnected = false;
	m_rightTrigger = 0.f;
	m_leftTrigger = 0.f;
	for(int i =0; i < XBOX_NUM_BUTTONS; i++){
		m_buttons[i] = KeyButtonState();
	}
}

XboxController::~XboxController()
{
}

XboxController::XboxController(int ID)
{
	m_controllerID = ID;
	m_leftStick = AnalogJoystick();
	m_rightStick = AnalogJoystick();
	m_isConnected = false;
	m_rightTrigger = 0.f;
	m_leftTrigger = 0.f;
	for(int i =0; i < XBOX_NUM_BUTTONS; i++){
		m_buttons[i] = KeyButtonState();
	}

}

void XboxController::SetStates()
{
	//get the states
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	DWORD errorStatus = XInputGetState( m_controllerID, &xboxControllerState );
	if( errorStatus == ERROR_SUCCESS )
	{
		if (!m_isConnected){
			m_isConnected = true;
		}
		//set buttons
		SetButtons(xboxControllerState.Gamepad.wButtons);
		
		//set triggers
		SetTriggers(xboxControllerState.Gamepad.bLeftTrigger, xboxControllerState.Gamepad.bRightTrigger);

		//set thumbsticks
		SetJoysticks(xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY, xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);
	} else if ( errorStatus == ERROR_DEVICE_NOT_CONNECTED ){
		m_isConnected = false;

	}

}

void XboxController::SetJoystickVirtualButtons()
{
	//set left press
	SetJoystickVirtualButton(m_leftStick.GetX()			, XBOX_THUMBSTICK_LEFT_PRESS_RIGHT	);
	SetJoystickVirtualButton(1.f - m_leftStick.GetX()	, XBOX_THUMBSTICK_LEFT_PRESS_LEFT	);
	SetJoystickVirtualButton(m_leftStick.GetY()			, XBOX_THUMBSTICK_LEFT_PRESS_UP	);
	SetJoystickVirtualButton(1.f - m_leftStick.GetY()	, XBOX_THUMBSTICK_LEFT_PRESS_DOWN	);

	//set right press
	SetJoystickVirtualButton(m_rightStick.GetX()		, XBOX_THUMBSTICK_RIGHT_PRESS_RIGHT	);
	SetJoystickVirtualButton(1.f - m_rightStick.GetX()	, XBOX_THUMBSTICK_RIGHT_PRESS_LEFT	);
	SetJoystickVirtualButton(m_rightStick.GetY()		, XBOX_THUMBSTICK_RIGHT_PRESS_UP	);
	SetJoystickVirtualButton(1.f - m_rightStick.GetY()	, XBOX_THUMBSTICK_RIGHT_PRESS_DOWN	);
}

void XboxController::SetJoystickVirtualButton(float axisVal, XboxButton virtualButton)
{
	if (axisVal > THUMBSTICK_PRESS_THRESHOLD){
		PressXboxButton(virtualButton);
	}
	if (axisVal > THUMBSTICK_PRESS_THRESHOLD){
		PressXboxButton(virtualButton);
	}
}

void XboxController::PassFrame()
{
	if (m_isConnected){
		for (int i = 0; i < XBOX_NUM_BUTTONS; i++){
			m_buttons[i].PassFrame();
		}
	}
}

float XboxController::GetLeftThumbstickMagnitude()
{
	return m_leftStick.GetMagnitude();
}

float XboxController::GetRightThumbstickMagnitude()
{
	return m_rightStick.GetMagnitude();
}

float XboxController::GetLeftThumbstickAngle()
{
	return m_leftStick.GetAngle();
}

float XboxController::GetRightThumbstickAngle()
{
	return m_rightStick.GetAngle();
}

Vector2 XboxController::GetLeftThumbstickCoordinates()
{
	return m_leftStick.GetPositionCartesian();
}

Vector2 XboxController::GetRightThumbstickCoordinates()
{
	return m_rightStick.GetPositionCartesian();
}

bool XboxController::IsButtonDown(int button)
{
	return m_buttons[button].IsDown();
}

bool XboxController::WasButtonJustPressed(int button)
{
	return m_buttons[button].WasJustPressed();
}

bool XboxController::WasButtonJustReleased(int button)
{
	return m_buttons[button].WasJustReleased();
}

float XboxController::GetLeftTrigger()
{
	return m_leftTrigger;
}

float XboxController::GetRightTrigger()
{
	return m_rightTrigger;
}

bool XboxController::IsConnected()
{
	return m_isConnected;
}
