#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"

class Map;

class Entity{
public:
	~Entity();									
	Entity() ;									
	//Entity(Vector3 position, std::string objFile, std::string texturePath = "default");
	Entity(Vector3 position, std::string objFile, std::string materialFile = "default");

	virtual void Update();

	//virtual void Render();
	virtual void RenderDevMode();

	void SetDiffuseTexture(std::string texPath);
	void SetMaterial(int index, std::string diffusePath, std::string normalPath = "NONE");

	virtual bool IsAboutToBeDeleted();

	virtual void RunCorrectivePhysics();
	virtual void RunWorldPhysics();
	virtual void RunEntityPhysics();

	void RenderHealthBar();

	void SetTransform(Transform newT);

	void Translate(Vector3 translation);
	void Rotate(Vector3 rotation);
	void SetPosition(Vector3 newPos);

	bool IsPointInForwardView(Vector3 point);

	
	//Transform m_transform;
	//Mesh* m_mesh;
	//Texture* m_texture;

	float m_spinDegreesPerSecond;
	float m_ageInSeconds;

	float m_range;
	int m_health;
	int m_maxHealth;

	bool m_aboutToBeDeleted;
	bool m_noClipMode;

	Renderable* m_renderable;

};
