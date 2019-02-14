#pragma once
#include "Game/TagSet.hpp"
#include "Game/Character.hpp"


class CharacterState{
public:
	CharacterState(Character* parent);
	CharacterState(CharacterState* previousState);
	~CharacterState();

	std::string ToString() const;

	bool HasTag(TagPair tag);

	Character* m_character;
	TagSet m_tags;
};