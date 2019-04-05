#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"

class GameState_Playing;
class GameCamera;

class Entity{
public:
	~Entity();									
	Entity() ;									
	Entity(Vector3 position, std::string objFile, std::string material = "default");

	virtual void Update();
	virtual void Render();

	virtual void RenderBounds();

	//virtual void Render();
	virtual void RenderDevMode();

	void SetDiffuseTexture(std::string texPath);
	void SetMaterial(int index, std::string diffusePath, std::string normalPath = "NONE");
	void SetMaterial(Material* mat, int index = 0);

	virtual bool IsAboutToBeDeleted();

	void RunPhysics();
	void RunCorrectivePhysics();


	//sets my camera pointer to this new camera and deletes any existing camera
	void GiveGameCamera(GameCamera* camera);
	//take away my reference to the camera I owned - DOES NOT delete camera
	void RemoveGameCamera();

	IntVector2 GetCurrentChunkCoordinates() const;

	void SetTransform(Transform newT);

	void Translate(Vector3 translation);
	void Rotate(float yaw, float pitch, float roll);
	void SetPosition(Vector3 newPos);
	void SetScale(Vector3 scale);


	//Getters
	inline Vector3 GetForward() const { return m_transform.GetForward(); }
	inline Vector3 GetUp() const { return m_transform.GetUp(); }
	inline Vector3 GetRight() const { return m_transform.GetRight(); }

	inline Transform GetTransform() const { return m_transform; };
	inline Vector3 GetPosition() const { return m_transform.GetWorldPosition(); };

	bool IsPointInForwardView(Vector3 point);


	//Transform m_transform;
	//Mesh* m_mesh;
	//Texture* m_texture;

	Sphere m_collider;

	float m_spinDegreesPerSecond;
	float m_ageInSeconds;

	float m_range;
	int m_health;
	int m_maxHealth;

	bool m_aboutToBeDeleted;
	bool m_noClipMode;

	float m_walkingSpeed = 5.f;
	float m_accelerationXY = 160.f;

	Vector3 m_moveIntention;
	Vector3 m_gravity = Vector3(0.f, 0.f, 0.0f);


	Vector3 m_velocity;

	Renderable* m_renderable;
	GameState_Playing* m_playState;
	Transform m_transform;
	Transform* m_eyePosition;
	float m_eyeHeight = 0.0f;
	GameCamera* m_camera = nullptr;
	eEntityPhysicsMode m_physicsMode = PHYSICS_MODE_NOCLIP;

	std::string GetPhysicsModeString() const;
};
