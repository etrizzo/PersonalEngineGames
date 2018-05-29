/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Renderer.hpp"

/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL VARIABLES                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL FUNCTIONS                                                      */
/*                                                                      */
/************************************************************************/

static void BindImageToSide( GLenum side, Image &img, unsigned int size, unsigned int ox, unsigned int oy, GLenum channels, GLenum pixel_layout ) 
{
	void *ptr = img.GetBuffer( ox, oy ); 
	//void const *ptr = &img.GetTexel(ox,oy);
	glTexSubImage2D( side,
		0,          // mip_level
		0, 0,       // offset
		size, size, 
		channels, 
		pixel_layout, 
		ptr ); 

	//glTexSubImage2D( side,					//target
	//	0,									// mip_level
	//	0, 0,								// offset
	//	size, size,							//size
	//	channels,							//format
	//	pixel_layout,						//type
	//	ptr );								//pixels

	//glTexImage2D(side,
	//	0,
	//	internalFormat,
	//	size,size,
	//	0,
	//	channels,
	//	pixel_layout,
	//	ptr);

	GL_CHECK_ERROR(); 
}





TextureCube::TextureCube(char const * filename)
{
	MakeFromImage(filename);
}

TextureCube::~TextureCube()
{
	Cleanup();
}

void TextureCube::Cleanup()
{
	if (IsValid()) {
		glDeleteTextures( 1, &m_handle );
		m_handle = NULL; 
	}

	m_size = 0U; 
}

//bool TextureCube::MakeFromImages(Image const * images)
//{
//	if (m_handle == NULL) {
//		glGenTextures( 1, &m_handle ); 
//		ASSERT_OR_DIE( IsValid(), "No skybox texture!!" ); 
//	}
//
//	m_size = images[0].GetDimensions().x; 
//	m_format = TEXTURE_FORMAT_RGBA8;
//
//	GLenum internal_format = GL_RGBA8; 
//	GLenum channels = GL_RGBA8; 
//	GLenum pixel_layout = GL_BYTE; 
//
//	// bind it; 
//	glBindTexture( GL_TEXTURE_CUBE_MAP, m_handle ); 
//
//	glTexStorage2D( GL_TEXTURE_CUBE_MAP, 1, internal_format, m_size, m_size ); 
//	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 ); 
//
//	// bind the image to the side; 
//	FlipAndBindImage( GL_TEXTURE_CUBE_MAP_POSITIVE_X,	images[0], channels, pixel_layout ); 
//	FlipAndBindImage( GL_TEXTURE_CUBE_MAP_NEGATIVE_X,	images[1], channels, pixel_layout ); 
//	FlipAndBindImage( GL_TEXTURE_CUBE_MAP_POSITIVE_Y,	images[2], channels, pixel_layout ); 
//	FlipAndBindImage( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,	images[3], channels, pixel_layout ); 
//	FlipAndBindImage( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,	images[4], channels, pixel_layout ); 
//	FlipAndBindImage( GL_TEXTURE_CUBE_MAP_POSITIVE_Z,   images[5], channels, pixel_layout ); 
//
//	return GLSucceeded(); 
//}

//static void FlipAndBindImage( GLenum side, Image const &img, GLenum channels, GLenum pixel_layout ) 
//{
//	Image copy = Image(img.GetTexels(), img.GetDimensions());
//	BindImageToSide( side, copy, copy.GetDimensions().x, 0, 0, channels, pixel_layout ); 
//}

bool TextureCube::MakeFromImage(Image & image)
{
	unsigned int width = (unsigned int) image.GetDimensions().x; 
	unsigned int size = width / 4; 

	// make sure it is the type I think it is; 
	ASSERT_OR_DIE( image.GetDimensions().y == (size * 3), "Invalid skybox format" ); 

	if (m_handle == NULL) {
		glGenTextures( 1, &m_handle ); 
		if (!IsValid()){
			return false;
		}
	}

	m_size = size; 
	TODO("Determine format but actually");
	m_format = TEXTURE_FORMAT_RGBA8;	
	//Image copy = Image(image.GetTexels(), image.GetDimensions());

	GLenum internal_format = GL_RGBA8; 
	GLenum channels = GL_RGBA;  
	GLenum pixel_layout = GL_UNSIGNED_BYTE;  
	//GLenum internal_format = GL_DEPTH_STENCIL; 
	//GLenum channels = GL_DEPTH_STENCIL; 
	//GLenum pixel_layout = GL_UNSIGNED_INT_24_8; 
	//TextureGetInternalFormat( &internal_format, &channels, &pixel_layout, m_format ); 
	GL_CHECK_ERROR(); 

	// bind it; 
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_handle ); 
	GL_CHECK_ERROR();
	glTexStorage2D( GL_TEXTURE_CUBE_MAP, 1, internal_format, m_size, m_size ); 
	GL_CHECK_ERROR(); 

	glPixelStorei( GL_UNPACK_ROW_LENGTH, image.GetDimensions().x ); 
	GL_CHECK_ERROR(); 

	// bind the image to the side; 
	BindImageToSide( GL_TEXTURE_CUBE_MAP_POSITIVE_X,	image,  m_size, m_size * 2, m_size * 1, channels, pixel_layout ); 																
	BindImageToSide( GL_TEXTURE_CUBE_MAP_NEGATIVE_X,	image,  m_size, m_size * 0, m_size * 1, channels, pixel_layout ); 															
	BindImageToSide( GL_TEXTURE_CUBE_MAP_POSITIVE_Y,    image,  m_size, m_size * 1, m_size * 0, channels, pixel_layout ); 																
	BindImageToSide( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,	image,  m_size, m_size * 1, m_size * 2, channels, pixel_layout ); 																
	BindImageToSide( GL_TEXTURE_CUBE_MAP_POSITIVE_Z,	image,  m_size, m_size * 1, m_size * 1, channels, pixel_layout ); 															
	BindImageToSide( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,	image,  m_size, m_size * 3, m_size * 1, channels, pixel_layout ); 

	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 ); 

	return GLSucceeded(); 
}

bool TextureCube::MakeFromImage(char const * filename)
{
	Image img = Image(IMAGE_DIRECTORY + filename); 
	return MakeFromImage( img ); 
}
