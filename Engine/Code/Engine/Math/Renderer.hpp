#pragma once
//#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
//#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <cassert>
#include <crtdbg.h>


//#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#include <time.h>
#include <stdlib.h>
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

#include "Engine/Renderer/external/gl/glcorearb.h"
#include "Engine/Renderer/external/gl/glext.h"
#include "Engine/Renderer/external/gl/wglext.h"
#include <string>
#include <vector>

#include "Engine/Renderer/glfunctions.h"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/MatrixStack.hpp"
#include "Engine/Renderer/Skybox.hpp"



// Uniform bindings
#define LIGHT_BUFFER_BINDING	(2)
#define TIME_BUFFER_BINDING		(8)
#define SIZE_BUFFER_BINDING		(9)

typedef RenderBuffer UniformBuffer;


struct size_data_t
{
	float m_texelSizeX;
	float m_texelSizeY;
	float m_screenSizeX;
	float m_screenSizeY;
};


const std::string FONT_DIRECTORY = "Data/Fonts/";
const std::string SHADER_DIRECTORY = "Data/Shaders/";

const std::string FONT_EXTENSION = ".png";



class Shader;
class ShaderProgram;
class Sampler;
class Camera;
class FrameBuffer;
class Sprite;
class SubMesh;
class Light;
class Material;

class Renderer{
public:
	~Renderer();
	Renderer();

	std::vector<Texture*> m_loadedTextures;
	std::vector<std::string> m_loadedTexturePaths;
	std::map<std::string, BitmapFont*> m_loadedFonts;
	std::map<std::string, ShaderProgram*> m_loadedShaderPrograms;
	std::map<std::string, Sprite*> m_loadedSprites;

	Shader* m_defaultShader;
	Shader* m_invalidShader;
	Shader* m_currentShader;

	GLuint m_defaultVAO;
	SubMesh* m_immediateMesh;

	Camera* m_defaultCamera = nullptr;
	Camera* m_currentCamera  = nullptr;
	Camera* m_effectCamera = nullptr;

	Texture* m_defaultColorTarget ;
	Texture* m_defaultDepthTarget ;

	std::vector<Sampler*> m_samplers = std::vector<Sampler*>();
	Texture* m_defaultTexture = nullptr;		//a 2x2 white square

	RGBA m_ambientLight;
	MatrixStack m_modelMatrix;

	bool Initialize();

	void BindNewWGLFunctions();
	void BindGLFunctions();
	RECT CalculateClientWindow(float clientAspect);
	void SetWindowSize(Window* theWindow);

	GLenum GetGLPrimitiveType(eDrawPrimitiveType primType);
	void DrawMeshImmediate(  Vertex3D_PCU* verts, int numVerts, eDrawPrimitiveType drawPrimitive, const int* indices = nullptr, int numIndices = 0);
	void BindStateAndDrawMesh(SubMesh* mesh);		//was draw mesh
	void DrawMesh(SubMesh* mesh);

	void BeginFrame(const Vector2 & bottomLeft, const Vector2 & topRight, RGBA color);
	void BeginFrame(const Vector3 & nearBottomLeft, const Vector3 & farTopRight, RGBA color);
	void BindFrameUniforms();
	void UpdateClock(float gameDS, float systemDS);


	void BindUniform(std::string uniformName, const float& value);

	void BindRendererUniforms();
	void BindRenderState(RenderState state);
	void BindMeshToProgram(ShaderProgram* program, SubMesh* mesh);
	void BindLightUniforms(Light* light);
	void BindLightUniforms(std::vector<Light*> light);
	void BindLightUniforms(Light* lights[MAX_LIGHTS]);
	void BindSurfaceUniforms(float specAmount, float specFactor);
	void BindAmbientLight();

	void BindMaterial(Material* mat);

	void DrawSkybox(Skybox* skybox);

	//should maybe be removed?
	void BindProjection();
	void BindCamera(Camera* cam);		//binds projection for this camera

	void BindModel(Matrix44 mat, Shader* shader = nullptr);

	void SetAmbientLight(RGBA color);

