#pragma once
#include "Engine/Core/EngineCommon.hpp"


struct particle_t{
	Vector3 m_position;
	Vector3 m_velocity;
	Vector3 m_force;
	float m_mass;
	RGBA m_tint;
	float m_size;

	float m_timeBorn;		//time the particle was born
	float m_timeDead;		//time the particle will die

	void Update(float ds);
	void SetTint(RGBA& tint);

	float GetNormalizedAge(float t);		//will give age on a zero-to-one range

	inline bool IsDead(float time) {return time >= m_timeDead; }
};