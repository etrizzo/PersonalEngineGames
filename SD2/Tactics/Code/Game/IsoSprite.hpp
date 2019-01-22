#include "Game/GameCommon.hpp"

class IsoSprite{
public:
	IsoSprite(tinyxml2::XMLElement* isoSpriteElement);

	Sprite* GetSpriteforViewAngle(Vector3 camDirection, Vector2 actorDirection);

	std::string m_id;
	std::vector<IntVector2> m_directions = {IntVector2::NORTH, IntVector2::SOUTH, IntVector2::EAST, IntVector2::WEST};
	Sprite* m_awayLeft		= nullptr;
	Sprite* m_awayRight		= nullptr;
	Sprite* m_towardLeft	= nullptr;
	Sprite* m_towardRight	= nullptr;

	static std::map<std::string, IsoSprite*> s_isoSprites;
	static IsoSprite* GetIsoSprite(std::string name);
		
protected:
	void SetAwayLeft(tinyxml2::XMLElement* facingElement);
	void SetAwayRight(tinyxml2::XMLElement* facingElement);
	void SetTowardLeft(tinyxml2::XMLElement* facingElement);
	void SetTowardRight(tinyxml2::XMLElement* facingElement);

};