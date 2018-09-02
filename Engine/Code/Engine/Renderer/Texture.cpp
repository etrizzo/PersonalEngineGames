//-----------------------------------------------------------------------------------------------
// Texture.cpp
//
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
//#include <gl/GL.h>
#include "Engine/Renderer/Texture.hpp"
//#include "ThirdParty/stb/stb_image.c"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/Image.hpp"

#include "Engine/Renderer/external/gl/glcorearb.h"
#include "Engine/Renderer/external/gl/glext.h"
#include "Engine/Renderer/external/gl/wglext.h"
#include "Engine/Renderer/glfunctions.h"

std::map<std::string, Texture*>	Texture::s_loadedTextures;

//-----------------------------------------------------------------------------------------------
// Called only by the Renderer.  Use renderer->CreateOrGetTexture() to instantiate textures.
//
Texture::Texture( const std::string& imageFilePath, bool setMips )
	: m_textureID( 0 )
	, m_dimensions( 0, 0 )
{
	Image* texImage = new Image(imageFilePath);
	texImage->FlipY();
	PopulateFromImage(texImage, setMips);
	m_path = imageFilePath;
}
Texture::Texture(const Image * image, bool setMips)
{
	PopulateFromImage(image, setMips);
}

//-----------------------------------------------------------------------------------------------
// Creates a texture identity on the video card, and populates it with the given image texel data
//
void Texture::PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents )
{
	m_dimensions = texelSize;

	// Enable texturing
	glEnable( GL_TEXTURE_2D );

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, (GLuint*) &m_textureID );

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_2D, m_textureID );

	// Set texture clamp vs. wrap (repeat)
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // GL_CLAMP or GL_REPEAT
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); // GL_CLAMP or GL_REPEAT

	// Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if( numComponents == 3 )
		bufferFormat = GL_RGB;

	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL

	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		m_dimensions.x,			// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
		m_dimensions.y,			// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1, recommend 0)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
		imageData );		// Address of the actual pixel data bytes/buffer in system memory

	glDisable(GL_TEXTURE_2D);
}

void Texture::PopulateFromImage(const Image * image, bool setMips)
{
	m_dimensions = image->GetDimensions();

	if (m_dimensions.x == 0 && m_dimensions.y == 0){
		TODO("Make an invalid texture");
		std::vector<RGBA> invalid = {RGBA::MAGENTA, RGBA::BLACK, RGBA::BLACK, RGBA::MAGENTA};
		image = new Image(invalid, IntVector2(1,1));
	}

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	GL_CHECK_ERROR();
	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, (GLuint*) &m_textureID );
	GL_CHECK_ERROR();
	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_2D, m_textureID );
	GL_CHECK_ERROR();
													   // Set magnification (texel > pixel) and minification (texel < pixel) filters

	GLenum bufferFormat = GL_RGBA8; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	//if( numComponents == 3 )
	//	bufferFormat = GL_RGB;

	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL

	GLenum channels = GL_RGBA;
	//if (numComponents == 3){
	//	channels = GL_RGB;
	//}
	
	unsigned int mip_count = 1;
	if (setMips){
		mip_count = CalculateMipCount( Max(m_dimensions.x, m_dimensions.y) ); 
	}									  // only this is needed for render targets; 
	
	glTexStorage2D( GL_TEXTURE_2D, 
		mip_count,       // number of levels (mip-layers)           CHANGED FROM ONE!!
		internalFormat, // how is the memory stored on the GPU
		m_dimensions.x, m_dimensions.y ); // dimenions


	//glTexImage2D(			// Upload this pixel data to our new OpenGL texture
	//	GL_TEXTURE_2D,		// Creating this as a 2d texture
	//	0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
	//	internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
	//	m_dimensions.x,			// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
	//	m_dimensions.y,			// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
	//	0,					// Border size, in texels (must be 0 or 1, recommend 0)
	//	bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
	//	GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
	//	image->GetImageData() );		// Address of the actual pixel data bytes/buffer in system memory
	
	// copies cpu memory to the gpu - needed for texture resources
	glTexSubImage2D( GL_TEXTURE_2D,
		0,             // mip layer we're copying to
		0, 0,          // offset
		m_dimensions.x, m_dimensions.y, // dimensions
		channels,      // which channels exist in the CPU buffer
		GL_UNSIGNED_BYTE,     // how are those channels stored
		image->GetImageData() ); // cpu buffer to copy;
	GL_CHECK_ERROR();

	if (mip_count > 1){
		glActiveTexture( GL_TEXTURE0 ); 
		glBindTexture( GL_TEXTURE_2D, GetHandle() );    // bind our texture to our current texture unit (0)
		glGenerateMipmap( GL_TEXTURE_2D ); 
	}
	GL_CHECK_ERROR();

}

