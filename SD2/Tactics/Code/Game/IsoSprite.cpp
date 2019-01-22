#include "IsoSprite.hpp"
#include "Engine/Renderer/Sprite.hpp"

std::map<std::string, IsoSprite*>	IsoSprite::s_isoSprites;

IsoSprite::IsoSprite(tinyxml2::XMLElement* isoSpriteElement)
{
	m_id = ParseXmlAttribute(*isoSpriteElement, "id", "NO_ID");

	for (tinyxml2::XMLElement* facingElement = isoSpriteElement->FirstChildElement("facing"); facingElement != nullptr; facingElement = facingElement->NextSiblingElement("facing")){
		IntVector2 dir = ParseXmlAttribute(*facingElement, "dir", IntVector2(0,0));
		if (dir == IntVector2::AWAY_LEFT){
			SetAwayLeft(facingElement);
		} else if (dir == IntVector2::AWAY_RIGHT){
			SetAwayRight(facingElement);
		} else if (dir == IntVector2::TOWARD_LEFT){
			SetTowardLeft(facingElement);
		} else if (dir == IntVector2::TOWARD_RIGHT){
			SetTowardRight(facingElement);
		}
	}
}

Sprite * IsoSprite::GetSpriteforViewAngle(Vector3 camDirection, Vector2 actorDirection)
{
	Vector2 camForward = camDirection.XZ();
	Vector2 camRight = Vector2(camForward.y, -camForward.x);
	
	float facingAway = DotProduct(camForward, actorDirection);
	float right  = DotProduct(camRight, actorDirection);
	if (facingAway >= 0.f){
		if (right >= 0.f){
			return m_awayRight;
		} else {
			return m_awayLeft;
		}
	} else {
		if (right >= 0.f){
			return m_towardRight;
		} else {
			return m_towardLeft;
		}
	}
	return nullptr;
}

IsoSprite * IsoSprite::GetIsoSprite(std::string name)
{
	auto containsSprite = s_isoSprites.find((std::string)name); 
	IsoSprite* sprite = nullptr;
	if (containsSprite != s_isoSprites.end()){
		sprite = containsSprite->second;
	}
	return sprite;
}

void IsoSprite::SetAwayLeft(tinyxml2::XMLElement* facingElement)
{
	std::string src = ParseXmlAttribute(*facingElement, "src", "NO_SRC");
	Vector2 scale = ParseXmlAttribute(*facingElement, "scale", Vector2(1,1));
	Sprite* sprite = g_theRenderer->GetSprite(src);
	if (scale != Vector2(1.f,1.f)){
		m_awayLeft = sprite->GetScaledSprite(scale);
	} else {
		m_awayLeft = sprite;
	}
	
}

void IsoSprite::SetAwayRight(tinyxml2::XMLElement* facingElement)
{
	std::string src = ParseXmlAttribute(*facingElement, "src", "NO_SRC");
	Vector2 scale = ParseXmlAttribute(*facingElement, "scale", Vector2(1,1));
	Sprite* sprite = g_theRenderer->GetSprite(src);
	if (scale != Vector2(1.f,1.f)){
		m_awayRight = sprite->GetScaledSprite(scale);
	} else {
		m_awayRight = sprite;
	}
}

void IsoSprite::SetTowardLeft(tinyxml2::XMLElement* facingElement)
{
	std::string src = ParseXmlAttribute(*facingElement, "src", "NO_SRC");
	Vector2 scale = ParseXmlAttribute(*facingElement, "scale", Vector2(1,1));
	Sprite* sprite = g_theRenderer->GetSprite(src);
	if (scale != Vector2(1.f,1.f)){
		m_towardLeft = sprite->GetScaledSprite(scale);
	} else {
		m_towardLeft = sprite;
	}
}

void IsoSprite::SetTowardRight(tinyxml2::XMLElement* facingElement)
{
	std::string src = ParseXmlAttribute(*facingElement, "src", "NO_SRC");
	Vector2 scale = ParseXmlAttribute(*facingElement, "scale", Vector2(1,1));
	Sprite* sprite = g_theRenderer->GetSprite(src);
	if (scale != Vector2(1.f,1.f)){
		m_towardRight = sprite->GetScaledSprite(scale);
	} else {
		m_towardRight = sprite;
	}
}
