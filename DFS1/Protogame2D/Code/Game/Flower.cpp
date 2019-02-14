#include "Flower.hpp"

Flower::Flower(Vector2 position, float size)
{
	
	m_renderable = new Renderable2D();
	m_renderable->SetPosition(position);
	m_size = size;
	m_center = position;
	m_cpuMesh = MeshBuilder();
	GenerateFlower();
	
}

Flower::~Flower()
{
}

void Flower::GenerateFlower()
{
	m_petalColor = RGBA::GetRGBAFromHSV(GetRandomFloatInRange(.105f, .16f), GetRandomFloatInRange(.6f, 1.f), GetRandomFloatInRange(.9f, 1.f));
	m_darkPetalColor = RGBA::GetRGBAFromHSV(GetRandomFloatInRange(.08f, .13f), GetRandomFloatInRange(.8f, 1.f), GetRandomFloatInRange(.3f, .5f));	//redder and lower value than base color
	//m_darkPetalColor = RGBA::BLACK;
	m_numPetals = GetRandomIntInRange(32, 64);
	m_size = GetRandomFloatInRange(4.f, 7.f);
	m_centerWedges = GetRandomIntInRange(12,20);
	m_cpuMesh.Clear();
	m_cpuMesh.Begin(PRIMITIVE_TRIANGLES, true);
	GenerateCenter();
	GeneratePetals();
	m_cpuMesh.End();
	delete m_renderable->m_mesh;
	m_renderable->m_mesh = m_cpuMesh.CreateMesh(VERTEX_TYPE_3DPCU);
	m_renderable->SetMaterial(Material::GetMaterial("default_unlit"));
}

void Flower::GenerateCenter()
{
	float ratio = GetRandomFloatInRange(.3f, .4f);
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


	m_cpuMesh.AppendCircle(m_center, m_size * ratio, m_centerWedges, RGBA::GetRandomMixedColor(Interpolate(m_centerColor, RGBA::BLACK, .1f), .8f));


}

void Flower::GeneratePetals()
{
	float orientation = GetRandomFloatInRange(0.f, 180.f);
	Vector2 forward = Vector2::MakeDirectionAtDegrees(orientation);
	//RGBA goodYellow = RGBA(240, 200, 60);
	for (int i = 0; i < m_numPetals; i++)
	{
		float perc = (float) i / (float) m_numPetals;
		RGBA interpColor = Interpolate(m_petalColor, m_darkPetalColor, SmoothStart3(perc));
		AddPetal(RGBA::GetRandomMixedColor(interpColor, .85f), forward);
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

	Vector2 tinyRight = right * .2f;

	for (int i = 0; i < (int) m_size; i++){
		float perc = (float) i / (float) m_size;

		if (CheckRandomChance(.8f)){
			color = RGBA::GetRandomMixedColor(baseColor, .8f);
		}

		//first tri
		m_cpuMesh.AppendTriangle(start, a1, a2, color);
		
		if (CheckRandomChance(.8f)){
			color = RGBA::GetRandomMixedColor(baseColor, .8f);
		}
		//second tri
		start = start;
		a1 = a2;
		a2 = a1 - (right * .5f);
		m_cpuMesh.AppendTriangle(start, a1, a2, color);

		//set up for next loop
		start = a2;
		a1 = a1;
		a2 = a1 + (forward * .5f);
	}

	a2 = a2 - (right * .25f);
	m_cpuMesh.AppendTriangle(start, a1, a2, RGBA::GetRandomMixedColor(baseColor, .8f));
}


