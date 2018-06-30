#include "Light.hpp"
#include "Engine/Math/Renderer.hpp"

Light::Light()
{
	m_transform = Transform();
	m_lightColor = RGBA(0,0,0,0);
}

Light::Light(Vector3 pos, RGBA color)
{
	m_transform = Transform();
	m_transform.SetLocalPosition(pos);
	m_lightColor = color;
}

Light::Light(Vector3 pos, Vector4 color)
{
	m_transform = Transform();
	m_transform.SetLocalPosition(pos);
	m_lightColor;
	m_lightColor.SetAsFloats(color.x, color.y, color.z, color.w);
}

void Light::Translate(Vector3 translation)
{
	m_transform.TranslateLocal(translation);
}

void Light::SetPosition(Vector3 newPos)
{
	m_transform.SetLocalPosition(newPos);
}

void Light::SetAttenuation(Vector3 attenuation)
{
	m_attenuation = attenuation;
}

float Light::GetAttenuation(float distance) const
{
	return m_lightColor.GetNormalized().w / (m_attenuation.x + distance * m_attenuation.y + distance * distance * m_attenuation.z);
}

void Light::SetTransform(Transform newTransform)
{
	m_transform = newTransform;
}

void Light::SetColor(RGBA newColor)
{
	m_lightColor = newColor;
}

void Light::SetColor(Vector4 newColor)
{
	m_lightColor.SetAsFloats(newColor.x, newColor.y, newColor.z, newColor.w);
}

void Light::RenderAsPoint(Renderer * r)
{
	RGBA color = m_lightColor.GetColorWithAlpha(100);
	r->DrawPoint(GetPosition(), 1.f, m_transform.GetRight(), m_transform.GetUp(), m_transform.GetForward(), color);
	//draw some spindlys
	Vector3 right2 = (m_transform.GetRight() + m_transform.GetForward() + m_transform.GetUp()).GetNormalized();
	Vector3 up2 = (-1.f * m_transform.GetRight() + -1.f * m_transform.GetForward() + m_transform.GetUp()).GetNormalized();
	Vector3 forward2 = Cross(right2, up2);
	r->DrawPoint(GetPosition(), 1.f, right2, up2, forward2, color);
	//
	Vector3 right3 = (m_transform.GetRight() - m_transform.GetForward() - m_transform.GetUp()).GetNormalized();
	Vector3 up3 = (-1.f * m_transform.GetRight() + -1.f * m_transform.GetForward() + -1.f *  m_transform.GetUp()).GetNormalized();
	Vector3 forward3 = Cross(right3, up3);
	r->DrawPoint(GetPosition(), 1.f, right3, up3, forward3, color);
	
}

light_t Light::GetBuffer() const
{
	light_t buffer;
	

	buffer.color = m_lightColor.GetNormalized();
	buffer.pos	 = m_transform.GetWorldPosition();
	buffer.usesShadows = 0.f;
	if (m_createsShadow){
		buffer.usesShadows = 1.f;
	} else {
		buffer.usesShadows = 0.f;
	}
	
	buffer.direction		= m_transform.GetForward();				//for a point light, this isn't really used in the final value, but still used in calculations so don't fuck it!
	buffer.is_point_light	= 1.f;		//is a directional light = 0.f? or point light = 1.f?
	
	buffer.attenuation		= m_attenuation;		//heck with this
	buffer.dot_inner_angle	= CosDegreesf(180.f);			//for cone light;
	
	buffer.spec_attenuation = Vector3(.2f,0.f,0.01f);;		//if you want it to be different :)
	buffer.dot_outer_angle	= CosDegreesf(180.f);		//for cone light

	if (m_createsShadow){
		buffer.shadowVP = m_shadowVP;
	}

	return buffer;
}

void Light::SetShadowMatrix(const Matrix44 & mat)
{
	m_shadowVP = mat;
}

DirectionalLight::DirectionalLight(Vector3 pos, RGBA color, Vector3 rotation)
	:Light(pos, color)
{
	m_transform.RotateByEuler(rotation);
	m_attenuation = Vector3(1.f, 0.f, 0.f);		//default directional attenuation to not falloff
}

light_t DirectionalLight::GetBuffer() const
{
	light_t buffer;

	buffer.color = m_lightColor.GetNormalized();
	buffer.pos	 = m_transform.GetWorldPosition();
	buffer.usesShadows = 0.f;
	if (m_createsShadow){
		buffer.usesShadows = 1.f;
	} else {
		buffer.usesShadows = 0.f;
	}

	buffer.direction		= m_transform.GetForward();				//for a point light, this isn't really used in the final value, but still used in calculations so don't fuck it!
	buffer.is_point_light	= 0.f;						//is a directional light = 0.f? or point light = 1.f?

	buffer.attenuation		= m_attenuation;		//heck with this
	buffer.dot_inner_angle	= CosDegreesf(180.f);			//for cone light;

	buffer.spec_attenuation = Vector3(.2f,0.f,0.01f);;		//if you want it to be different :)
	buffer.dot_outer_angle	= CosDegreesf(180.f);		//for cone light

	if (m_createsShadow){
		buffer.shadowVP = m_shadowVP;
	}

	return buffer;
}

void DirectionalLight::RenderAsPoint(Renderer * r)
{
	RGBA color = m_lightColor.GetColorWithAlpha(100);
	Plane p = Plane(m_transform.GetWorldPosition(), m_transform.GetUp(), m_transform.GetRight(),Vector2(.5f,.5f) );
	r->UseShader("wireframe");
	r->DrawPlane(p, color);
	r->DrawLine3D(m_transform.GetWorldPosition(), m_transform.GetWorldPosition() + m_transform.GetForward(), color, color);
	r->ReleaseShader();
}

SpotLight::SpotLight(Vector3 pos, RGBA color, float innerAngleDegrees, float outerAngleDegrees)
	:Light(pos, color)
{
	m_innerAngleDot = CosDegreesf(innerAngleDegrees);
	m_outerAngleDot = CosDegreesf(outerAngleDegrees);
}

void SpotLight::LookAt(const Vector3 & position, const Vector3 & target, const Vector3 & up)
{
	
	m_transform.LookAt(position, target, up);
}

light_t SpotLight::GetBuffer() const
{
	light_t buffer;

	buffer.color = m_lightColor.GetNormalized();
	buffer.pos	 = m_transform.GetWorldPosition();
	if (m_createsShadow){
		buffer.usesShadows = 1.f;
	} else {
		buffer.usesShadows = 0.f;
	}

	buffer.direction		= m_transform.GetForward();				//for a point light, this isn't really used in the final value, but still used in calculations so don't fuck it!
	buffer.is_point_light	= 0.f;						//is a directional light = 0.f? or point light = 1.f?

	buffer.attenuation		= m_attenuation;		//heck with this
	buffer.dot_inner_angle	= m_innerAngleDot;			//for cone light;

	buffer.spec_attenuation = Vector3(.2f,0.f,0.01f);;		//if you want it to be different :)
	buffer.dot_outer_angle	= m_outerAngleDot;		//for cone light

	if (m_createsShadow){
		buffer.shadowVP = m_shadowVP;
	}

	return buffer;
}

void SpotLight::RenderAsPoint(Renderer * r)
{
	RGBA color = m_lightColor.GetColorWithAlpha(100);
	TODO("draw a cone lol");
	r->UseShader("wireframe");
	r->DrawCube(m_transform.GetWorldPosition(), Vector3::ONE * .3f, color);
	r->DrawLine3D(m_transform.GetWorldPosition(), m_transform.GetWorldPosition() + m_transform.GetForward(), color, color);
	r->ReleaseShader();
}
