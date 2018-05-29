#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
//#include "Engine/Math/Renderer.hpp"


struct VertexAttribute
{
	VertexAttribute();
	VertexAttribute(const char* name, eRenderDataType type, int elementCount, bool isNormalized, size_t offset);
	std::string			m_name; 
	eRenderDataType		m_type; 
	unsigned int        m_elementCount; 
	bool				m_normalized; 
	unsigned int        m_memberOffset; 
};

class VertexLayout 
{
public:
	VertexLayout(){};
	VertexLayout(unsigned int stride, const VertexAttribute attributes[], int numAttributes);
	unsigned int GetAttributeCount() const { return m_attributes.size(); };
	VertexAttribute const& GetAttribute( unsigned int const idx ) const { return m_attributes[idx]; }; 

public:
	// what are the members
	std::vector<VertexAttribute> m_attributes;

	// how large is the vertex in bytes
	unsigned int m_stride; 
};

