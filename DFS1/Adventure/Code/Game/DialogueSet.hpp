#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Dialogue.hpp"

class DialogueSetDefinition;

class DialogueSet{
public:
	DialogueSet(DialogueSetDefinition* def);
	//progresses active dialogue and returns whether or not the dialogue finished- pauses & unpauses appropriately
	bool ProgressAndCheckFinish();
	void Render(const AABB2& box);
	//resets dialogue to the definition's dialogue
	void Reset();

	DialogueSetDefinition* m_definition;
	std::queue<Dialogue*> m_dialogues;
	Dialogue* m_activeDialogue;
};