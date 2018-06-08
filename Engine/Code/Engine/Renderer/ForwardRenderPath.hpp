#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Renderer/DrawCall.hpp"

class ForwardRenderPath{
public:
	ForwardRenderPath(){};
	ForwardRenderPath(Renderer* r) { m_renderer = r; };

	void Render(RenderScene* scene);
	void RenderSceneForCamera(Camera* cam, RenderScene* scene);

	void ComputeMostContributingLights(Light* (&lightarray)[8], const Vector3& position, std::vector<Light*>& lights);
	void SortDrawCalls(std::vector<DrawCall> &drawCalls, Camera* cam);

	
	Renderer* m_renderer;

private:
	void ClearForCamera(Camera* cam);
	float GetLightFactor(Light* l, Vector3 position);
	void RenderSkybox(Camera* cam);
};