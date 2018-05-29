//-----------------------------------------------------------------------------------------------
// Texture.hpp
//
#pragma once
#include "Engine/Math/IntVector2.hpp"

#include <string>
#include <map>

class Image;

enum eTextureFormat 
{
	TEXTURE_FORMAT_RGBA8, // default color format
	TEXTURE_FORMAT_D24S8, 
	NUM_TEXTURE_FORMATS
}; 

const std::string IMAGE_DIRECTORY = "Data/Images/";


//---------------------------------------------------------------------------
class Texture
{
	friend class Renderer; // Textures are managed by a Renderer instance

private:
	Texture(){};
	Texture( const std::string& imageFilePath ); // Use renderer->CreateOrGetTexture() instead!
	Texture( const Image* image);
	void PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents );
	void PopulateFromImage( const Image* image );
	bool CreateRenderTarget(unsigned int width, unsigned int height, eTextureFormat format);

public:
	unsigned int GetHandle() const { return m_textureID; };
	unsigned int GetWidth()	 const { return m_dimensions.x; };
	unsigned int GetHeight() const { return m_dimensions.y; };
	IntVector2 GetDimensions() const { return m_dimensions; };
	std::string GetPath() const { return m_path; };

	static Texture* CreateCompatible(Texture* src);

	static std::map<std::string, Texture*> s_loadedTextures;
	static Texture* CreateOrGetTexture(const std::string& texPath, const std::string& directory = IMAGE_DIRECTORY);
	static void RegisterTextureFromImage(const std::string& name, Image* img);

private:
	unsigned int	m_textureID;
	IntVector2		m_dimensions;
	eTextureFormat	m_format;	
	std::string		m_path;
};




