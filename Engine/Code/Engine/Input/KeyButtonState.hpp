#pragma once

class KeyButtonState{

private:
	bool m_isDown;
	bool m_justPressed;
	bool m_justReleased;


public:
	KeyButtonState();

	bool IsDown() const {return m_isDown;};
	bool WasJustPressed() const {return m_justPressed;};
	bool WasJustReleased() const {return m_justReleased;};


	void Press();
	void Release();
	void PassFrame();


};