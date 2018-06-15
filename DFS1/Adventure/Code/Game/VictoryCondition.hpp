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
	VictoryCondition(const tinyxml2::XMLElement* conditionsElement);

	virtual VictoryCondition* Clone(Adventure* adventure) const = 0;
	
	std::string m_name;
	bool m_complete = false;
	virtual bool CheckIfComplete()  = 0;
	virtual std::string GetText() = 0;
	Adventure* m_currentAdventure = nullptr;

	static VictoryCondition* CreateVictoryCondition( const tinyxml2::XMLElement* conditionElement );
};


class VictoryCondition_KillActor : public VictoryCondition{
public:
	VictoryCondition_KillActor(const tinyxml2::XMLElement* conditionsElement, Adventure* adv);

	std::string m_actorToKillName;
	int m_numberToKill = 1;
	int m_numberKilled = 0;

	virtual VictoryCondition_KillActor* Clone(Adventure* adventure) const;
	virtual bool CheckIfComplete();
	virtual std::string GetText();
};