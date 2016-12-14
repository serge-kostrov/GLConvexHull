#pragma once
#include "EventManager.h"
#include "ModelViewProjection.h"
#include "GLColor.h"


class CGLManager
{
private:
	friend class CEventManager;

public:
	~CGLManager() {}

	static CGLManager* instance();
	bool initializeGL();
	void releaseGL();
	bool handleInput();
	void paintGL();
	void updateGL();

private:
	CGLManager() {}
	CGLManager(const CGLManager& other);
	void operator=(const CGLManager& other);
	void initializeBuffers();
	void releaseBuffers();
	bool initializeShaderProgram();
	bool initializeShader(GLuint& shader, const GLuint& shaderType, const char* shaderSource);
	void releaseShaderProgram();
	void drawScene();

	void scaleVx(double factor);
	void moveVx(const double2& shift);
	void rotateVx(const double2& shift);

private:
	static CGLManager* m_instance;
	GLuint m_vbo;
	GLuint m_vao;
	GLuint m_ibo;
	GLuint m_shaderProgram;
	GLuint m_vxShader;
	GLuint m_fgShader;
	GLint m_unMVP;
	GLint m_unLightPos;
	GLint m_unLightCol;
	CModelViewProjection m_MVP;
	GLFWwindow* m_window;
	int2 m_winSize;
	std::vector<GLfloat> m_aVx;
	std::vector<GLuint> m_aInd;
	std::vector<GLuint> m_aIndEd;
	glfloat3 m_lightPos;
	CGLColor m_lightCol;
};

