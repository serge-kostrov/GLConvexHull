#include "GLColor.h"


const CGLColor CGLColor::white = { 1, 1, 1, 1 };
const CGLColor CGLColor::gray = { 0.5, 0.5, 0.5, 1 };
const CGLColor CGLColor::black = { 0, 0, 0, 1 };
const CGLColor CGLColor::red = { 1, 0, 0, 1 };
const CGLColor CGLColor::green = { 0, 1, 0, 1 };
const CGLColor CGLColor::blue = { 0, 0, 1, 1 };

GLclampf& CGLColor::operator[](int index)
{
	assert(0 <= index && index < dim);
	return *(&m_r + index);
}

GLclampf CGLColor::operator[](int index) const
{
	assert(0 <= index && index < dim);
	return *(&m_r + index);
}

CGLColor CGLColor::operator=(const CGLColor& other)
{
	m_r = other.m_r;
	m_g = other.m_g;
	m_b = other.m_b;
	m_a = other.m_a;

	return *this;
}