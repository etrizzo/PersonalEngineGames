#include "RenderScene.hpp"

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

void RenderScene::AddSkybox(Skybox * s)
{
	m_skybox = s;
}

void RenderScene::AddParticleSystem(ParticleSystem * ps)
{
	m_particleSystems.push_back(ps);
	ps->m_scene = this;
}

void RenderScene::RemoveCamera(Camera * c)
{
	for (int i = 0; i < m_cameras.size(); i++){
		if (m_cameras[i] == c){
			RemoveAtFast(m_cameras, i);
			break;
		}
	}
}

void RenderScene::RemoveRenderable(Renderable * r)
{
	for (int i = m_renderables.size()-1; i >= 0; i--){
		if (m_renderables[i] == r){
			RemoveAtFast(m_renderables, i);
			break;
		}
	}
}

void RenderScene::RemoveLight(Light * l)
{
	for (int i = m_lights.size()-1; i >= 0; i--){
		if (m_lights[i] == l){
			RemoveAtFast(m_lights, i);
		}
	}
}

void RenderScene::RemoveSkybox()
{
	m_skybox = nullptr;
}

void RenderScene::AddNewLight(std::string type, Vector3 pos, RGBA color)
{
	if (type == "point"){
		AddNewPointLight(pos, color);
	} else if (type == "dir" || type == "directional"){
		AddNewDirectionalLight(pos, color);
	} else if (type == "spot" || type == "cone"){
		AddNewSpotLight(pos, color);
	} else {
		ConsolePrintf(RGBA::RED, "%s is not a valid light type. Valid types are: < point | dir | spot >", type.c_str());
		return;
	}
}

void RenderScene::AddNewLight(std::string type, Transform t, RGBA color)
{
	if (type == "point"){
		AddNewPointLight(t.GetWorldPosition(), color);
	} else if (type == "dir" || type == "directional"){
		AddNewDirectionalLight(t.GetWorldPosition(), color, t.GetEulerAngles());
	} else if (type == "spot" || type == "cone"){
		AddNewSpotLight(t, color);
	} else {
		ConsolePrintf(RGBA::RED, "%s is not a valid light type. Valid types are: < point | dir | spot >", type.c_str());
		return;
	}
}

Light* RenderScene::AddNewPointLight(Vector3 pos, RGBA color)
{
	Light* newLight = new Light(pos, color);
	AddLight(newLight);
	return newLight;
}

void RenderScene::AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle, float outerAngle)
{
	SpotLight* s = new SpotLight(pos, color, innerAngle,outerAngle);
	AddLight(s);
}

void RenderScene::AddNewSpotLight(Transform t, RGBA color, float innerAngle, float outerAngle)
{
	SpotLight* s = new SpotLight(t.GetWorldPosition(), color, innerAngle,outerAngle);
	s->SetTransform(t);
	AddLight((Light*) s);
}

void RenderScene::AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation)
{
	DirectionalLight* d = new DirectionalLight(pos, color, rotation);
	AddLight((Light*) d);
}

void RenderScene::RemoveLight(int idx)
{
	if (m_lights.size() > idx){
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
	if (lightIndex < m_lights.size()){
		m_lights[lightIndex]->SetAttenuation(att);
	}
}
