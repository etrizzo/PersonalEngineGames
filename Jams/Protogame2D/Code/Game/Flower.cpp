#include "Flower.hpp"

Flower::Flower(Vector2 position, float size)
{

	m_sizeRange = FloatRange(size - 1.f, size + 1.f);
	m_renderable = new Renderable2D();
	m_renderable->SetPosition(position);
	m_size = m_sizeRange.GetRandomInRange();
	m_center = position;
	m_cpuMesh = MeshBuilder();
	GenerateFlower();
	
}

Flower::~Flower()
{
}

void Flower::GenerateFlower()
{
	GetFlowerColors();
	//m_darkPetalColor = RGBA::BLACK;
	m_numPetals = GetRandomIntInRange(32, 52);
	m_size = m_sizeRange.GetRandomInRange();
	m_centerWedges = GetRandomIntInRange(14,24);
	m_cpuMesh.Clear();
	m_cpuMesh.Begin(PRIMITIVE_TRIANGLES, true);
	GenerateCenter();
	GeneratePetals();
	m_cpuMesh.End();
	delete m_renderable->m_mesh;
	m_renderable->m_mesh = m_cpuMesh.CreateMesh(VERTEX_TYPE_3DPCU);
	m_renderable->SetMaterial(Material::GetMaterial("default_unlit"));
}

void Flower::GetFlowerColors()
{
	float randomChance = GetRandomFloatZeroToOne();
	if (randomChance < 0.2f){
	//get some yellow ass flowers
		m_petalColor = RGBA::GetRGBAFromHSVWhereHueIsDegrees(GetRandomFloatInRange(27.f, 50.f), GetRandomFloatInRange(.95f, 1.0f), GetRandomFloatInRange(.8f, 1.f));
		if (CheckRandomChance(0.0f))
		{
			m_petalStripeColor = m_petalColor;
		} else {
			m_petalStripeColor = RGBA::GetRGBAFromHSV(GetRandomFloatInRange(0.f, .02f), GetRandomFloatInRange(.8f, 1.f), GetRandomFloatInRange(.4f, .6f));
			//m_petalStripeColor = RGBA::BLACK;
		}
		m_darkPetalColor = RGBA::GetRGBAFromHSV(GetRandomFloatInRange(.06f, .1f), GetRandomFloatInRange(.8f, 1.f), GetRandomFloatInRange(.3f, .5f));	//redder and lower value than base color
		m_centerColor = RGBA::GetRGBAFromHSV(GetRandomFloatInRange(0.01f, .05f), GetRandomFloatInRange(.8f, .95f), GetRandomFloatInRange(.2f, .4f));
	} else if (randomChance < .4f){
		//pink w/ yellow center
		m_petalColor = RGBA::GetRGBAFromHSVWhereHueIsDegrees(GetRandomFloatInRange(310.f, 360.f), GetRandomFloatInRange(.3f, 1.f), GetRandomFloatInRange(.8f, 1.f));
		if (CheckRandomChance(.8f))
		{
			m_petalStripeColor = m_petalColor;
		} else {
			m_petalStripeColor = RGBA::GetRandomPastelColor(.85f);
		}
		m_darkPetalColor = RGBA::GetRGBAFromHSVWhereHueIsDegrees(GetRandomFloatInRange(290, 310.f), GetRandomFloatInRange(.6f, 1.f), GetRandomFloatInRange(.5f, .6f));
		m_centerColor = RGBA::GetRGBAFromHSVWhereHueIsDegrees(GetRandomFloatInRange(36.f, 50.f), GetRandomFloatInRange(.8f, 1.f), GetRandomFloatInRange(.9f, 1.f));
	} else {
		//pure random!
		float hue = GetRandomFloatInRange(160.f, 420.f);		//get everything except green
		if (hue > 360.f){
			hue -= 360.f;
		}
		hue *= (1.f / 360.f);		//range back to 0-1
		float saturation = GetRandomFloatInRange(.5f, 1.f);
		float value = GetRandomFloatInRange(.5f, 1.f);
		m_petalColor = RGBA::GetRGBAFromHSV(hue, saturation, value);
		if (CheckRandomChance(.8f))
		{
			m_petalStripeColor = m_petalColor;
		} else {
			m_petalStripeColor = RGBA::GetRandomPastelColor(.85f);
		}
		m_darkPetalColor = RGBA::GetRGBAFromHSV(hue - GetRandomFloatInRange(0.f, .01f), saturation + GetRandomFloatInRange(0.f, .01f), value - GetRandomFloatInRange(.05f,.15f));	//get a darker, more saturated color
		
		float newHue = hue + s_goldenRatioConjugate;
		if (newHue > 1.f)
		{
			newHue -= 1.f;
		}

		m_centerColor = RGBA::GetRGBAFromHSV(newHue, GetRandomFloatInRange(.4f,.8f), GetRandomFloatInRange(.2f, .5f));
	}
}

