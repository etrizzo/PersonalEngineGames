#include "Mouse.hpp"
#include "Engine/Core/Window.hpp"
#include <wtypes.h>

void Mouse::Update()
{
	m_leftMouseButton.PassFrame();
	m_rightMouseButton.PassFrame();
	UpdatePositions();
}

void Mouse::SetLeftMouseButton(eMouseButtonAction action)
{
	if (action == MOUSE_LEFT_BUTTON_DOWN){
		m_leftMouseButton.Press();
	} else {
		m_leftMouseButton.Release();
	}
}

void Mouse::SetRightMouseButton(eMouseButtonAction action)
{
	if (action == MOUSE_RIGHT_BUTTON_DOWN){
		m_rightMouseButton.Press();
	} else {
		m_rightMouseButton.Release();
	}
}

void Mouse::LockToCenter()
{
	m_lastFramePosition = g_Window->GetCenter();
	SetMousePosition(m_lastFramePosition);
	//Vector2 center = g_Window->GetInstance()->GetCenterInDesktopCoords();
	//::SetCursorPos(center.x, center.y);
	//m_position = g_Window->GetInstance()->GetCenter();
}

Vector2 Mouse::GetCurrentPosition() const
{
	return m_position;
}

Vector2 Mouse::GetNormalizedScreenPosition(AABB2 screenBounds) const
{
	float width = g_Window->GetWidth();
	float height = g_Window->GetHeight();
	Vector2 screenPos = GetCurrentPosition();
	Vector2 pos;
	pos.x = RangeMapFloat(screenPos.x, 0.f, width, screenBounds.mins.x, screenBounds.maxs.x);
	pos.y = RangeMapFloat(screenPos.y, 0.f, height, screenBounds.maxs.y, screenBounds.mins.y);	//flip y coord
	return pos;
}

Vector2 Mouse::GetDirection() const
{
	return m_position - m_lastFramePosition;
}

Vector2 Mouse::GetNormalizedDirection() const
{
	return GetDirection().GetNormalized();
}

void Mouse::SetMousePosition(Vector2 clientPos)
{
	Vector2 desktopPos = g_Window->GetInstance()->ClientToScreenCoord(clientPos);
	SetMouseScreenPosition(desktopPos);
}

void Mouse::SetMouseScreenPosition(Vector2 desktopPos)
{
	::SetCursorPos((int) desktopPos.x, (int) desktopPos.y);
}

bool Mouse::IsButtonDown(eMouseButton button) const
{
	switch (button){
	case MOUSE_BUTTON_LEFT:
		return m_leftMouseButton.IsDown();
	case MOUSE_BUTTON_RIGHT:
		return m_rightMouseButton.IsDown();
	}
	return false;
}

bool Mouse::WasButtonJustPressed(eMouseButton button) const
{
	switch (button){
	case MOUSE_BUTTON_LEFT:
		return m_leftMouseButton.WasJustPressed();
	case MOUSE_BUTTON_RIGHT:
		return m_rightMouseButton.WasJustPressed();
	}
	return false;
}

bool Mouse::WasButtonJustReleased(eMouseButton button) const
{
	switch (button){
	case MOUSE_BUTTON_LEFT:
		return m_leftMouseButton.WasJustReleased();
	case MOUSE_BUTTON_RIGHT:
		return m_rightMouseButton.WasJustReleased();
	}
	return false;
}

void Mouse::UpdatePositions()
{
	POINT desktop_pos;
	::GetCursorPos( &desktop_pos);

	HWND hwnd = (HWND) Window::GetInstance()->GetHandle();

	::ScreenToClient(hwnd, &desktop_pos);
	POINT client_pos = desktop_pos;

	m_lastFramePosition = m_position;
	m_position = Vector2((float) client_pos.x, (float) client_pos.y);

}
