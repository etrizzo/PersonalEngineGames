#include "ForwardRenderPath.hpp"
#include "Engine/Math/Renderer.hpp"


ForwardRenderPath::ForwardRenderPath()
{
	m_fogData;
	m_fogData.SetFogBuffer(RGBA::WHITE, 30.f, 45.f, 0.f, .9f);
}

ForwardRenderPath::ForwardRenderPath(Renderer * r)
{
	m_renderer = r;
	Initialize();
}

void ForwardRenderPath::Initialize()
{
	m_fogData;
	m_fogData.SetFogBuffer(RGBA::WHITE, 20.f, 45.f, 0.f, .7f);
	m_shadowDepthTarget = m_renderer->CreateDepthStencilTarget(8192, 8192);
	m_shadowColorTarget = m_renderer->CreateRenderTarget(8192, 8192);
}

void ForwardRenderPath::Render(RenderScene * scene)
{
	m_usingShadows = false;
	BindFog();
	SetShadows(scene);
	scene->SortCameras();



	for (Camera* cam : scene->m_cameras){
		RenderSceneForCamera(cam, scene);
	}
}

void ForwardRenderPath::RenderSceneForCamera(Camera * cam, RenderScene * scene)
{
	
	//now that we know which camera to render from, we can set it
	m_renderer->BindCamera(cam);

	//clear color/depth, based on the camera's settings
	// i.e., are we clearing to a color? sky box?
	ClearForCamera(cam);

	for (ParticleSystem* s : scene->m_particleSystems){
		s->PreRenderForCamera(cam);
	}

	std::vector<DrawCall> drawCalls;
	// generate the draw calls
	for(Renderable* r : scene->m_renderables){
		//this will change for multi-pass shaders or multi-material meshes
		Light* lights[MAX_LIGHTS];
		if (r->GetEditableMaterial()->UsesLights()){
			ComputeMostContributingLights(lights, r->GetPosition(), scene->m_lights);
		}
		for (int i = 0; i < (int) r->m_mesh->m_subMeshes.size(); i++){
			DrawCall dc;
			//set up the draw call for this renderable :)
			// the layer/queue comes from the shader!
			dc.m_mesh = r->m_mesh->m_subMeshes[i];
			dc.m_model = r->m_transform.GetWorldMatrix();
			dc.m_material = r->GetEditableMaterial(i);
			if (m_usingShadows){
				dc.m_material->SetTexture(SHADOW_DEPTH_BINDING, scene->m_shadowCamera->GetDepthTarget());
			}
			dc.m_layer = r->GetEditableMaterial(i)->m_shader->m_sortLayer;
			dc.m_queue = 0;
			if (r->GetEditableMaterial(i)->UsesLights()){
				dc.SetLights(lights);
			}
			//add the draw call to your list of draw calls
			drawCalls.push_back(dc);
		}
	}

	//now we sort draw calls by layer/queue
	SortDrawCalls(drawCalls, cam);
	//sort alpha by distance to camera, etc.

	PROFILE_PUSH("FRP::Draw");
	for(DrawCall dc: drawCalls){
		
		//an optimization would be to only bind the thing if it's different from the previous bind.
		m_renderer->BindMaterial(dc.m_material);
		m_renderer->BindModel(dc.m_model);
		m_renderer->BindLightUniforms(dc.m_lights);
		m_renderer->BindStateAndDrawMesh(dc.m_mesh);
	}
	PROFILE_POP();

	TODO("Add post-processing to forward render path");
	////post-processing? it go here
	//for(Material * effect in cam-> m_effects){
	//	m_renderer->ApplyEffect( effect );
	//}

	//m_renderer->FinishEffects();
}

