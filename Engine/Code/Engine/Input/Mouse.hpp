#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/KeyButtonState.hpp"

enum eMouseButton{
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	NUM_MOUSE_BUTTONS
};

enum eMouseButtonAction{
	MOUSE_LEFT_BUTTON_DOWN,
	MOUSE_LEFT_BUTTON_UP,
	MOUSE_RIGHT_BUTTON_DOWN,
	MOUSE_RIGHT_BUTTON_UP,
	NUM_MOUSE_ACTIONS

};

class Mouse{
public:
	Mouse(){};

	void Update();
	void SetLeftMouseButton(eMouseButtonAction action);
	void SetRightMouseButton(eMouseButtonAction action);

	void LockToCenter();

	Vector2 GetCurrentPosition() const;
	Vector2 GetDirection() const;
	Vector2 GetNormalizedDirection() const;

	void SetMousePosition(Vector2 clientPos);
	void SetMouseScreenPosition(Vector2 desktopPos);

	bool IsButtonDown(eMouseButton button) const;
	bool WasButtonJustPressed(eMouseButton button) const;
	bool WasButtonJustReleased(eMouseButton button) const;

	KeyButtonState m_leftMouseButton;
	KeyButtonState m_rightMouseButton;

	Vector2 m_position;
	Vector2 m_lastFramePosition;

private:
	void UpdatePositions();
};