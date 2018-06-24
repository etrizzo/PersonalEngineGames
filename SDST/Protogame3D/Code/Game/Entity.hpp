#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"

class Map;
class GameState_Playing;

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
	void SetMaterial(Material* mat, int index = 0);

	virtual bool IsAboutToBeDeleted();

	virtual void RunCorrectivePhysics();
	virtual void RunWorldPhysics();
	virtual void RunEntityPhysics();

	void RenderHealthBar();

	void SetTransform(Transform newT);

	void Translate(Vector3 translation);
	void Rotate(Vector3 rotation);
	void SetPosition(Vector3 newPos);
	void SetScale(Vector3 scale);


	//Getters
	inline Vector3 GetForward() const { return m_renderable->m_transform.GetForward(); }
	inline Vector3 GetUp() const { return m_renderable->m_transform.GetUp(); }
	inline Vector3 GetRight() const { return m_renderable->m_transform.GetRight(); }

	inline Transform GetTransform() const { return m_renderable->m_transform; };
	inline Vector3 GetPosition() const { return m_renderable->GetPosition(); };

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
	GameState_Playing* m_playState;

};
