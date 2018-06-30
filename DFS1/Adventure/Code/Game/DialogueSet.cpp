#include "DialogueSet.hpp"
#include "Game/Game.hpp"
#include "Game/DialogueSetDefinition.hpp"

DialogueSet::DialogueSet(DialogueSetDefinition * def)
{
	m_definition = def;
	Reset();
}

bool DialogueSet::ProgressAndCheckFinish()
{
	if (!m_dialogues.empty()){
		//opening dialogue for the first time
		//if (m_activeDialogue == nullptr){
		//	g_theGame->Pause();
		//}
		m_activeDialogue = m_dialogues.front();
		m_dialogues.pop();
		return false;
	} else {
		//reached the end of the dialogue
		m_activeDialogue = nullptr;
		//g_theGame->Unpause();
		Reset();
		return true;
	}
}

void DialogueSet::Render(const AABB2 & box)
{
	if (m_activeDialogue != nullptr){
		m_activeDialogue->Render(box);
	}
}

void DialogueSet::Reset()
{
	m_dialogues = std::queue<Dialogue*>();
	if (m_definition != nullptr){
		for (Dialogue* d : m_definition->m_dialogues){
			m_dialogues.push(d);
		}
	}
}
