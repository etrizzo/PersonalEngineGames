#include "DrawCall.hpp"
#include "Engine/Renderer/Material.hpp"

int DrawCall::GetSortLayer()
{
	return m_material->GetShader()->m_sortLayer;
}

float DrawCall::GetDistance(Vector3 pos)
{

	Vector3 myPos = m_model.GetPosition();
	return (pos - myPos).GetLength();
}

void DrawCall::SetLights(Light * lights[MAX_LIGHTS])
{
	for (int i = 0; i < MAX_LIGHTS; i++){
		m_lights[i] = lights[i];
	}
}