void Flower::GenerateCenter()
{
	//float ratio = GetRandomFloatInRange(.3f, .4f);
	float ratio;
	if (CheckRandomChance(.5f))
	{
		//big boi
		ratio = GetRandomFloatInRange(.3f, .45f);
	} else {
		//little circle
		ratio = GetRandomFloatInRange(.15f,.3f);
	}
	int numTriangles = GetRandomIntInRange(200,500);
	
	float theta = GetRandomFloatInRange(0.f, 360.f);

	//generate the spiral pattern
	float step = s_goldenAngleDegrees;

	float triSize =  ratio * m_size * .2f;

	for (int i = 0; i < numTriangles; i++){
		float perc = (float) i / (float) numTriangles;
		Vector2 startPoint = PolarToCartesian(m_size * ratio * perc * .9f, theta) + m_center;

		//make a triangle starting at start point with 2 additional points determined by the current orientation.
		Vector2 dir1 = Vector2::MakeDirectionAtDegrees(theta);
		Vector2 dir2 = Vector2::MakeDirectionAtDegrees(theta  +( step * GetRandomFloatInRange(.95f,1.f)));
		
		
		dir1.NormalizeAndGetLength();
		dir2.NormalizeAndGetLength();
		
		dir1 *= (triSize * GetRandomFloatInRange(.8f, 1.f));
		dir2 *= (triSize * GetRandomFloatInRange(.8f, 1.f)) * .6f;
		Vector2 a1 = startPoint + dir1;
		Vector2 a2 = startPoint + dir2;

		m_cpuMesh.AppendTriangle(startPoint, a1, a2, RGBA::GetRandomMixedColor(m_centerColor, .8f));
		theta += step;
	}

	m_cpuMesh.AppendCircle(m_center, m_size * ratio * .95f, m_centerWedges, RGBA::GetRandomMixedColor(Interpolate(m_centerColor, RGBA::NICEBLACK, .1f), .8f));

	m_cpuMesh.AppendCircle(m_center, m_size * ratio, m_centerWedges, RGBA::GetRandomMixedColor(Interpolate(m_centerColor, RGBA::NICEBLACK, .45f), .8f));
	
}

void Flower::GeneratePetals()
{
	float orientation = GetRandomFloatInRange(0.f, 180.f);
	Vector2 forward = Vector2::MakeDirectionAtDegrees(orientation);
	//RGBA goodYellow = RGBA(240, 200, 60);
	for (int i = 0; i < m_numPetals; i++)
	{
		float perc = (float) i / (float) m_numPetals;
		RGBA interpColor = Interpolate(m_petalColor, m_darkPetalColor, SmoothStep3(perc));
		AddPetal(RGBA::GetRandomMixedColor(interpColor, .85f), forward);
		//AddStripedPetal(RGBA::GetRandomMixedColor(interpColor, .85f), forward);
		orientation+= s_goldenAngleDegrees;
		forward = Vector2::MakeDirectionAtDegrees(orientation);
	}

}

