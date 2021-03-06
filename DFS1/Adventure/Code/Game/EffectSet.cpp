#include "EffectSet.hpp"
#pragma once

EffectSet::EffectSet(tinyxml2::XMLElement * effectsElement, StoryDataDefinition* parent)
{
	m_parentData = parent;
	if (effectsElement != nullptr){
		for (tinyxml2::XMLElement* tagElement = effectsElement->FirstChildElement("TagChange"); tagElement != nullptr; tagElement = tagElement->NextSiblingElement("TagChange")){
			Effect* newEffect = (Effect*) new Effect_TagChange(tagElement, parent);
			m_effects.push_back(newEffect);
		}
	}

	TODO("Parse trait effects and story effects");
}

EffectSet::EffectSet(EffectSet * clone, StoryDataDefinition * newParent)
{
	m_parentData = newParent;
	for (Effect* eff : clone->m_effects){
		Effect* newEffect = eff->Clone();
		newEffect->m_parentData = newParent;
		m_effects.push_back(newEffect);
	}
}
