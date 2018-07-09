#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Adventure.hpp"

// <VictoryConditions sequential="true" completeAll="false" numberToComplete="3">
//		<KillActor haveDied="Balrog" numberKilled="2" />
//		.
//		.
//		.
// </VictoryConditions>


class VictoryCondition{
public:
	VictoryCondition(const tinyxml2::XMLElement* conditionsElement, Quest* quest = nullptr);

	virtual VictoryCondition* Clone(Adventure* adventure, Quest* quest) const = 0;
	
	std::string m_name;
	bool m_complete = false;
	virtual bool CheckIfComplete()  = 0;
	virtual std::string GetText() = 0;
	virtual bool SpeakToGiver() { return false; };		//possibility to update the condition on speaking to actor
	Adventure* m_currentAdventure = nullptr;
	Quest* m_quest;

	static VictoryCondition* CreateVictoryCondition( const tinyxml2::XMLElement* conditionElement );
};


class VictoryCondition_KillActor : public VictoryCondition{
public:
	VictoryCondition_KillActor(const tinyxml2::XMLElement* conditionsElement, Adventure* adv, Quest* quest = nullptr);

	std::string m_actorToKillName;
	int m_numberToKill = 1;
	int m_numberKilled = 0;

	virtual VictoryCondition_KillActor* Clone(Adventure* adventure, Quest* quest) const;
	virtual bool CheckIfComplete();
	virtual std::string GetText();
};

class VictoryCondition_SpeakToActor : public VictoryCondition{
public:
	VictoryCondition_SpeakToActor(const tinyxml2::XMLElement* conditionsElement, Adventure* adv, Quest* quest = nullptr);

	std::string m_actorTypeName;

	virtual VictoryCondition_SpeakToActor* Clone(Adventure* adventure, Quest* quest) const;
	virtual bool CheckIfComplete();
	virtual std::string GetText();
	bool SpeakToGiver() override;
};

class VictoryCondition_CollectItem : public VictoryCondition{
public:
	VictoryCondition_CollectItem(const tinyxml2::XMLElement* conditionsElement, Adventure* adv, Quest* quest = nullptr);

	std::string m_itemName;
	int m_numberToCollect = 1;
	int m_numberCollected = 0;
	bool m_keepItemAfterFinish = true;

	virtual VictoryCondition_CollectItem* Clone(Adventure* adventure, Quest* quest) const;
	virtual bool CheckIfComplete();
	virtual std::string GetText();
};


