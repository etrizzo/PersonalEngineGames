#pragma once
#include "Engine/Core/EngineCommon.hpp"




class Light{
public:
	Light();
	Light(Vector3 pos, RGBA color);
	Light(Vector3 pos, Vector4 color);

	
	Vector3 GetPosition() { return m_transform.GetWorldPosition(); };
	void Translate(Vector3 translation);
	void SetPosition(Vector3 newPos);

	void SetAttenuation(Vector3 attenuation);
	float GetAttenuation(float distance) const;

	void SetTransform(Transform newTransform);
	
	void SetColor(RGBA newColor);
	void SetColor(Vector4 newColor);

	virtual void RenderAsPoint(Renderer* r);

	virtual light_t GetBuffer() const;
	
	Transform m_transform;
	RGBA m_lightColor;
	Vector3 m_attenuation = Vector3(.1f, .06f, .05f);
};


class DirectionalLight : public Light{
public:
	DirectionalLight(Vector3 pos, RGBA color, Vector3 rotation);
	light_t GetBuffer() const override;
	void RenderAsPoint(Renderer* r) override;
};


class SpotLight : public Light{
public:
	SpotLight(Vector3 pos, RGBA color, float innerAngleDegrees = 20.f, float outerAngleDegrees = 23.f);

	void LookAt(const Vector3& position, const Vector3& target, const Vector3& up = Vector3::UP);

	light_t GetBuffer() const override;
	void RenderAsPoint(Renderer* r) override;

	float m_innerAngleDot;
	float m_outerAngleDot;
};
