#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/XboxController.hpp"

class InputSystem
{

	
public:
	InputSystem();
	~InputSystem();		//TODO

	KeyButtonState m_keys[256];
	XboxController m_controllers[4];


	void BeginFrame();
	void EndFrame();

	bool IsKeyDown( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;
	XboxController* GetController( int controllerID );

	void PressKey(unsigned char Key);
	void ReleaseKey(unsigned char Key);

	

};

