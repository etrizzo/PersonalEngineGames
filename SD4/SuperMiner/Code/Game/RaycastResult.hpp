#pragma once
#include "Game/GameCommon.hpp"
#include "Game/BlockLocator.hpp"

struct RaycastResult
{
	Ray3D m_ray					= Ray3D(Vector3::ZERO, Vector3::ZERO);
	float m_maxDistance			= 0.f;
	Vector3 m_impactPosition	= Vector3::ZERO;
	Vector3 m_endPosition		= Vector3::ZERO;
	float m_impactFraction		= 0.f;
	float m_impactDistance		= 0.f;
	BlockLocator m_impactBlock	= BlockLocator(0, nullptr);
	Vector3 m_impactNormal		= Vector3::ZERO;

	bool DidImpact() const { return m_impactFraction < 1.f; }
};