bool Texture::CreateRenderTarget(unsigned int width, unsigned int height, eTextureFormat format)
{
	GL_CHECK_ERROR(); 
	// generate the link to this texture
	glGenTextures( 1, &m_textureID ); 
	if (m_textureID == NULL) {
		return false; 
	}

	// TODO - add a TextureFormatToGLFormats( GLenum*, GLenum*, GLenum*, eTextureFormat )
	//        when more texture formats are required; 
	GLenum internal_format = GL_RGBA8; 
	GLenum channels = GL_RGBA;  
	GLenum pixel_layout = GL_UNSIGNED_BYTE;  
	if (format == TEXTURE_FORMAT_D24S8) {
		internal_format = GL_DEPTH24_STENCIL8; 
		channels = GL_DEPTH_STENCIL; 
		pixel_layout = GL_UNSIGNED_INT_24_8; 
	}

	// Copy the texture - first, get use to be using texture unit 0 for this; 
	glActiveTexture( GL_TEXTURE0 ); 
	glBindTexture( GL_TEXTURE_2D, m_textureID );    // bind our texture to our current texture unit (0)

												 // Copy data into it;
	//glTexImage2D( GL_TEXTURE_2D, 0, 
	//	internal_format, // what's the format OpenGL should use
	//	width, 
	//	height,        
	//	0,             // border, use 0
	//	channels,      // how many channels are there?
	//	pixel_layout,  // how is the data laid out
	//	nullptr );     // don't need to pass it initialization data 

	unsigned int mip_count = 1;							  // only this is needed for render targets; 

	glTexStorage2D( GL_TEXTURE_2D, 
		mip_count,       // number of levels (mip-layers)           CHANGED FROM ONE!!
		internal_format, // how is the memory stored on the GPU
		width, height); // dimenions

					   // make sure it suceeded
	GL_CHECK_ERROR(); 
	// cleanup after myself; 
	glBindTexture( GL_TEXTURE_2D, NULL ); // unset it; 

										  // Save this all off
	m_dimensions = IntVector2((int) width, (int) height);

	m_format = format; // I save the format with the texture
					// for sanity checking.

					// great, success
	return true; 
}

Texture * Texture::CreateCompatible(Texture * src)
{
	Texture* newTex = new Texture();
	newTex->CreateRenderTarget(src->GetWidth(), src->GetHeight(), src->m_format);
	return newTex;
}

Texture * Texture::CreateOrGetTexture(const std::string & texPath, const std::string & directory, bool setMips)
{
	auto containsTex = s_loadedTextures.find((std::string)texPath); 
	Texture* loadedTex = nullptr;
	if (containsTex != s_loadedTextures.end()){
		loadedTex = containsTex->second;
	} else {
		std::string texturePath = directory + texPath;
		loadedTex = new Texture(texturePath.c_str(), setMips);
		ASSERT_OR_DIE(loadedTex->GetHandle() != NULL, "Invalid texture path");
		s_loadedTextures.insert(std::pair<std::string, Texture*> (texPath, loadedTex));
	}
	return loadedTex;
}

void Texture::RegisterTextureFromImage(const std::string & name, Image * img)
{
	Texture* tex = new Texture(img);
	s_loadedTextures.insert(std::pair<std::string, Texture*> (name, tex));
}

unsigned int Texture::CalculateMipCount(int size)
{
	unsigned int mipCount = 1;
	int power = (int) pow(2, mipCount);
	while (power < size){
		mipCount++;
		power = (int) pow(2, mipCount);
	}
	return mipCount;
}

