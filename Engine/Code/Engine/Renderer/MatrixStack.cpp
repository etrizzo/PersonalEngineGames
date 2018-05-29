#include "MatrixStack.hpp"

MatrixStack::MatrixStack()
{
	m_top = Matrix44();
	m_top.SetIdentity();
}

void MatrixStack::Push()
{
	m_stack.push(m_top);
	//m_top.SetIdentity();
}

void MatrixStack::Pop()
{
	m_top = m_stack.top();
	m_stack.pop();
}

Matrix44 const & MatrixStack::GetTop()
{
	return m_top;
}

void MatrixStack::Load(Matrix44 const & mat)
{
	m_top = mat;
}

void MatrixStack::LoadIdentity()
{
	m_top.SetIdentity();
}

void MatrixStack::Transform(Matrix44 const & mat)
{
	m_top.Append(mat);
}
