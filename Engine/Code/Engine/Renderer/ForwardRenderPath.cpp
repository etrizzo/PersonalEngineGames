#include "ForwardRenderPath.hpp"
#include "Engine/Math/Renderer.hpp"


ForwardRenderPath::ForwardRenderPath()
{
	m_fogData;
	m_fogData.SetFogBuffer(RGBA::WHITE, 20.f, 45.f, 0.f, .7f);
}

ForwardRenderPath::ForwardRenderPath(Renderer * r)
{
	m_renderer = r;
	m_fogData;
	m_fogData.SetFogBuffer(RGBA::WHITE, 5.f, 10.f, .1f, 1.f);
}

void ForwardRenderPath::Render(RenderScene * scene)
{
	BindFog();
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
			dc.m_layer = r->GetEditableMaterial()->m_shader->m_sortLayer;
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

	for(DrawCall dc: drawCalls){
		//an optimization would be to only bind the thing if it's different from the previous bind.
		m_renderer->BindMaterial(dc.m_material);
		m_renderer->BindModel(dc.m_model);
		m_renderer->BindLightUniforms(dc.m_lights);
		m_renderer->DrawMesh(dc.m_mesh);
	}

	TODO("Add post-processing to forward render path");
	////post-processing? it go here
	//for(Material * effect in cam-> m_effects){
	//	m_renderer->ApplyEffect( effect );
	//}

	//m_renderer->FinishEffects();
}

void ForwardRenderPath::ComputeMostContributingLights(Light* (&lightarray)[8], const Vector3 & position, std::vector<Light*>& lights)
{
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

		//fill the drawcall's light array with the MAX_LIGHTS closest
		for(int i = 0; i < MAX_LIGHTS; i++){
			lightarray[i] = lights[i];
		}
	}
}

void ForwardRenderPath::SortDrawCalls(std::vector<DrawCall>& drawCalls, Camera* cam)
{
	Vector3 camPos = cam->GetPosition();
	//sort by sort layer
	for (int i = 1; i < (int) drawCalls.size(); i ++){
		bool sorted = false;
		for (int j = i; j < (int) drawCalls.size(); j++){
			sorted = true;
			DrawCall dc = drawCalls[j];
			DrawCall prevDC = drawCalls[j-1];
			if (dc.GetSortLayer() < prevDC.GetSortLayer()){
				//swap l and prevLight
				drawCalls[j] = prevDC;
				drawCalls[j-1] = dc;
				sorted = false;
			}
			//sort alpha layer by distance to camera
			if (dc.GetSortLayer() == 1 && prevDC.GetSortLayer() == 1){
				//sort by distance to camera
				if (dc.GetDistance(camPos) > prevDC.GetDistance(camPos)){
					drawCalls[j] = prevDC;
					drawCalls[j-1] = dc;
					sorted = false;
				}
			}

		}
		if (sorted){
			break;
		}
	}
}

void ForwardRenderPath::BindFog()
{
	TODO("Move fog binding to renderer");
	m_fogBuffer.CopyToGPU( sizeof(m_fogData), &m_fogData);
	glBindBufferBase( GL_UNIFORM_BUFFER, 
		FOG_BUFFER_BINDING, 
		m_fogBuffer.GetHandle() ); 
}

void ForwardRenderPath::ClearForCamera(Camera * cam)
{
	if (cam->m_skybox != nullptr){
		RenderSkybox(cam);
	} 
	/*else {
		m_renderer->ClearScreen(cam->m_clearColor);
	}*/
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

void fogData_t::SetFogBuffer(RGBA color, float nearPlane, float farPlane, float nearFactor, float farFactor)
{
	fogColor = color.GetNormalized();
	fogNearPlane = nearPlane;
	fogFarPlane = farPlane;
	fogNearFactor = nearFactor;
	fogFarFactor = farFactor;
}
