#include "ClothingSet.hpp"
#include "Game/ClothingSetDefinition.hpp"

ClothingSet::ClothingSet()
{
	m_currentTextures = std::vector<Texture*>();
	m_defaultTextures = std::vector<Texture*>();
	m_currentTextures.resize(NUM_RENDER_SLOTS);
	m_defaultTextures.resize(NUM_RENDER_SLOTS);

	m_currentTints = std::vector<RGBA>();
	m_defaultTints = std::vector<RGBA>();
	m_currentTints.resize(NUM_RENDER_SLOTS);
	m_defaultTints.resize(NUM_RENDER_SLOTS);
}

void ClothingSet::InitTexture(RENDER_SLOT slot, Texture * texture, RGBA tint)
{
	m_defaultTextures[slot] = texture;
	m_currentTextures[slot] = texture;
	m_currentTints[slot] = tint;
	m_defaultTints[slot] = tint;
}


void ClothingSet::SetDefaultTexture(RENDER_SLOT slot)
{
	m_currentTextures[slot] = m_defaultTextures[slot];
}


void ClothingSet::SetTexture(RENDER_SLOT slot, Texture * newTexture, RGBA tint)
{
	m_currentTextures[slot] = newTexture;
	m_currentTints[slot] = tint;
}

void ClothingSet::SetTexture(RENDER_SLOT slot, ClothingSetDefinition* setDefinition, RGBA tint)
{
	SetTexture(slot, setDefinition->GetRandomOfType(slot), tint);
}

Texture * ClothingSet::GetTexture(RENDER_SLOT slot) const
{
	return m_currentTextures[slot];
}

Texture * ClothingSet::GetTexture(int i) const
{
	return GetTexture((RENDER_SLOT) i);
}

RGBA ClothingSet::GetTint(RENDER_SLOT slot) const
{
	return m_currentTints[slot];
}

RGBA ClothingSet::GetTint(int i) const
{
	return GetTint((RENDER_SLOT) i);
}
