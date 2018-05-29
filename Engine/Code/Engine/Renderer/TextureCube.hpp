#pragma once


/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "Engine/Core/EngineCommon.hpp"


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
class Image;  // core/image/image.h

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

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class TextureCube 
{
public:
	TextureCube( char const* filename );
	~TextureCube();

	void Cleanup();

	// make a cube map from 6 images
	// +x -x +y -y +z -z
	bool MakeFromImages( Image const *images );

	// todo - may want this to take tile offsets
	// into the image since I can't assume the same
	// plus sign format my demo one is in
	bool MakeFromImage( Image &image );
	bool MakeFromImage( char const *filename ); 

	// cube maps should be square on each face
	inline unsigned int GetWidth() const { return m_size; }
	inline unsigned int GetHeight() const { return m_size; }

	inline bool IsValid() const { return (m_handle != NULL); }

public:
	// gl
	inline unsigned int GetHandle() const { return m_handle; }

public:
	unsigned int m_size = 1U;          // cube maps are equal length on all sizes
	eTextureFormat m_format; 

	// GL 
	unsigned int m_handle; 

public:
};

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* FUNCTION PROTOTYPES                                                  */
/*                                                                      */
/************************************************************************/
