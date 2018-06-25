#pragma once
#include "Game/Entity.hpp"
#include "Game/Stats.hpp"
class ItemDefinition;

class Item: public Entity{
public:
	Item(){};
	Item(ItemDefinition* definition, Map* entityMap, Vector2 initialPos, float initialRotation = 0.f, std::string faction = "none");
	~Item(){};

	void Update(float deltaSeconds);
	void Render();

	void RenderInMenu(AABB2 boxToRenderIn, bool selected = false);
	void RenderImageInBox(AABB2 boxToRenderIn);
	void RenderStats(AABB2 boxToRenderIn);

	void RunCorrectivePhysics();

	ItemDefinition* m_definition;
	Stats m_stats;
	bool m_currentlyEquipped = false;
	Texture* GetEquipTexture() const;
	bool ShowsHair() const;
private:
	void RenderEquipmentIcon(AABB2 boxToRenderIn);
	void GetRandomStatsFromDefinition();
};