void Flower::AddPetal(RGBA baseColor, Vector2 forward)
{
	Vector2 right = Vector2(-forward.y, forward.x);
	Vector2 start = m_center;
	Vector2 midpoint = m_center + forward * .5f;
	Vector2 a1 = midpoint + (right * .25f);
	Vector2 a2 = midpoint - (right * .25f);

	m_cpuMesh.AppendTriangle(start, a1, a2, RGBA::GetRandomMixedColor(baseColor, .8f));

	start = a2;
	a1 = a1;
	a2 = a1 + (forward * .5f);
	RGBA color = RGBA::GetRandomMixedColor(baseColor, .8f);

	Vector2 tinyRight = right * .05f;

	RGBA borderColor = RGBA::GetRandomMixedColor(baseColor, .8f);
	borderColor = Interpolate(borderColor, RGBA::NICEBLACK, .4f);

	for (int i = 0; i < (int) m_size; i++){
		float perc = (float) i / (float) m_size;
		float nextPerc = (float) (i + 1) / (float) m_size;
		Vector2 minsWidth = tinyRight * InAndOut2(perc);
		Vector2 maxsWidth = tinyRight * InAndOut2(nextPerc);

		// chance to change color on each new tri
		if (CheckRandomChance(.8f)){
			color = RGBA::GetRandomMixedColor(baseColor, .8f);
		}

		//first tri
		m_cpuMesh.AppendTriangle(start, a1, a2, color);
		
		// chance to change color on each new tri
		if (CheckRandomChance(.8f)){
			color = RGBA::GetRandomMixedColor(baseColor, .8f);
		}
		//second tri
		start = start;
		a1 = a2;
		a2 = a1 - (right * .5f);
		m_cpuMesh.AppendTriangle(start, a1, a2, color);

		//add a line - right
		Vector2 rightBorderMins = start + (right * .5f);
		Vector2 rightBorderMaxs = a1 + maxsWidth;
		m_cpuMesh.AppendPlane(Vector3(rightBorderMins), Vector3(rightBorderMins + minsWidth), Vector3(a1), Vector3(rightBorderMaxs), RGBA::GetRandomMixedColor(borderColor, .8f), Vector2::ZERO, Vector2::ONE);


		//add a line - left
		Vector2 leftBorderMins = start - minsWidth;
		Vector2 leftBorderMaxs = a2;
		m_cpuMesh.AppendPlane(Vector3(leftBorderMins), Vector3(start), Vector3(leftBorderMaxs - maxsWidth), Vector3(leftBorderMaxs), RGBA::GetRandomMixedColor(borderColor, .8f), Vector2::ZERO, Vector2::ONE);


		//set up for next loop
		start = a2;
		a1 = a1;
		a2 = a1 + (forward * .5f);
	}

	a2 = a2 - (right * .25f);
	m_cpuMesh.AppendTriangle(start, a1, a2, RGBA::GetRandomMixedColor(baseColor, .8f));
}

void Flower::AddStripedPetal(RGBA baseColor, Vector2 forward)
{
	float quadRatio = .25f;
	Vector2 right = Vector2(-forward.y, forward.x);
	Vector2 start = m_center;
	Vector2 midpoint = m_center + forward * quadRatio;
	Vector2 a1 = midpoint + (right * quadRatio * .5f);
	Vector2 a2 = midpoint - (right * quadRatio * .5f);

	m_cpuMesh.AppendTriangle(start, a1, a2, RGBA::GetRandomMixedColor(baseColor, .8f));

	Vector2 miniRight = right * quadRatio;

	start = a2 - (miniRight * .5f);
	a1 = a1- (miniRight * .5f);
	a2 = a1 + (forward * quadRatio) - (miniRight * .5f);
	RGBA color = RGBA::GetRandomMixedColor(baseColor, .8f);	//the color of this triangle

	

	float numTriangles = m_size * (.5f / quadRatio);

	for (int i = 0; i < (int) numTriangles; i++){
		float perc = (float) i / ( numTriangles) ;

		// chance to change color on each new tri
		if (CheckRandomChance(.8f)){
			color = RGBA::GetRandomMixedColor(baseColor, .8f);
		}
		color = Interpolate(color, m_petalStripeColor, InAndOut2(perc + .2f) * .8f);

		//first tri
		m_cpuMesh.AppendTriangle(start, a1, a2, color);
		m_cpuMesh.AppendTriangle(start + miniRight, a1 + miniRight, a2 + miniRight, RGBA::GetRandomMixedColor(color, .8f));

		// chance to change color on each new tri
		if (CheckRandomChance(.8f)){
			color = RGBA::GetRandomMixedColor(baseColor, .8f);
		}
		color = Interpolate(color, m_petalStripeColor, InAndOut2(perc + .2f) * .8f);
		//second tri
		start = start;
		a1 = a2;
		a2 = a1 - (right * quadRatio);
		m_cpuMesh.AppendTriangle(start, a1, a2, color);
		m_cpuMesh.AppendTriangle(start + miniRight, a1 + miniRight, a2 + miniRight, RGBA::GetRandomMixedColor(color, .8f));

		//set up for next loop
		start = a2;
		a1 = a1;
		a2 = a1 + (forward * quadRatio);
	}

	a2 = a2 - (right * quadRatio * .5f);
	m_cpuMesh.AppendTriangle(start, a1, a2, RGBA::GetRandomMixedColor(baseColor, .8f));
}

void Flower::AddRoundedPetal(RGBA baseColor, Vector2 forward)
{
}