void ForwardRenderPath::ComputeMostContributingLights(Light* (&lightarray)[8], const Vector3 & position, std::vector<Light*>& lights)
{
	PROFILE_PUSH("FRP::ComputeLights");
	if (lights.size() <= MAX_LIGHTS){
		for(int i = 0; i < MAX_LIGHTS; i++){
			if (i < (int) lights.size()){
				lightarray[i] = lights[i];
			} 
			else {
				lightarray[i] = nullptr;
			}
		}
	} else {
		//sort lights array by light factor
		
		for (int i = 1; i < (int) lights.size(); i ++){
			bool sorted = false;
			for (int j = i; j < (int) lights.size(); j++){
				sorted = true;
				Light* light = lights[j];
				Light* prevLight = lights[j-1];
				if (GetLightFactor(light, position) > GetLightFactor(prevLight, position)){
					//swap l and prevLight
					lights[j] = prevLight;
					lights[j-1] = light;
					sorted = false;
				}
				
			}
			if (sorted){
				break;
			}
		}

		//fill the draw call's light array with the MAX_LIGHTS closest
		for(int i = 0; i < MAX_LIGHTS; i++){
			lightarray[i] = lights[i];
		}
	}
	PROFILE_POP();
}

void ForwardRenderPath::SortDrawCalls(std::vector<DrawCall>& drawCalls, Camera* cam)
{
	PROFILE_PUSH("FRP::SortDrawCalls");
	Vector3 camPos = cam->GetPosition();
	//sort by sort layer
	std::sort(drawCalls.begin(), drawCalls.end(), CompareDrawCallsBySortLayer);

	for (int i = 0; i < (int) drawCalls.size(); i++)
	{
		if (drawCalls[i].GetSortLayer() >= SORT_LAYER_ALPHA)
		{
			drawCalls[i].m_distanceToCamera = drawCalls[i].GetDistance(camPos);
		} else {
			drawCalls[i].m_distanceToCamera = 9999.f;
		}
	}

	std::sort(drawCalls.begin(), drawCalls.end(), CompareAlphaDrawCallsCameraDistance);

	PROFILE_POP();
	//for (int i = 1; i < (int) drawCalls.size(); i ++){
	//	bool sorted = false;
	//	for (int j = 1; j < (int) drawCalls.size(); j++){
	//		sorted = true;
	//		DrawCall dc = drawCalls[j];
	//		DrawCall prevDC = drawCalls[j-1];
	//		if (dc.GetSortLayer() < prevDC.GetSortLayer()){
	//			//swap l and prevLight
	//			drawCalls[j] = prevDC;
	//			drawCalls[j-1] = dc;
	//			sorted = false;
	//		}
	//	}
	//	if (sorted){
	//		break;
	//	}
	//}

	//sort alpha layer
	//for (int i = 1; i < (int) drawCalls.size(); i ++){
	//	bool sorted = false;
	//	if (i >= SORT_LAYER_ALPHA){
	//		for (int j = 1; j < (int) drawCalls.size(); j++){
	//			sorted = true;
	//			DrawCall dc = drawCalls[j];
	//			DrawCall prevDC = drawCalls[j-1];
	//			//sort alpha layer by distance to camera
	//			if (dc.GetSortLayer() >= SORT_LAYER_ALPHA && prevDC.GetSortLayer() >= SORT_LAYER_ALPHA){
	//				//sort by distance to camera
	//				if (dc.GetDistance(camPos) > prevDC.GetDistance(camPos)){
	//					drawCalls[j] = prevDC;
	//					drawCalls[j-1] = dc;
	//					sorted = false;
	//				}
	//			}
	//		}
	//		if (sorted){
	//			break;
	//		}
	//	}
	//}
}

void ForwardRenderPath::SetFogColor(RGBA color)
{
	m_fogData.fogColor = color.GetNormalized();
}

void ForwardRenderPath::BindFog()
{
	TODO("Move fog binding to renderer");
	m_renderer->BindFog(m_fogData);
}

void ForwardRenderPath::SetShadows(RenderScene* scene)
{
	PROFILE_PUSH("FRP::RenderShadows");
	for (Light* light : scene->m_lights){
		if (light->UsesShadows()){
			RenderShadowsForLight(light, scene);
			m_usingShadows = true;
		}
	}
	PROFILE_POP();
}

void ForwardRenderPath::ClearForCamera(Camera * cam)
{
	if (cam->m_skybox != nullptr){
		RenderSkybox(cam);
	} 
	else {
		m_renderer->ClearScreen(cam->m_clearColor);
	}
}

