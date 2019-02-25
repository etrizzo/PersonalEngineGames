#include "DialogueSet.hpp"
#include "Game/Game.hpp"
#include "Game/DialogueSetDefinition.hpp"
#include "Game/Actor.hpp"

DialogueSet::DialogueSet(DialogueSetDefinition * def)
{
	m_definition = def;
	if (m_definition != nullptr)
	{
		for (Dialogue* dialog : m_definition->m_dialogues)
		{
			m_dialogues.push_back(new Dialogue(dialog->m_content));
		}
	}
	Reset();
}

bool DialogueSet::ProgressAndCheckFinish()
{
	if (m_dialogueIndex < (int) m_dialogues.size()){
		//opening dialogue for the first time
		//if (m_activeDialogue == nullptr){
		//	g_theGame->Pause();
		//}
		m_activeDialogue = m_dialogues[m_dialogueIndex];
		m_dialogueIndex++;
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
		AABB2 wholeBox = AABB2(box);
		//draw the outline
		g_theRenderer->DrawAABB2(wholeBox, RGBA::BLACK.GetColorWithAlpha(200));
		g_theRenderer->DrawAABB2Outline(wholeBox, RGBA::WHITE);
		//split the box
		AABB2 portraitBox = AABB2(wholeBox);
		AABB2 dialogueBox = AABB2(wholeBox);
		//get a square for the dialogue
		portraitBox.maxs.x = wholeBox.mins.x + wholeBox.GetHeight();
		dialogueBox.mins.x = portraitBox.maxs.x;
		//wholeBox.SplitAABB2Vertical(.85f, dialogueBox, portraitBox);
		//render the text
		m_activeDialogue->Render(dialogueBox, m_speaker->m_name);
		//render the portrait
		m_speaker->RenderFaceInBox(portraitBox);
	}
	
}

void DialogueSet::Reset()
{
	m_dialogueIndex = 0;
}

void DialogueSet::ClearDialogues()
{
	for (Dialogue* dialogue : m_dialogues)
	{
		delete dialogue;
	}
	m_dialogues = std::vector<Dialogue*>();
}

void DialogueSet::AddDialogueLine(std::string newLine)
{
	m_dialogues.push_back(new Dialogue(newLine));
}
