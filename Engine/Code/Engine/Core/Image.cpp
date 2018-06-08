#include "Image.hpp"

#include "Engine/Renderer/Texture.hpp"
//#include "ThirdParty/stb/stb_image.c"
#include "ThirdParty/stb/stb_image.h"



Image::Image(const std::string & imageFilePath)
{

	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)

									// Load (and decompress) the image RGB(A) bytes from a file on disk, and create an OpenGL texture instance from it
	unsigned char* imageData = stbi_load( imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested );

	ASSERT_OR_DIE(imageData != nullptr, "No image found: " + imageFilePath);

	if(numComponents == 1){
		PopulateFromR8(imageData, m_dimensions);
	}

	if (numComponents == 3){
		PopulateFromRGB(imageData, m_dimensions);
	}
	if (numComponents == 4){
		PopulateFromRGBA(imageData, m_dimensions);
	}
	//FlipY();
	stbi_image_free( imageData );
}

Image::Image( std::vector<RGBA> texels, IntVector2 dimensions)
{
	m_texels = texels;
	m_dimensions = dimensions;
}

RGBA Image::GetTexel(int x, int y) const
{
	int texelIndex = GetIndexFromCoordinates(x,m_dimensions.y - y - 1,m_dimensions.x, m_dimensions.y);
	return m_texels[texelIndex];
}

RGBA Image::GetTexel(int idx) const
{
	return m_texels[idx];
}

void Image::SetTexel(int x, int y, const RGBA & color)
{
	int texelIndex = GetIndexFromCoordinates(x,y,m_dimensions.x, m_dimensions.y);
	m_texels[texelIndex] = color;
}

RGBA Image::GetTexelAtUVS(Vector2 uvs) const
{
	Vector2 coordFloat  = Vector2( uvs.x * (float)GetDimensions().x, uvs.y * (float) GetDimensions().y);
	IntVector2 coords = IntVector2((int)floor(coordFloat.x), (int)floor(coordFloat.y));
	return GetTexel(coords.x, coords.y);
}

RGBA * Image::GetBuffer(int x, int y)
{
	int texIndex = GetIndexFromCoordinates(x, y,  m_dimensions.x, m_dimensions.y);
	return &(m_texels[texIndex]);
}

IntVector2 Image::GetDimensions() const
{
	return m_dimensions;
}

void * Image::GetImageData() const
{
	return (void*) &m_texels[0];
}

void Image::FlipY()
{
	std::vector< RGBA >	tempTexels = m_texels;
	for (int x = 0; x < m_dimensions.x; x++){
		for (int y = 0; y < m_dimensions.y; y++){
			IntVector2 flippedCoords = IntVector2(x, m_dimensions.y - y - 1);
			RGBA newColor = tempTexels[GetIndexFromCoordinates(flippedCoords, m_dimensions)];
			SetTexel(x,y,newColor);
		}
	}
}

void Image::PopulateFromR8(unsigned char * imageData, IntVector2 dimensions)
{
	int arraySize = dimensions.x * dimensions.y;
	m_texels.resize(dimensions.x * dimensions.y);
	int texelIndex = 0;
	for (int byteIndex = 0; byteIndex < arraySize; byteIndex++){
		unsigned char r = imageData[byteIndex];
		unsigned char g = imageData[byteIndex];
		unsigned char b = imageData[byteIndex];
		texelIndex = byteIndex;
		m_texels[texelIndex] = RGBA(r,g,b,(unsigned char) 255);
	}
}

void Image::PopulateFromRGB(unsigned char * imageData, IntVector2 dimensions)
{
	int arraySize = dimensions.x * dimensions.y * 3;
	m_texels.resize(dimensions.x * dimensions.y);
	int texelIndex = 0;
	for (int byteIndex = 0; byteIndex < arraySize; byteIndex+=3){
		unsigned char r = imageData[byteIndex];
		unsigned char g = imageData[byteIndex + 1];
		unsigned char b = imageData[byteIndex + 2];
		texelIndex = byteIndex / 3;
		m_texels[texelIndex] = RGBA(r,g,b,(unsigned char) 255);
	}
}

void Image::PopulateFromRGBA(unsigned char * imageData, IntVector2 dimensions)
{
	int arraySize = dimensions.x * dimensions.y * 4;
	m_texels.resize(dimensions.x * dimensions.y);
	int texelIndex = 0;
	for (int byteIndex = 0; byteIndex < arraySize; byteIndex+=4){
		unsigned char r = imageData[byteIndex];
		unsigned char g = imageData[byteIndex + 1];
		unsigned char b = imageData[byteIndex + 2];
		unsigned char a = imageData[byteIndex + 3];
		texelIndex = byteIndex / 4;
		m_texels[texelIndex] = RGBA(r,g,b,a);
	}
}
