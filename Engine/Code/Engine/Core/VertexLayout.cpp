#include "VertexLayout.hpp"

VertexLayout::VertexLayout(unsigned int stride, const VertexAttribute attributes[], int numAttributes)
{
	m_stride = stride;
	m_attributes.insert(m_attributes.begin(), attributes, attributes + numAttributes);
}


VertexAttribute::VertexAttribute()
{
	m_name			= "";
	m_type			= RDT_UNSIGNED_BYTE;
	m_elementCount	= 0;
	m_normalized	= false;
	m_memberOffset	= 0;
}

VertexAttribute::VertexAttribute(const char * name, eRenderDataType type, int elementCount, bool isNormalized, size_t offset)
{
	m_name			= name;
	m_type			= type;
	m_elementCount	= elementCount;
	m_normalized	= isNormalized;
	m_memberOffset	= (unsigned int) offset;
}


