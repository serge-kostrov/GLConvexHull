#pragma once
#include "base.h"

struct SIdVx
{
public:
	SIdVx() : m_id(0) {}
	~SIdVx() {}

	explicit operator GLint() const { return static_cast<GLint>(m_id); }

	SIdVx& operator++();
	SIdVx operator++(int);

	bool operator==(const SIdVx& other) { return m_id == other.m_id; }
	bool operator!=(const SIdVx& other) { return !operator==(other); }

private:
	int m_id;
};

class CIdVxManager
{
public:
	~CIdVxManager() {}
	static CIdVxManager* instance();
	SIdVx next() { return ++m_id; }
	SIdVx current() const { return m_id; }

private:
	CIdVxManager() {}
	CIdVxManager(const CIdVxManager& other);
	void operator=(const CIdVxManager& other);

private:
	static CIdVxManager* m_instance;
	SIdVx m_id;
};