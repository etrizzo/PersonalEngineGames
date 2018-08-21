#include "RenderScene.hpp"

RenderScene::RenderScene(Renderer* r)
{
	m_shadowCamera = new Camera();
	m_shadowDepthTarget = r->CreateDepthStencilTarget(2048, 2048);
	m_shadowColorTarget = r->CreateRenderTarget(2048,2048);
	//m_shadowCamera->SetDepthStencilTarget(m_shadowDepthTarget);
	//m_shadowCamera->SetColorTarget(m_shadowColorTarget);
	////m_shadowCamera->SetDepthStencilTarget(m_renderer->m_defaultDepthTarget);
	////m_shadowCamera->SetColorTarget(m_renderer->m_defaultColorTarget);
	//m_shadowCamera->SetProjectionOrtho(50.f, 1.f, .001, 500.f);
	//m_shadowCamera->Finalize();
}

RenderScene::~RenderScene()
{
	delete m_shadowCamera;
	delete m_shadowColorTarget;
	delete m_shadowDepthTarget;
}

void RenderScene::AddRenderable(Renderable * r)
{
	m_renderables.push_back(r);
}

void RenderScene::AddLight(Light * l)
{
	m_lights.push_back(l);
}

void RenderScene::AddCamera(Camera * c)
{
	//m_cameras.clear();		//only allow one camera for now
	m_cameras.push_back(c);
}

void RenderScene::AddParticleSystem(ParticleSystem * ps)
{
	m_particleSystems.push_back(ps);
	ps->m_scene = this;
}

void RenderScene::RemoveCamera(Camera * c)
{
	for (int i = 0; i < (int) m_cameras.size(); i++){
		if (m_cameras[i] == c){
			RemoveAtFast(m_cameras, i);
			break;
		}
	}
}

void RenderScene::RemoveRenderable(Renderable * r)
{
	for (int i = (int) m_renderables.size()-1; i >= 0; i--){
		if (m_renderables[i] == r){
			RemoveAtFast(m_renderables, i);
			break;
		}
	}
}

void RenderScene::RemoveLight(Light * l)
{
	for (int i = (int) m_lights.size()-1; i >= 0; i--){
		if (m_lights[i] == l){
			RemoveAtFast(m_lights, i);
		}
	}
}


Light* RenderScene::AddNewLight(std::string type, Vector3 pos, RGBA color)
{
	if (type == "point"){
		return AddNewPointLight(pos, color);
	} else if (type == "dir" || type == "directional"){
		return AddNewDirectionalLight(pos, color);
	} else if (type == "spot" || type == "cone"){
		return AddNewSpotLight(pos, color);
	} else {
		ConsolePrintf(RGBA::RED, "%s is not a valid light type. Valid types are: < point | dir | spot >", type.c_str());
		return nullptr;
	}
}

Light* RenderScene::AddNewLight(std::string type, Transform t, RGBA color)
{
	if (type == "point"){
		return AddNewPointLight(t.GetWorldPosition(), color);
	} else if (type == "dir" || type == "directional"){
		return AddNewDirectionalLight(t.GetWorldPosition(), color, t.GetEulerAngles());
	} else if (type == "spot" || type == "cone"){
		return AddNewSpotLight(t, color);
	} else {
		ConsolePrintf(RGBA::RED, "%s is not a valid light type. Valid types are: < point | dir | spot >", type.c_str());
		return nullptr;
	}
}

Light* RenderScene::AddNewPointLight(Vector3 pos, RGBA color)
{
	Light* newLight = new Light(pos, color);
	AddLight(newLight);
	return newLight;
}

Light* RenderScene::AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle, float outerAngle)
{
	SpotLight* s = new SpotLight(pos, color, innerAngle,outerAngle);
	AddLight(s);
	return (Light*) s;
}

Light* RenderScene::AddNewSpotLight(Transform t, RGBA color, float innerAngle, float outerAngle)
{
	SpotLight* s = new SpotLight(t.GetWorldPosition(), color, innerAngle,outerAngle);
	s->SetTransform(t);
	AddLight((Light*) s);
	return (Light*) s;
}

Light* RenderScene::AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation)
{
	DirectionalLight* d = new DirectionalLight(pos, color, rotation);
	AddLight((Light*) d);
	return (Light*) d;
}

void RenderScene::RemoveLight(int idx)
{
	if ((int) m_lights.size() > idx){
		if (m_lights.size() > 1){
			Light* del = m_lights[idx];
			m_lights[idx] = m_lights[m_lights.size()-1];
			delete del;
		}
		m_lights.pop_back();
	}
}

void RenderScene::SetLightPosition(Vector3 newPos, unsigned int idx)
{
	if (idx < m_lights.size()){
		m_lights[idx]->SetPosition(newPos);
	}
}

void RenderScene::SetLightColor(RGBA newColor, unsigned int idx)
{
	m_lights[idx]->SetColor(newColor);
}

void RenderScene::SetLightColor(Vector4 newColor, unsigned int idx)
{
	m_lights[idx]->SetColor(newColor);
}

void RenderScene::SetLightAttenuation(int lightIndex, Vector3 att)
{
	if (lightIndex < (int) m_lights.size()){
		m_lights[lightIndex]->SetAttenuation(att);
	}
}

void RenderScene::SetShadowCameraPosition(Vector3 worldPos)
{
	m_shadowCamera->m_transform.SetLocalPosition(worldPos);
	//m_shadowCamera->
}

void RenderScene::SetShadowCameraTransform(const Transform& t)
{
	m_shadowCamera->m_transform.SetLocalMatrix(t.GetWorldMatrix());
}
