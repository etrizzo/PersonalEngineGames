#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderScene2D.hpp"
#include "Engine/Renderer/DrawCall.hpp"

class SpriteRenderPath{
public:
	SpriteRenderPath(){};
	SpriteRenderPath(Renderer* r) { m_renderer = r; };

	void Render(RenderScene2D* scene);
	void RenderSceneForCamera(Camera* cam, RenderScene2D* scene);

	//void ComputeMostContributingLights(DrawCall& drawCall, const Vector3& position, std::vector<Light*>& lights);
	void SortDrawCalls(std::vector<DrawCall> &drawCalls, Camera* cam);


	Renderer* m_renderer;

private:
	//float GetLightFactor(Light* l, Vector3 position);
	//void RenderSkybox(Camera* cam, RenderScene* scene);
};