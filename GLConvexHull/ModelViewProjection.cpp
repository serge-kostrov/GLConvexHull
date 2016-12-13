#include "ModelViewProjection.h"
#include <glm\gtc\matrix_transform.hpp>


CModelViewProjection::CModelViewProjection()
{
	m_mtxTransToCm = glm::translate(mat4(), m_cm);
	m_mtxTransFromCm = glm::translate(mat4(), -m_cm);
}

CModelViewProjection::~CModelViewProjection()
{
}

void CModelViewProjection::addScaling(const vec3& factor)
{
	m_mtxScale = glm::scale(mat4(), factor)*m_mtxScale;
}

void CModelViewProjection::addTranslation(const vec3& vector)
{
	m_mtxTrans = glm::translate(mat4(), vector)*m_mtxTrans;
}

void CModelViewProjection::addRotation(const vec3& angle)
{
	m_mtxRot = glm::rotate(mat4(), angle.x, vec3(1.0, 0.0, 0.0))*m_mtxRot;
	m_mtxRot = glm::rotate(mat4(), angle.y, vec3(0.0, 1.0, 0.0))*m_mtxRot;
	m_mtxRot = glm::rotate(mat4(), angle.z, vec3(0.0, 0.0, 1.0))*m_mtxRot;
}

void CModelViewProjection::setProjection()
{
	m_mtxProj = glm::perspective(45.0f, 1.0f, 1.0f, 10.0f)*glm::lookAt(vec3(0, 0, -3), vec3(0, 0, 0), vec3(0, 1, 0));
}

mat4 CModelViewProjection::mvp() const
{
	return m_mtxProj*m_mtxTransToCm*m_mtxTrans*m_mtxRot*m_mtxScale*m_mtxTransFromCm;
}
