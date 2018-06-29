#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Renderer/DrawCall.hpp"

#define FOG_BUFFER_BINDING (0)

struct fogData_t{
	void SetFogBuffer(RGBA color, float nearPlane, float farPlane, float nearFactor = .1f, float farFactor = .8f);
	//void SetFogColor(RGBA color);


	Vector4 fogColor;
	float fogNearPlane;
	float fogNearFactor;
	float fogFarPlane;
	float fogFarFactor;
};

class ForwardRenderPath{
public:
	ForwardRenderPath();
	ForwardRenderPath(Renderer* r);
	void Initialize();

	void Render(RenderScene* scene);
	void RenderSceneForCamera(Camera* cam, RenderScene* scene);

	void ComputeMostContributingLights(Light* (&lightarray)[8], const Vector3& position, std::vector<Light*>& lights);
	void SortDrawCalls(std::vector<DrawCall> &drawCalls, Camera* cam);

	void BindFog();
	void SetShadows(RenderScene* scene);
	
	Renderer* m_renderer;

private:
	fogData_t	m_fogData;
	UniformBuffer m_fogBuffer;

	void ClearForCamera(Camera* cam);
	float GetLightFactor(Light* l, Vector3 position);
	void RenderSkybox(Camera* cam);
	void RenderShadowsForLight(Light* l, RenderScene* scene);

	Texture* m_shadowDepthTarget;
	Texture* m_shadowColorTarget;
};