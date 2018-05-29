#include "Particle.hpp"
#pragma once

void particle_t::Update(float ds)
{
	//just apply stuff
	//forward-euler (calculate things first then apply, as opposed to apply and calculate for next frame)
	Vector3 acceleration = m_force / m_mass;
	m_velocity += acceleration * ds;
	m_position += m_velocity * ds;

	//zero out the force, so that it's a by-frame force
	m_force = Vector3::ZERO;
}

void particle_t::SetTint(RGBA & tint)
{
	m_tint = tint;
}

float particle_t::GetNormalizedAge(float t)
{
	return (t- m_timeBorn) / (m_timeDead - m_timeBorn);
}
