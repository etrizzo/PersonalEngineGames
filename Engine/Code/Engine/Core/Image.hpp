#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <vector>



class Image
{
public:
	//was explicit - changed to make default texture image
	Image( const std::string& imageFilePath );		//by default, image will read (0,0) as bottom left now
	Image( std::vector<RGBA> texels, IntVector2 dimensions);
	RGBA	GetTexel( int x, int y ) const; 			// (0,0) is bottom-left
	RGBA	GetTexel(int idx) const;
	void	SetTexel( int x, int y, const RGBA& color );

	RGBA*	GetBuffer(int x, int y);
	IntVector2 GetDimensions() const;
	void* GetImageData() const;
	void FlipY();			//flips texel data on y 	
	std::vector<RGBA> GetTexels() const {return m_texels; };

private:
	IntVector2		m_dimensions;
	std::vector< RGBA >	m_texels; // ordered left-to-right, then down, from [0]=(0,0) at top-left

	void PopulateFromR8(unsigned char* imageData, IntVector2 dimensions);
	void PopulateFromRGB(unsigned char* imageData, IntVector2 dimensions);
	void PopulateFromRGBA(unsigned char* imageData, IntVector2 dimensions);

};