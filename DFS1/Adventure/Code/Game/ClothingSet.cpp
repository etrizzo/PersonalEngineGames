#include "ClothingSet.hpp"
#include "Game/ClothingSetDefinition.hpp"

ClothingSet::ClothingSet()
{
	//m_currentTextures = std::vector<Texture*>();
	//m_defaultTextures = std::vector<Texture*>();
	//m_currentTextures.resize(NUM_RENDER_SLOTS);
	//m_defaultTextures.resize(NUM_RENDER_SLOTS);

	//m_currentTints = std::vector<RGBA>();
	//m_defaultTints = std::vector<RGBA>();
	//m_currentTints.resize(NUM_RENDER_SLOTS);
	//m_defaultTints.resize(NUM_RENDER_SLOTS);
	m_currentLayers = std::vector<ClothingLayer*>();
	m_defaultLayers = std::vector<ClothingLayer*>();
	m_currentLayers.resize(NUM_RENDER_SLOTS);
	m_defaultLayers.resize(NUM_RENDER_SLOTS);
}

void ClothingSet::InitLayer(RENDER_SLOT slot, ClothingLayer * layer)
{
	m_defaultLayers[slot] = layer;
	m_currentLayers[slot] = layer;
}

void ClothingSet::SetDefaultLayer(RENDER_SLOT slot)
{
	m_currentLayers[slot] = m_defaultLayers[slot];
}

void ClothingSet::SetLayer(RENDER_SLOT slot, ClothingLayer * layer)
{
	m_currentLayers[slot] = layer;
}

//void ClothingSet::InitTexture(RENDER_SLOT slot, Texture * texture, RGBA tint)
//{
//	m_defaultTextures[slot] = texture;
//	m_currentTextures[slot] = texture;
//	m_currentTints[slot] = tint;
//	m_defaultTints[slot] = tint;
//}
//
//
//void ClothingSet::SetDefaultTexture(RENDER_SLOT slot)
//{
//	m_currentTextures[slot] = m_defaultTextures[slot];
//}
//
//
//void ClothingSet::SetTexture(RENDER_SLOT slot, Texture * newTexture, RGBA tint)
//{
//	m_currentTextures[slot] = newTexture;
//	m_currentTints[slot] = tint;
//}

//void ClothingSet::SetTexture(RENDER_SLOT slot, ClothingSetDefinition* setDefinition, RGBA tint)
//{
//	SetTexture(slot, setDefinition->GetRandomOfType(slot), tint);
//}

ClothingLayer * ClothingSet::GetLayer(RENDER_SLOT slot) const
{
	return m_currentLayers[slot];
}

Texture * ClothingSet::GetTexture(RENDER_SLOT slot) const
{
	if (m_currentLayers[slot] != nullptr){
		return m_currentLayers[slot]->GetTexture();
	}
	return nullptr;
}

Texture * ClothingSet::GetTexture(int i) const
{
	return GetTexture((RENDER_SLOT) i);
}

RGBA ClothingSet::GetTint(RENDER_SLOT slot) const
{
	return m_currentLayers[slot]->GetTint();
}

RGBA ClothingSet::GetTint(int i) const
{
	return GetTint((RENDER_SLOT) i);
}
