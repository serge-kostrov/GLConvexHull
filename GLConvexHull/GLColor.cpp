#include "GLColor.h"


const CGLColor CGLColor::white = { 1.f, 1.f, 1.f, 1.f };
const CGLColor CGLColor::gray = { 0.5f, 0.5f, 0.5f, 1.0f };
const CGLColor CGLColor::black = { 0.f, 0.f, 0.f, 1.f };
const CGLColor CGLColor::red = { 1.f, 0.f, 0.f, 1.f };
const CGLColor CGLColor::green = { 0.f, 1.f, 0.f, 1.f };
const CGLColor CGLColor::blue = { 0.f, 0.f, 1.f, 1.f };

CGLColor CGLColor::operator=(const CGLColor& other)
{
	m_r = other.m_r;
	m_g = other.m_g;
	m_b = other.m_b;
	m_a = other.m_a;

	return *this;
}