	void DrawLine2D(const Vector2 start, const Vector2 end, const RGBA startColor = RGBA(), const RGBA endColor = RGBA() , float lineThickness=1.5f);
	void DrawDottedLine2D(const Vector2 start, const Vector2 end, const RGBA color = RGBA(), float dotLength = .5f, float lineThickness = 1.5f);
	
	void DrawShapeFromVertices2(Vector2 vertices[], int numVertices, RGBA color = RGBA(), int numInArray = -1);
	
	void DrawRegularPolygon2(int numSides, Vector2 pos = Vector2(0.f,0.f), float radius = 1.f, float rotation = 0.f, RGBA color = RGBA(),bool dottedLine = false);
	void DrawCircleArc(float percentage = 1.f, int numSides = 20, Vector2 pos = Vector2(0.f, 0.f), float radius =1.f, float rotation = 0.f, RGBA color = RGBA()); 
	void DrawSquigglyLine(const Vector2 start, const Vector2 end, const float time = -1.f, RGBA color = RGBA());
	
	void DrawTriangle(const Vector2 vertices[], const RGBA& color);
	void DrawAABB2(const AABB2& bounds, const RGBA& color,  const Vector2& uvMins = Vector2::ZERO, const Vector2& uvMaxs = Vector2::ONE);
	void DrawOBB2(const OBB2& bounds, const RGBA& color, const Vector2& uvMins = Vector2::ZERO, const Vector2& uvMaxs = Vector2::ONE);
	void DrawDisc2(const Vector2& center, const float& radius, const RGBA& color, int segments = 10);
	void DrawBlendedAABB2(const AABB2& bounds, const RGBA& topLeftColor, const RGBA& bottomLeftColor, const RGBA& topRightColor, const RGBA& bottomRightColor);
	void DrawAABB2Outline(const AABB2& bounds, const RGBA& color);
	void DrawTexturedAABB2( const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const RGBA& tint);
	
	void DrawCube(const Vector3& center, const Vector3& dimensions, const RGBA& color = RGBA::WHITE, const Vector3& right = Vector3::X_AXIS, const Vector3& up = Vector3::Y_AXIS, const Vector3& forward = Vector3::Z_AXIS, AABB2 uvs = AABB2::ZERO_TO_ONE);
	void DrawCube(const Vector3& center, const Vector3& dimensions, const RGBA& color, const Vector3& right, const Vector3& up, const Vector3& forward, AABB2 UV_TOP, AABB2 UV_SIDE, AABB2 UV_BOTTOM = AABB2::ZERO_TO_ONE);
	void DrawSphere(const Vector3& center, const float& radius, const int& wedges, const int& slices, const RGBA& color = RGBA::WHITE, const AABB2& uvs = AABB2::ZERO_TO_ONE);
	void DrawSphere(const Sphere& sphere, const RGBA& color = RGBA::WHITE, const AABB2& uvs = AABB2::ZERO_TO_ONE);
	void DrawPlane(const Vector3& nearBottomLeft, const Vector3& nearBottomRight, const Vector3& farTopRight, const Vector3& farTopLeft, const RGBA& color = RGBA::WHITE, Vector2 uvMins = Vector2::ZERO, Vector2 uvMaxs = Vector2::ONE);
	void DrawPlane(const Plane& plane, const RGBA& color = RGBA::WHITE, const Vector2& uvMins= Vector2::ZERO, const Vector2& uvMaxs= Vector2::ONE);
	void DrawLine3D(const Vector3& start, const Vector3& end, const RGBA& startColor = RGBA::WHITE, const RGBA& endColor = RGBA::WHITE, float width = 1.f);
	void DrawPoint(const Vector3& pos,float size, const Vector3& right, const Vector3& up, const Vector3& forward, RGBA rightColor , RGBA upColor, RGBA forwardColor);
	void DrawPoint(const Vector3& pos, float size = 1.f, const Vector3& right = Vector3::X_AXIS, const Vector3& up = Vector3::Y_AXIS, const Vector3& forward = Vector3::Z_AXIS, const RGBA& color = RGBA::WHITE);

