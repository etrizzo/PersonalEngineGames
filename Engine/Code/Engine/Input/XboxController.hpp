#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"


enum XboxButton{
	XBOX_A,
	XBOX_B,
	XBOX_X,
	XBOX_Y,
	XBOX_D_LEFT,
	XBOX_D_RIGHT,
	XBOX_D_UP,
	XBOX_D_DOWN,
	XBOX_START,
	XBOX_BACK,
	XBOX_BUMPER_RIGHT,
	XBOX_BUMPER_LEFT,
	XBOX_STICK_LEFT,
	XBOX_STICK_RIGHT,

	XBOX_TRIGGER_PRESS_LEFT,
	XBOX_TRIGGER_PRESS_RIGHT,

	XBOX_THUMBSTICK_LEFT_PRESS_LEFT		,
	XBOX_THUMBSTICK_LEFT_PRESS_RIGHT	,
	XBOX_THUMBSTICK_LEFT_PRESS_UP		,
	XBOX_THUMBSTICK_LEFT_PRESS_DOWN		,
	XBOX_THUMBSTICK_RIGHT_PRESS_LEFT	,
	XBOX_THUMBSTICK_RIGHT_PRESS_RIGHT	,
	XBOX_THUMBSTICK_RIGHT_PRESS_UP		,
	XBOX_THUMBSTICK_RIGHT_PRESS_DOWN	,

	XBOX_NUM_BUTTONS
};

#define TRIGGER_RELEASE_THRESHOLD (.45f)
#define TRIGGER_PRESS_THRESHOLD (.55f)

#define THUMBSTICK_PRESS_THRESHOLD (.95f)
#define THUMBSTICK_RELEASE_THRESHOLD (.9f)

class XboxController {
	KeyButtonState m_buttons[XBOX_NUM_BUTTONS];
	AnalogJoystick m_leftStick;
	AnalogJoystick m_rightStick;
	int m_controllerID;
	bool m_isConnected;

	float m_rightTrigger;
	float m_leftTrigger;


	void SetButtons(unsigned short wButtons);
	void SetTriggers(unsigned char bLeftTrigger, unsigned char bRightTrigger);
	void SetJoysticks(short sThumbLX, short sThumbLY, short sThumbRX, short sThumbRY);
	bool CheckButtonWithMask(unsigned short wButtons, short mask);
	void PressXboxButton(int button);
	void ReleaseXboxButton(int button);
	

public:
	XboxController();
	~XboxController();
	XboxController(int ID);

	void SetStates();
	void SetJoystickVirtualButtons();
	void SetJoystickVirtualButton(float axisVal, XboxButton virtualButton);
	void PassFrame();

	float GetLeftThumbstickMagnitude();
	float GetRightThumbstickMagnitude();

	float GetLeftThumbstickAngle();
	float GetRightThumbstickAngle();

	Vector2 GetLeftThumbstickCoordinates();
	Vector2 GetRightThumbstickCoordinates();

	bool IsButtonDown(int button);
	bool WasButtonJustPressed(int button);
	bool WasButtonJustReleased(int button);

	float GetLeftTrigger();
	float GetRightTrigger();

	bool IsConnected();

};