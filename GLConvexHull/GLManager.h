#pragma once
#include "EventManager.h"
#include "ModelViewProjection.h"
#include "GLColor.h"
#include "scene.h"


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
	void operator=(const CGLManager& other) {}
	void enableAntialiasing() const;
	void initializeBuffers();
	void releaseBuffers();
	bool initializeShaderProgram();
	bool initializeShader(GLuint& shader, const GLuint& shaderType, const char* shaderSource);
	void setVertexAttributes(bool bUseShading);
	void releaseShaderProgram();
	void drawScene();

	void scale(double factor);
	void move(const double2& shift);
	void rotate(const double2& shift);

private:
	static CGLManager* m_instance;

    unique_ptr<ISceneObject> m_object;
    CMesh m_mesh;

	//GLuint m_vboVx;
	//GLuint m_vboEd;
	//GLuint m_vboFc;

	//GLuint m_vaoVx;
	//GLuint m_vaoEd;
	//GLuint m_vaoFc;

	//GLuint m_iboEd;	

	GLuint m_shaderProgram;
	GLuint m_vxShader;
	GLuint m_fgShader;
	//GLint m_unUseShading;
	GLint m_unMVP;
	GLint m_unLightPos;
	GLint m_unLightCol;
	CModelViewProjection m_MVP;
	GLFWwindow* m_window;
	int2 m_winSize;

	//CGLColor m_vxCol;
	//CGLColor m_edCol;
	//CGLColor m_fcCol;
	//vector<GLfloat> m_aVxVBO;
	//vector<GLfloat> m_aEdVBO;
	//vector<GLfloat> m_aFcVBO;
	//vector<GLuint> m_aEdIBO;
	//bool m_bDrawVx;
	//bool m_bDrawEd;
	//bool m_bDrawFc;
	//GLfloat m_ptSize;
	//GLfloat m_lineWidth;

	glfloat3 m_lightPos;
	CGLColor m_lightCol;
};

