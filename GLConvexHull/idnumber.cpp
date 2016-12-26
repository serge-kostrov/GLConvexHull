#include "idnumber.h"


CIdVxManager* CIdVxManager::m_instance = nullptr;

CIdVxManager* CIdVxManager::instance()
{
	if (!m_instance)
	{
		m_instance = new CIdVxManager{};
		m_instance->m_id = SIdVx{};
	}

	return m_instance;
}

SIdVx& SIdVx::operator++()
{
	++m_id;
	return *this;
}

SIdVx SIdVx::operator++(int)
{
	SIdVx result(*this);
	++(*this);
	return result;
}