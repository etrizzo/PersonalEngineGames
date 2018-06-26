#include "ClothingSet.hpp"
#include "Game/ClothingSetDefinition.hpp"

ClothingSet::ClothingSet()
{
	m_currentTextures = std::vector<Texture*>();
	m_defaultTextures = std::vector<Texture*>();
	m_currentTextures.resize(NUM_RENDER_SLOTS);
	m_defaultTextures.resize(NUM_RENDER_SLOTS);
}

void ClothingSet::InitTexture(RENDER_SLOT slot, Texture * texture)
{
	m_defaultTextures[slot] = texture;
	m_currentTextures[slot] = texture;
}

void ClothingSet::SetDefaultTexture(RENDER_SLOT slot)
{
	m_currentTextures[slot] = m_defaultTextures[slot];
}

void ClothingSet::SetTexture(RENDER_SLOT slot, Texture * newTexture)
{
	m_currentTextures[slot] = newTexture;
}

void ClothingSet::SetTexture(RENDER_SLOT slot, ClothingSetDefinition* setDefinition)
{
	SetTexture(slot, setDefinition->GetRandomOfType(slot));
}

Texture * ClothingSet::GetTexture(RENDER_SLOT slot) const
{
	return m_currentTextures[slot];
}

Texture * ClothingSet::GetTexture(int i) const
{
	return GetTexture((RENDER_SLOT) i);
}
