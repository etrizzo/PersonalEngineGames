#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Renderer/DrawCall.hpp"




class ForwardRenderPath{
public:
	ForwardRenderPath();
	ForwardRenderPath(Renderer* r);
	void Initialize();

	void Render(RenderScene* scene);
	void RenderSceneForCamera(Camera* cam, RenderScene* scene);

	void ComputeMostContributingLights(Light* (&lightarray)[8], const Vector3& position, std::vector<Light*>& lights);
	void SortDrawCalls(std::vector<DrawCall> &drawCalls, Camera* cam);

	void SetFogColor(RGBA color);
	void BindFog();
	void SetShadows(RenderScene* scene);
	
	Renderer* m_renderer;
	bool m_usingShadows;

private:
	FogData_t	m_fogData;
	UniformBuffer m_fogBuffer;

	void ClearForCamera(Camera* cam);
	float GetLightFactor(Light* l, Vector3 position);
	void RenderSkybox(Camera* cam);
	void RenderShadowsForLight(Light* l, RenderScene* scene);

	Texture* m_shadowDepthTarget;
	Texture* m_shadowColorTarget;
};


bool CompareDrawCallsBySortLayer(DrawCall i, DrawCall j);
bool CompareAlphaDrawCallsCameraDistance(DrawCall i, DrawCall j);