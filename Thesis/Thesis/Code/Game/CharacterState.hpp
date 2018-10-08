#pragma once
#include "Game/Tags.hpp"
#include "Game/Character.hpp"


class CharacterState{
public:
	CharacterState(Character* parent);

	bool HasTag(TagPair tag);

	Character* m_character;
	Tags m_tags;
};