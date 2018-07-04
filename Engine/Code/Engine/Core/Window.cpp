#include "Engine/Core/Window.hpp"
#include "Engine/Input/InputSystem.hpp"

typedef void (*windows_message_handler_cb)( unsigned int msg, size_t wparam, size_t lparam ); 

//// For singleton style classes, I like to had the instance variable within the CPP; 
//static Window *g_Window = nullptr; // Instance Pointer; 



Window::Window(const char * name, const float aspectRatio, HINSTANCE applicationInstanceHandle)
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)( desktopRect.right - desktopRect.left );
	float desktopHeight = (float)( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	
	if( aspectRatio > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / aspectRatio;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * aspectRatio;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int) clientMarginX;
	clientRect.right = clientRect.left + (int) clientWidth;
	clientRect.top = (int) clientMarginY;
	clientRect.bottom = clientRect.top + (int) clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, name, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	m_hwnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL );

	ShowWindow( m_hwnd, SW_SHOW );
	SetForegroundWindow( m_hwnd );
	SetFocus( m_hwnd );

	m_displayContext = GetDC( m_hwnd );

	m_cursor = LoadCursor( NULL, IDC_ARROW );

	m_dimensions = Vector2(clientWidth, clientHeight);
}

Window::~Window()
{
}

void Window::RegisterHandler(windows_message_handler_cb cb)
{
	listeners.push_back(cb);
}

void Window::UnregisterHandler(windows_message_handler_cb cb)
{
	for(unsigned int i = 0; i < listeners.size(); i++){
		if (listeners[i] == cb){
			listeners.erase(listeners.begin() + i);
			break;
		}
	}
}

void Window::Quit()
{
	m_isQuitting = true;
}

void Window::SetInputSystem(InputSystem * inputSystem)
{
	m_inputSystem = inputSystem;
}

Vector2 Window::GetCenter() const
{
	return m_dimensions * .5f;
}

Vector2 Window::GetCenterInDesktopCoords() const
{
	HWND hwnd = (HWND) GetHandle(); // Get your windows HWND

	RECT client_rect; // window class RECT
	::GetClientRect( hwnd, &client_rect ); 

	Vector2 center = GetCenter();
	POINT c; 
	c.x = (LONG) center.x; 
	c.y = (LONG) center.y; 
	::ClientToScreen( hwnd, &c ); 

	Vector2 desktopCenter = Vector2((float) c.x, (float) c.y);
	return desktopCenter;
}

Vector2 Window::ClientToScreenCoord(Vector2 clientPos)
{
	HWND hwnd = (HWND) GetInstance()->GetHandle();
	POINT desktopPos;
	desktopPos.x = clientPos.x;
	desktopPos.y = clientPos.y;
	::ClientToScreen( hwnd, &desktopPos);

	return Vector2((float) desktopPos.x, (float) desktopPos.y);
}

Window * Window::GetInstance()
{
	return g_Window;
}


// I believe in App you will have a windows procedure you use that looks similar to the following; 
// This will be moved to Windows.cpp (here), with a slight tweak; 
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND hwnd, 
	UINT msg, 
	WPARAM wparam, 
	LPARAM lparam)
{
	// NEW:  Give the custom handlers a chance to run first; 
	Window *window = Window::GetInstance(); 
	if (nullptr != window) {
		for (unsigned int i = 0; i < window->listeners.size(); ++i) {
			window->listeners[i]( msg, wparam, lparam ); 
		}
	}

	// Do what you were doing before - some of this may have to be moved; 
	switch (msg) {
		case WM_CLOSE:		
		{
			window->m_isQuitting = true;
			return 0; // "Consumes" this message (tells Windows "okay, we handled it")
		}

		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			unsigned char asKey = (unsigned char) wparam;

			if (window != nullptr && window->m_inputSystem != nullptr){
				window->m_inputSystem->PressKey(asKey);
			}
			break;

			//returnVal = g_theApp->ProcessKeyDown(asKey);
			//break;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{

			unsigned char asKey = (unsigned char) wparam;
			if (g_Window != nullptr && g_Window->m_inputSystem != nullptr){
				g_Window->m_inputSystem->ReleaseKey(asKey);
			}
			break;
			//returnVal = g_theApp->ProcessKeyUp(asKey);
			//break;
		}
		//handle mouse input
		case WM_LBUTTONDOWN:
			g_Window->m_inputSystem->HandleMouseButton( MOUSE_LEFT_BUTTON_DOWN ); 
			break; 
		case WM_LBUTTONUP:
			g_Window->m_inputSystem->HandleMouseButton( MOUSE_LEFT_BUTTON_UP ); 
			break; 
		case WM_RBUTTONDOWN:
			g_Window->m_inputSystem->HandleMouseButton( MOUSE_RIGHT_BUTTON_DOWN ); 
			break; 
		case WM_RBUTTONUP:
			g_Window->m_inputSystem->HandleMouseButton( MOUSE_RIGHT_BUTTON_UP ); 
			break; 
	}; 

	// do default windows behaviour (return before this if you don't want default windows behavior for certain messages)
	return ::DefWindowProc( hwnd, msg, wparam, lparam );
}