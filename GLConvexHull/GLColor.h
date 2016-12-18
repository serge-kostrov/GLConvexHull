#pragma once
#include "stdheaders.h"
#include <glew/glew.h>
#include <GLFW/glfw3.h>

class CGLColor
{
public:

	// Constructors and destructors
	CGLColor() : m_r(0.f), m_g(0.f), m_b(0.f), m_a(1.f) {}
	CGLColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a = 1.f) : m_r(r), m_g(g), m_b(b), m_a(a) {}
	CGLColor(const CGLColor& other) : m_r(other.m_r), m_g(other.m_g), m_b(other.m_b), m_a(other.m_a) {}
	~CGLColor() {}

	// Access
	GLclampf r() const { return m_r; }
	GLclampf& r() { return m_r; }
	GLclampf g() const { return m_g; }
	GLclampf& g() { return m_g; }
	GLclampf b() const { return m_b; }
	GLclampf& b() { return m_b; }
	GLclampf a() const { return m_a; }
	GLclampf& a() { return m_a; }

	GLclampf* begin() { return &m_r; }
	const GLclampf* begin() const { return &m_r; }
	GLclampf* end() { return &m_r + dim; }
	const GLclampf* end() const { return &m_r + dim; }

	GLclampf& operator[](int index);
	GLclampf operator[](int index) const;

	CGLColor operator=(const CGLColor& other);

public:
	static const int dim = 4;

	// Predefined colors
	static const CGLColor black;
	static const CGLColor gray;
	static const CGLColor white;
	static const CGLColor red;
	static const CGLColor green;
	static const CGLColor blue;
	
private:
	GLclampf m_r;
	GLclampf m_g;
	GLclampf m_b;
	GLclampf m_a;
};