float ForwardRenderPath::GetLightFactor(Light * l, Vector3 position)
{
	float dist = (l->GetPosition() - position).GetLength();
	//return dist;
	return l->GetAttenuation(dist);
}

void ForwardRenderPath::RenderSkybox(Camera * cam)
{
	if (cam->m_skybox != nullptr){
		cam->m_skybox->Update();
		//m_renderer->BindModel(cam->m_transform.GetWorldMatrix());
		m_renderer->DrawSkybox(cam->m_skybox);
	}
}

void ForwardRenderPath::RenderShadowsForLight(Light * l, RenderScene * scene)
{

	//PROFILE_PUSH_FUNCTION_SCOPE();
	//set shadow camera's transform to be the light's transform
	//scene->m_shadowCamera->m_transform.SetWorldMatrix(l->m_transform.GetWorldMatrix());
	
	TODO("Add ability to do multiple light shadows");
	scene->m_shadowCamera->SetDepthStencilTarget(m_shadowDepthTarget);
	scene->m_shadowCamera->SetColorTarget(m_shadowColorTarget);
	//scene->m_shadowCamera->SetDepthStencilTarget(m_renderer->m_defaultDepthTarget);
	//scene->m_shadowCamera->SetColorTarget(m_renderer->m_defaultColorTarget);
	scene->m_shadowCamera->SetProjectionOrtho(100, 1.f, 0.f, 100.f, Vector2(-50.f,-50.f));
	scene->m_shadowCamera->Finalize();
	m_renderer->BindCamera(scene->m_shadowCamera);
	m_renderer->ClearDepth(1.f);

	Matrix44 shadowVP = Matrix44::IDENTITY;
	
	//shadowVP.Append(scene->m_shadowCamera->m_transform.GetWorldMatrix());

	shadowVP.Append(scene->m_shadowCamera->GetProjectionMatrix());
	shadowVP.Append(scene->m_shadowCamera->GetViewMatrix());
	l->SetShadowMatrix(shadowVP);

	// Render scene to shadow camera
	std::vector<DrawCall> drawCalls;
	// generate the draw calls
	for(Renderable* r : scene->m_renderables){
		//don't render alpha meshes for shadows
		if ( r->GetEditableMaterial()->m_shader->m_sortLayer < SORT_LAYER_ALPHA){
			for (int i = 0; i < (int) r->m_mesh->m_subMeshes.size(); i++){
				DrawCall dc;
				//set up the draw call for this renderable :)
				// the layer/queue comes from the shader!
				dc.m_mesh = r->m_mesh->m_subMeshes[i];
				dc.m_model = r->m_transform.GetWorldMatrix();
				dc.m_material = Material::GetMaterial("default_unlit");
				dc.m_layer = r->GetEditableMaterial()->m_shader->m_sortLayer;
				dc.m_queue = 0;
				//add the draw call to your list of draw calls
				drawCalls.push_back(dc);
			}
		}
	}

	//now we sort draw calls by layer/queue
	SortDrawCalls(drawCalls, scene->m_shadowCamera);
	//sort alpha by distance to camera, etc.

	for(DrawCall dc: drawCalls){
		//an optimization would be to only bind the thing if it's different from the previous bind.
		m_renderer->BindMaterial(dc.m_material);
		GL_CHECK_ERROR();
		m_renderer->BindModel(dc.m_model);
		GL_CHECK_ERROR();
		m_renderer->BindStateAndDrawMesh(dc.m_mesh);
		GL_CHECK_ERROR();
	}

}






bool CompareDrawCallsBySortLayer(DrawCall i, DrawCall j)
{
	return i.GetSortLayer() < j.GetSortLayer();
}

bool CompareAlphaDrawCallsCameraDistance(DrawCall i, DrawCall j)
{
	if (i.GetSortLayer() >= SORT_LAYER_ALPHA && j.GetSortLayer() >= SORT_LAYER_ALPHA){
		return i.m_distanceToCamera > j.m_distanceToCamera;
	} else {
		return false;
	}

}
