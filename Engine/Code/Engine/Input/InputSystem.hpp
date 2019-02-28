#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/Mouse.hpp"


//input keys
#define BRACKET_SQUARE_LEFT (VK_OEM_4)
#define BRACKET_SQUARE_RIGHT (VK_OEM_6)
#define PERIOD (VK_OEM_PERIOD);
#define COMMA (VK_OEM_COMMA);
#define PLUS (VK_OEM_PLUS);
#define MINUS (VK_OEM_MINUS);


class InputSystem
{

	
public:
	InputSystem();
	~InputSystem();		//TODO

	KeyButtonState m_keys[256];
	XboxController m_controllers[4];
	Mouse m_mouse;
	bool m_mouseLocked = false;
	bool m_cursorShown = true;

	
	void BeginFrame();
	void EndFrame();

	Vector2 GetMouseDirection() const;
	Vector2 GetMouseNormalizedScreenPosition(AABB2 screenBounds);
	void ToggleMouseLock();
	void LockMouse();
	void UnlockMouse();
	void SetMouseLocked(bool locked);

	void ShowCursor(bool show);
	void ToggleCursor();

	bool IsMouseButtonDown(eMouseButton button);
	bool WasMouseButtonJustPressed(eMouseButton button);
	bool WasMouseButtonJustReleased(eMouseButton button);

	bool IsKeyDown( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;
	XboxController* GetController( int controllerID );

	void PressKey(unsigned char Key);
	void ReleaseKey(unsigned char Key);

	void HandleMouseButton(eMouseButtonAction wparam);

	bool IsShiftDown() const;
	bool IsControlDown() const;

};

