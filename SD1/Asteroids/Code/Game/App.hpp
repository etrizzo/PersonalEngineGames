#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Audio/AudioSystem.hpp"




class App{
public:
	~App() {}											// destructor: do nothing (for speed)
	App(); 											// default constructor: do nothing (for speed)

	App(HINSTANCE applicationInstanceHandle);

	
	Game* m_theGame;
	Vector2 m_bottomLeft;
	Vector2 m_topRight;
	RGBA m_backgroundColor;

	bool m_isQuitting;

	double m_appTime;
	float m_deltaTime;

public:
	void RunFrame();

	void Update();

	void Render();

	bool IsQuitting();

private:
	void CheckKeys();
};




bool TestXboxVirtualButtons();