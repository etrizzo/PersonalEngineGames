#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"
#include "Engine/Renderer/Sprite.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"

class Map;
class GameState_Playing;

enum eAnimState
{
	ANIM_STATE_IDLE,
	ANIM_STATE_WALK,
	ANIM_STATE_ATTACK,
	ANIM_STATE_RELOAD,
	ANIM_STATE_DEATH,
	NUM_ANIM_STATES
};

class Entity{
public:
	virtual ~Entity();									

	virtual void Update();

	virtual void Render();
	virtual void RenderDevMode();

	virtual void TakeDamage();

	//sprite utils
	virtual void UpdateAnimation();
	virtual std::string GetAnimName() const;
	virtual void BeginAttack();
	virtual void ExecuteAttack();

	virtual void BeginReload();
	virtual void ExecuteReload();

	eAnimState m_animState = ANIM_STATE_IDLE;
	SpriteAnimSet* m_animSet = nullptr;
	Sprite* m_sprite = nullptr;
	float m_percThroughAnimationToExecuteAttack = .5f;
	float m_percThroughAnimationToReload = 1.f;

	virtual void BeginDeath();
	virtual bool IsDead() const;

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
	inline Vector3 GetForward() const { return m_transform.GetForward(); }
	inline Vector3 GetUp() const { return m_transform.GetUp(); }
	inline Vector3 GetRight() const { return m_transform.GetRight(); }

	inline Transform GetTransform() const { return m_transform; };
	inline Vector3 GetPosition() const { return m_transform.GetWorldPosition(); };

	bool IsPointInForwardView(Vector3 point);


	Transform m_transform;

	Sphere m_collider;

	float m_spinDegreesPerSecond;
	float m_ageInSeconds;

	float m_range;

	bool m_aboutToBeDeleted;
	bool m_noClipMode;

	Vector3 m_facing;

	//GameState_Playing* m_playState;

};
