#include "SpriteRenderPath.hpp"

void SpriteRenderPath::Render(RenderScene2D * scene)
{
	scene->SortCameras();
	//for (Camera* cam : scene->m_cameras){
	//	RenderSkybox(cam, scene);
	//	RenderSceneForCamera(cam, scene);
	//}
	RenderSceneForCamera(scene->m_cameras[0], scene);
	m_renderer->ReleaseTexture(0);
	m_renderer->ReleaseTexture(1);

	m_renderer->ReleaseShader();
}

void SpriteRenderPath::RenderSceneForCamera(Camera * cam, RenderScene2D * scene)
{

	//now that we know which camera to render from, we can set it
	m_renderer->BindCamera(cam);			//could just use the global one

	//may want to clear color/depth, set based on the camera's settings
	// i.e., are we clearing to a color? skybox?
	//ClearBasedOnCameraOptions();

	//for (ParticleSystem* s : scene->m_particleSystems){
	//	s->PreRenderForCamera(cam);
	//}
	std::vector<DrawCall2D> drawCalls;
	//now we want to generate the draw calls
	for(Renderable2D* r : scene->m_renderables){
		//this will change for multi-pass shaders or multi-material meshes
		for (int i = 0; i < (int) r->m_mesh->m_subMeshes.size(); i++){
			if (r->m_mesh->m_subMeshes[i] != nullptr){
				DrawCall2D dc;
				//set up the draw call for this renderable :)
				// the layer/queue comes from the shader!
				dc.m_mesh = r->m_mesh->m_subMeshes[i];
				dc.m_model = r->m_transform.GetWorldMatrix();
				dc.m_material = r->GetEditableMaterial(i);
				dc.m_spriteLayer = r->GetZOrder();
				//dc.m_queue = 0;
				dc.m_yCoord = r->m_transform.GetWorldPosition().y;

				if (r->GetEditableMaterial(i)->UsesLights()){
					//compute most contributing lights based on renderable's position and puts them in the draw calls lights
					/*ComputeMostContributingLights(dc,
						r->GetPosition(), scene->m_lights);*/
				}
				drawCalls.push_back(dc);
			}
		}
	}

	//now we sort draw calls by layer/queue
	SortDrawCalls(drawCalls);
	//sort alpha by distance to camera, etc.

	for(DrawCall2D dc: drawCalls){
		//an optimization would be to only bind the thing if it's different from the previous bind.
		m_renderer->BindMaterial(dc.m_material);
		m_renderer->BindModel(dc.m_model);
		//m_renderer->BindLightUniforms(dc.m_lights);
		m_renderer->DrawMesh(dc.m_mesh);
	}
	TODO("Add post-processing to forward render path");
	////post-processing? it go here
	//for(Material * effect in cam-> m_effects){
	//	m_renderer->ApplyEffect( effect );
	//}

	//m_renderer->FinishEffects();
}

//void SpriteRenderPath::ComputeMostContributingLights(DrawCall & drawCall, const Vector3 & position, std::vector<Light*>& lights)
//{
//	if (lights.size() <= MAX_LIGHTS){
//		for(int i = 0; i < MAX_LIGHTS; i++){
//			if (i < lights.size()){
//				drawCall.m_lights[i] = lights[i];
//			}
//		}
//	} else {
//		//sort lights array by light factor
//
//		for (int i = 1; i < lights.size(); i ++){
//			bool sorted = false;
//			for (int j = i; j < lights.size(); j++){
//				sorted = true;
//				Light* light = lights[j];
//				Light* prevLight = lights[j-1];
//				if (GetLightFactor(light, position) > GetLightFactor(prevLight, position)){
//					//swap l and prevLight
//					lights[j] = prevLight;
//					lights[j-1] = light;
//					sorted = false;
//				}
//
//			}
//			if (sorted){
//				break;
//			}
//		}
//
//		//fill the drawcall's light array with the MAX_LIGHTS closest
//		for(int i = 0; i < MAX_LIGHTS; i++){
//			drawCall.m_lights[i] = lights[i];
//		}
//	}
//}

void SpriteRenderPath::SortDrawCalls(std::vector<DrawCall2D>& drawCalls)
{
	//sort by sort layer
	bool sorted = false;
	for (int i = 1; i < (int) drawCalls.size(); i ++){
		sorted = true;
		for (int j = 1; j < (int) drawCalls.size(); j++){
			
			DrawCall2D dc = drawCalls[j];
			DrawCall2D prevDC = drawCalls[j-1];
			if (dc.GetSpriteLayer() < prevDC.GetSpriteLayer()){		//sort layer is now 
				//swap l and prevLight
				std::swap(drawCalls[j], drawCalls[j-1]);
				//drawCalls[j] = prevDC;
				//drawCalls[j-1] = dc;
				sorted = false;
			}
			//sort alpha layer by distance to camera
			if (dc.GetSpriteLayer() >= 1 && prevDC.GetSpriteLayer() >= 1){
				//sort by distance to camera
				if (dc.m_yCoord > prevDC.m_yCoord){
					std::swap(drawCalls[j], drawCalls[j-1]);
					//drawCalls[j] = prevDC;
					//drawCalls[j-1] = dc;
					sorted = false;
				}
			}

		}
		if (sorted){
			break;
		}
	}
}

//float SpriteRenderPath::GetLightFactor(Light * l, Vector3 position)
//{
//	float dist = (l->GetPosition() - position).GetLength();
//	return dist;
//	//return l->GetAttenuation(dist);
//}
//
//void SpriteRenderPath::RenderSkybox(Camera * cam, RenderScene * scene)
//{
//	if (scene->m_skybox != nullptr){
//		scene->m_skybox->Update(cam);
//		//m_renderer->BindModel(cam->m_transform.GetWorldMatrix());
//		m_renderer->BindSkybox(scene->m_skybox, cam);
//	}
//}

