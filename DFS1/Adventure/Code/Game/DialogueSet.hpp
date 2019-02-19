#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Dialogue.hpp"

class DialogueSetDefinition;
class Actor;

class DialogueSet{
public:
	DialogueSet(DialogueSetDefinition* def);
	//progresses active dialogue and returns whether or not the dialogue finished- pauses & unpauses appropriately
	bool ProgressAndCheckFinish();
	void Render(const AABB2& box);
	//resets dialogue to the definition's dialogue
	void Reset();

	bool m_hasDefinition = true;
	Actor* m_speaker = nullptr;
	DialogueSetDefinition* m_definition = nullptr;
	Dialogue* m_activeDialogue = nullptr;
	std::queue<Dialogue*> m_dialogues;
	
};