	//scales the box provided to the sprites dimensions
	AABB2 DrawSpriteOnAABB2(const AABB2& box, Sprite* sprite, RGBA tint = RGBA::WHITE);
	void DrawSprite(const Vector3& pos, Sprite* sprite, const Vector3& right = Vector3::X_AXIS, const Vector3& up = Vector3::Y_AXIS, RGBA tint = RGBA::WHITE);
	void DrawTextAsSprite(const std::string& asciiText, const Vector3& pos, const Vector2 & pivot = Vector2::HALF, float cellHeight = 1.f, const Vector3& right = Vector3::X_AXIS, const Vector3& up = Vector3::Y_AXIS, RGBA tint = RGBA::WHITE, const BitmapFont* font = nullptr, float aspect = 1.f);

	void BindTextureCube(const TextureCube& cube, unsigned int textureIndex = 8U);
	void BindTexture(const std::string& fileName, unsigned int textureIndex = 0U, const std::string& directory = IMAGE_DIRECTORY);
	void BindTexture( const Texture& texture, unsigned int textureIndex = 0U);
	void BindSampler( const Sampler& sampler, unsigned int samplerINdex = 0U);
	void BindDefaultTexture(unsigned int textureIndex = 0U);
	void ReleaseTexture(unsigned int textureIndex = 0U);
	


	void ClearScreen( const RGBA& clearColor);
	void SetOrtho(const Vector3& nearBottomLeft, const Vector3& farTopRight);
	void SetOrtho(const Vector2& bottomLeft, const Vector2& topRight);
	void SetProjectionMatrix( Matrix44 const& projection);
	//void SetViewMatrix(Matrix44 const& viewMatrix);


	void UseShader(std::string shaderID);
	void BindShader(Shader* shader);
	void ReleaseShader();

	void BindShaderProgram(std::string shaderRoot);
	void UseShaderProgram(ShaderProgram* shader);
	void ReleaseShaderProgram();		//resets to default shader program

	void ApplyEffect(std::string shaderRoot, const AABB2& uvs = AABB2::ZERO_TO_ONE);
	void ApplyEffect(ShaderProgram* program, const AABB2& uvs = AABB2::ZERO_TO_ONE);		//backwards compatibility
	void ApplyEffect(Material* material, const AABB2& uvs = AABB2::ZERO_TO_ONE);
	void FinishEffects();

	void EnableDepth( eCompare compare, bool should_write );
	void DisableDepth();
	void ClearDepth(float depth = 1.f);
	
	void EndFrame(HDC displayContext);
	bool CopyFrameBuffer( FrameBuffer* dst, FrameBuffer* src);
	
	void Pop();
	void Push();

	void Pop2();
	void Push2();
	
	void Transform2(Vector2 position, float rotation, float scale);
	void Transform(Matrix44 mat);
	void Transform(Vector3 position, Vector3 rotation, float scale);
	void Translate2(Vector2 position);
	void Translate(Vector3 position);
	void Rotate2(float rotation);
	void Rotate(Vector3 rotation);
	void Scale(float scale);
	void Scale2(float scale);			//maybe not necessary
	
	void PushAndTransform2(Vector2 position, float rotation, float scale);
	void PushAndTransform(Vector3 position, Vector3 rotation, float scale);
	void PushAndTransform(Matrix44 mat);
	
	void LoadSpritesFromSpriteAtlas(SpriteSheet* spriteSheet, int PPU, Vector2 pivot = Vector2::HALF);
	void LoadShadersFromFile(std::string fileName);
	void LoadMaterialsFromFile(std::string fileName);

	//name is the id used in the map - "spritesheetPath_index" i.e. Tiles8x8_14
	Sprite* CreateOrGetSprite(const std::string& name, const Texture* texture, AABB2 uvs, Vector2 pivot, Vector2 dimensions);		
	Sprite* GetSprite(const std::string& name);
	Texture* CreateOrGetTexture(const std::string& path, const std::string& directory = IMAGE_DIRECTORY, bool setMips = false);

