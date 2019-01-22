#pragma once
#include "Game/GameCommon.hpp"

struct MenuOption
{
public:
	MenuOption(std::string optText, int optValue, bool isActive = true);
	~MenuOption(){};

	std::string m_text;
	int m_value;
	bool m_isActive;

};

class Menu 
{
public: 
	//menu pos in terms of UI camera (Zero to One)
	Menu(AABB2 box){ m_menuBox = box; };
	~Menu(){};

	void HandleInput();   // process input for this menu
	void Render();          // render the menu

	void AddOption( char const *text, int value, bool isActive = true); 
	void DisableOption( int value );
	void EnableOption( int value  );

	int GetHoverIndex() const        { return m_currentIndex; }

	// if the user selected an option this frame
	// returns the value of the current selection, otherwise 
	// returns INVALID_INDEX (0xffffffffU)
	int GetFrameSelection(); 

public:
	int m_currentIndex = 0; 
	std::vector<MenuOption> m_options = std::vector<MenuOption>(); 
	bool m_selectedOption = false;

	AABB2 m_menuBox;

private:
	void MoveSelection(int direction);
};


