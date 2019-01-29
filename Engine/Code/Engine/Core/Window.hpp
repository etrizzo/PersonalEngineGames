#pragma once
//#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
//#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Core/EngineCommon.hpp"

typedef void (*windows_message_handler_cb)( unsigned int msg, size_t wparam, size_t lparam ); 

class InputSystem;

class Window
{
public:
	// Do all the window creation work is currently in App
	Window( const char* name, const float aspectRatio, HINSTANCE applicationInstanceHandle, float maxClientFraction); 
	~Window();

	// Register a function callback to the Window.  Any time Windows processing a 
	// message, this callback should forwarded the information, along with the
	// supplied user argument. 
	void RegisterHandler( windows_message_handler_cb cb ); 

	// Allow users to unregster (not needed for this Assignment, but I 
	// like having cleanup methods). 
	void UnregisterHandler( windows_message_handler_cb cb ); 

	// This is safely castable to an HWND
	void* GetHandle() const { return m_hwnd; }
	void Quit();

	void SetInputSystem(InputSystem* inputSystem);

	Vector2 GetCenter() const;
	Vector2 GetCenterInDesktopCoords() const;

	Vector2 ClientToScreenCoord(Vector2 clientPos);

	// ** EXTRAS ** //
	// void SetTitle( char const *new_title ); 

	// When the WindowsProcedure is called - let all listeners get first crack at the message
	// Giving us better code modularity. 
	std::vector<windows_message_handler_cb> listeners; 
	HDC m_displayContext;
	InputSystem* m_inputSystem = nullptr;	//pointer to app's input system?
	bool m_isQuitting = false;
	HCURSOR m_cursor;

private:
	HWND m_hwnd; // intptr_t  
	Vector2 m_dimensions;
	Vector2 m_center;

	

public:
	float GetWidth()		const { return m_dimensions.x; };
	float GetHeight()		const { return m_dimensions.y; };
	Vector2 GetDimensions()	const {return m_dimensions; };
	static Window* GetInstance();
};

LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND hwnd, 
	UINT msg, 
	WPARAM wparam, 
	LPARAM lparam);