	BitmapFont* CreateOrGetBitmapFont( const char* bitmapFontName);
	void DrawText2D(const std::string& asciiText, const Vector2& drawMins, float cellHeight, const RGBA& tint = RGBA::WHITE, const BitmapFont* font = nullptr, float aspectScale = 1.f);
	void DrawText3D(const std::string& asciiText, const Vector3& drawMins, float cellHeight, const Vector3& up = Vector3::Y_AXIS, const Vector3& right = Vector3::X_AXIS, const RGBA& tint = RGBA::WHITE, const BitmapFont* font = nullptr, float aspectScale = 1.f);
	//returns the text width
	float DrawTextInBox2D(const std::string& asciiText, AABB2 box, Vector2 alignment, float cellHeight, eTextDrawMode drawMode = TEXT_DRAW_OVERRUN, const RGBA& tint = RGBA::WHITE, const BitmapFont* font = nullptr, float aspectScale = 1.f);
	float GetTextWidth(const std::string& asciiText, float cellHeight, float aspectScale = 1.f);

	void SetCamera(Camera* newCamera);
	Texture* CreateRenderTarget (unsigned int width, unsigned int height, eTextureFormat format = TEXTURE_FORMAT_RGBA8);
	Texture* CreateDepthStencilTarget(unsigned int width, unsigned int height);

	ShaderProgram* CreateOrGetShaderProgram(const std::string& shaderRoot);
	ShaderProgram* CreateOrGetShaderProgram(const std::string& vsRoot, const std::string& fsRoot);
	ShaderProgram* CreatePreDefinedShaderProgram(const std::string& rootName, const char*  vertexShader, const char*  fragmentShader);
	void RecompileAllShaders();

	void SetDepth(eCompare compare, bool should_write );
	void SetFillMode(eFillMode fill = FILL_MODE_FILL);
	void SetCullMode(eCullMode cull = CULLMODE_BACK);
	void SetWindOrder(eWindOrder wind = WIND_COUNTER_CLOCKWISE);
	void SetBlendMode(eBlendMode blend);
	void SetAdditiveBlending();
	void SetAlphaBlending();
	void SetMultiplyBlending();
	void DisableBlending();

private:
	time_data_t m_timeData;
	UniformBuffer m_timeBuffer;

	light_buffer_t m_lightData;
	UniformBuffer m_lightBuffer;

	size_data_t m_sizeData;
	UniformBuffer m_sizeBuffer;


	Texture* m_effectTarget  = nullptr;
	Texture* m_effectScratch = nullptr;
	FrameBuffer* m_copyFBO = nullptr;
	FrameBuffer* m_copyFBOScratch = nullptr;
	
	Vector2 m_texelSize;
	Vector2 m_windowSizeTexels;

	void PostStartup();
	
	void CopyTexture(Texture* dst, Texture* src);

	void WrapLines( Strings& lines, AABB2 box, float cellHeight, float aspectScale);
	float FindScaleToFit(Strings lines, AABB2 box, float cellHeight, float aspectScale);

	float DrawTextLines(Strings lines, AABB2 box, Vector2 alignment, float cellHeight, const RGBA& tint, const BitmapFont* font, float aspectScale);
	
	//helper function for draw cube - adds vertices for each face of the cube. Same idea as DrawPlane but adds it to a vertex array to do in one drawCall, hopefully
	void AddFaceToCube(const Vector3& mins, const Vector3& maxs, Vertex3D_PCU (& vertices)[36], unsigned int& startIndex, const RGBA& color = RGBA::WHITE, Vector2 uvMins = Vector2::ZERO, Vector2 uvMaxs = Vector2::ONE);
	void AddFaceToCube(const Vector3& nearBottomLeft, const Vector3& nearBottomRight, const Vector3& farTopRight, const Vector3& farTopLeft, Vertex3D_PCU (& vertices)[36], unsigned int& startIndex, const RGBA& color = RGBA::WHITE, Vector2 uvMins = Vector2::ZERO, Vector2 uvMaxs = Vector2::ONE);

};

static HGLRC CreateOldRenderContext(HDC hdc);
static HGLRC CreateRealRenderContext(HDC hdc, int major, int minor);

static GLenum ToGLCompare( eCompare compare ) ;
static GLenum ToGLType( eRenderDataType type);
