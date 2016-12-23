#pragma once
#include "GLColor.h"
#include "halfedge.h"


class ISceneObject
{
public:
    ISceneObject() {}
    virtual ~ISceneObject() {}

    virtual void initialize(GLuint shaderProgram) = 0;
    virtual void release() = 0;
    virtual void draw() = 0;
    virtual void update() = 0;
};

class CSceneMesh : public ISceneObject
{
public:
    CSceneMesh(CMesh& mesh) : m_mesh(mesh) {}
    ~CSceneMesh() override {}

    void initialize(GLuint shaderProgram) override;
    void release() override;
    void draw() override;
    void update() override;

    void setVxVisibility(bool bVisible);
    void setEdVisibility(bool bVisible);
    void setFcVisibility(bool bVisible);

private:
    void setAttributes(GLuint shaderProgram, bool bUseShading);

private:
    CMesh& m_mesh;

    bool m_bDrawVx = true;
    bool m_bDrawEd = true;
    bool m_bDrawFc = true;

    GLfloat m_ptSize = 6.0f;
    GLfloat m_lnWidth = 2.0f;

    CGLColor m_vxCol = CGLColor::red;
    CGLColor m_edCol = CGLColor::black;
    CGLColor m_fcCol = CGLColor::gray;

    GLint m_unUseShading;

    GLuint m_vboVx;
    GLuint m_vboEd;
    GLuint m_vboFc;

    GLuint m_eboEd;

    GLuint m_vaoVx;
    GLuint m_vaoEd;
    GLuint m_vaoFc;

    vector<GLfloat> m_aVxVBO;
    vector<GLfloat> m_aEdVBO;
    vector<GLfloat> m_aFcVBO;

    vector<GLuint> m_aEdEBO;
};
