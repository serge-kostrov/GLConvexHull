#pragma once
#include "base.h"
#include <glm\glm.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;


class CModelViewProjection
{
public:
	CModelViewProjection();
	~CModelViewProjection();

	void addScaling(const vec3& factor);
	void addTranslation(const vec3& vector);
	void addRotation(const vec3& angle);
	void setProjection();
	mat4 mvp() const;

private:
	mat4 m_mtxScale;
	mat4 m_mtxTrans;
	mat4 m_mtxRot;
	mat4 m_mtxProj;
	mat4 m_mtxTransToCm;
	mat4 m_mtxTransFromCm;
	//const vec3 m_cm = vec3{ 0, 0.12, 1.1 };
	const vec3 m_cm = vec3{ 0, 0, 0 };
};

