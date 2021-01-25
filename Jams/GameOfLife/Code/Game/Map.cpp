#include "Map.hpp"
#include "Game.hpp"
#include "Game/GameOfLife.hpp"


Map::~Map()
{

}

Map::Map(std::vector<IntVector2> liveCells) :
	m_gameOfLife(new GameOfLife(liveCells))
{
	m_renderable = new Renderable2D();
	m_autoTickWatch = StopWatch(GetMasterClock());
	m_autoTickWatch.SetTimer(m_autoTickRate);

	if (BOARD_BITS_X > 7) {
		m_renderBoard = false;
	}
}



void Map::Render()
{
	if (m_renderBoard) {
		RenderTiles();
	}
	else {
		RenderCellInformation();
	}
	
}

void Map::RenderTiles()
{
	delete m_renderable;
	m_renderable = new Renderable2D();
	//make mesh
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	static std::vector<Vertex3D_PCU> tileVerts;
	tileVerts.clear();
	Vector2 spacing = Vector2(1.f,1.f);
	int cellIndex = 0;
	for (int y = 0; y < BOARD_SIZE_X; y++) {
		for (int x = 0; x < BOARD_SIZE_Y; x++) {
			//cellIndex = m_gameOfLife->GetCellIndex(x, y);
			AABB2 bounds = AABB2(x, y, x + CELL_SIZE, y + CELL_SIZE);
			bounds.AddPaddingToSides(-CELL_SIZE * .05f, -CELL_SIZE * .05f);		//shrink for space between tiles
			RGBA color = m_gameOfLife->m_cells[cellIndex] ? RGBA::GREEN : RGBA::RED;
			mb.AppendPlane2D(bounds, color, AABB2::ZERO_TO_ONE, .01f);
			cellIndex++;
		}
	}
	mb.End();

	m_renderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_3DPCU));
	m_renderable->m_zOrder = 0;

	//render mesh
	g_theRenderer->BindMaterial(m_renderable->GetEditableMaterial());
	g_theRenderer->BindModel(m_renderable->m_transform.GetWorldMatrix());
	g_theRenderer->DrawMesh(m_renderable->m_mesh->m_subMeshes[0]);
}

void Map::RenderCellInformation()
{
	AABB2 UIBounds = g_theGame->SetUICamera();
	float padding = UIBounds.GetWidth() * 0.01f;
	UIBounds.AddPaddingToSides(-padding, -padding);
	AABB2 header;
	AABB2 body;
	UIBounds.SplitAABB2Horizontal(0.9f, header, body);
	std::string headerText = Stringf("tick rate: %f (change with z,x)", m_autoTickRate);
	g_theRenderer->DrawTextInBox2D(headerText, header, Vector2::TOP_LEFT, .01f, TEXT_DRAW_SHRINK_TO_FIT);
	std::string debugText = m_gameOfLife->GetDebugInformation();
	g_theRenderer->DrawTextInBox2D(debugText, body, Vector2::TOP_LEFT, .01f, TEXT_DRAW_SHRINK_TO_FIT);
}





void Map::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	//UpdateEntities(deltaSeconds);

	//RunPhysics();
	
	if (m_autoTick && m_autoTickWatch.DecrementAll()) {
		m_gameOfLife->Tick();
	}


}

int Map::GetWidth() const
{
	if (m_gameOfLife) {
		return BOARD_SIZE_X;
	}
	return 0;
}

int Map::GetHeight() const
{
	if (m_gameOfLife) {
		return BOARD_SIZE_Y;
	}
	return 0;
}

void Map::IncreaseAutoTickRate(const float increment)
{
	m_autoTickRate += increment;
	m_autoTickRate = ClampFloat(m_autoTickRate, 0.0016f, 2.f);
	m_autoTickWatch.SetTimer(m_autoTickRate);
}

