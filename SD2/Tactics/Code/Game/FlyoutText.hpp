#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

//fire and forget entity that billboards to the camera and moves up in space
class FlyoutText: public Entity{
public:
	FlyoutText(std::string text, Vector3 startPos, float textHeight = .5f, float lengthInSeconds = .5f, RGBA tint = RGBA::WHITE, float speed = 3.f);
	FlyoutText(){};
	~FlyoutText(){};

	void Update(float deltaSeconds);
	void Render();


	std::string m_text;
	Vector3 m_pos;
	float m_lengthInSeconds;
	RGBA m_tint;
	float m_textHeight;
	float m_speed;
	float m_startTime = 0.f;
};