#include "stdafx.h"
#include "scene.h"


void CSceneMesh::initialize(GLuint shaderProgram)
{
    // aVx
    {
        glGenVertexArrays(1, &m_vaoVx);
        glBindVertexArray(m_vaoVx);
        glGenBuffers(1, &m_vboVx);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVx);
        setAttributes(shaderProgram, false);
    }

    // aEd
    {
        glGenVertexArrays(1, &m_vaoEd);
        glBindVertexArray(m_vaoEd);
        glGenBuffers(1, &m_vboEd);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboEd);
        glGenBuffers(1, &m_eboEd);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboEd);
        setAttributes(shaderProgram, false);
    }

    // aFc
    {
        glGenVertexArrays(1, &m_vaoFc);
        glBindVertexArray(m_vaoFc);
        glGenBuffers(1, &m_vboFc);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboFc);
        setAttributes(shaderProgram, true);
    }

    update();
}

void CSceneMesh::release()
{
    glDeleteBuffers(1, &m_vboFc);
    glDeleteBuffers(1, &m_eboEd);
    glDeleteBuffers(1, &m_vboEd);
    glDeleteBuffers(1, &m_vboVx);
    glDeleteVertexArrays(1, &m_vaoFc);
    glDeleteVertexArrays(1, &m_vaoEd);
    glDeleteVertexArrays(1, &m_vaoVx);
    //glDisableVertexAttribArray(m_inPos);
    //glDisableVertexAttribArray(m_inNm);
    //glDisableVertexAttribArray(m_inCol);
}

void CSceneMesh::draw()
{
    glUniform1ui(m_unUseShading, 0);

    if(m_bDrawVx)
    {
        glBindVertexArray(m_vaoVx);
        glDrawArrays(GL_POINTS, 0, m_mesh.aVx().size());
    }

    if(m_bDrawEd)
    {
        glBindVertexArray(m_vaoEd);
        glDrawElements(GL_LINES, 2*m_mesh.aEd().size(), GL_UNSIGNED_INT, 0);
    }

    if(m_bDrawFc)
    {
        glUniform1ui(m_unUseShading, 1);
        glBindVertexArray(m_vaoFc);
        glDrawArrays(GL_TRIANGLES, 0, 3*m_mesh.aFc().size());
    }
}

void CSceneMesh::update()
{
    list<CVertex>& aVx = m_mesh.aVx();
    list<CEdge>& aEd = m_mesh.aEd();
    list<CFace>& aFc = m_mesh.aFc();

    const int nVx = aVx.size();
    const int nEd = aEd.size();
    const int nFc = aFc.size();

    // aVx
    {
        m_aVxVBO.clear();
        m_aVxVBO.reserve(nVx*(double3::dim + CGLColor::dim));
        for(const auto& vx : aVx)
        {
            for(const auto& i : vx.pos())
                m_aVxVBO.push_back(static_cast<GLfloat>(i));
            for(const auto& i : m_vxCol)
                m_aVxVBO.push_back(i);
        }
    }

    // aEd
    {
        using namespace std;

        m_aEdVBO.clear();
        m_aEdVBO.reserve(nVx*(double3::dim + CGLColor::dim));
        m_aEdEBO.clear();
        m_aEdEBO.reserve(2*nEd);

        set<ItEdge> aEdVisited;
        GLint iVx = 0;
        for(auto& ed = aEd.begin(); ed != aEd.end(); ++ed)
        {
            if(aEdVisited.find(ed) == aEdVisited.end() && aEdVisited.find(ed->twin()) == aEdVisited.end())
            {
                array<double3, 2> edEnds;
                edEnds.front() = ed->vx()->pos();
                edEnds.back() = ed->next()->vx()->pos();

                for(const auto& vx : edEnds)
                {
                    for(const auto& i : vx)
                        m_aEdVBO.push_back(static_cast<GLfloat>(i));
                    for(const auto& i : m_edCol)
                        m_aEdVBO.push_back(i);
                    m_aEdEBO.push_back(iVx);
                    ++iVx;
                }
                aEdVisited.insert(ed);
            }
        }

        unordered_map<ItVertex, GLint> aVxVisited;
        GLint iVx = 0;
        for(auto& ed = aEd.begin(); ed != aEd.end(); ++ed)
        {
            const ItVertex& vx = ed->vx();
            const ItVertex& vxNb = ed->twin()->vx();
            const auto& vxIndex = aVxVisited.find(vx);
            const auto& vxNbIndex = aVxVisited.find(vxNb);

            if(aVxVisited.find(vx) == aVxVisited.end())
            {
                for(auto i : vx->pos())
                    m_aEdVBO.push_back(static_cast<GLfloat>(i));
                for(auto i : m_edCol)
                    m_aEdVBO.push_back(i);

                m_aEdEBO.push_back(iVx);
                if(vxNbIndex != aVxVisited.end())
                    m_aEdEBO.push_back(vxNbIndex->second);

                ++iVx;
            }

            if(aVxVisited.find(vxNb) == aVxVisited.end())
            {
                for(auto i : vx->pos())
                    m_aEdVBO.push_back(static_cast<GLfloat>(i));
                for(auto i : m_edCol)
                    m_aEdVBO.push_back(i);

                m_aEdEBO.push_back(iVx);
                if(vxIndex != aVxVisited.end())
                    m_aEdEBO.push_back(vxNbIndex->second);

                ++iVx;
            }
        }
    }

    // aFc
    {

    }
}

void CSceneMesh::setVxVisibility(bool bVisible)
{
    m_bDrawVx = bVisible;
}

void CSceneMesh::setEdVisibility(bool bVisible)
{
    m_bDrawEd = bVisible;
}

void CSceneMesh::setFcVisibility(bool bVisible)
{
    m_bDrawFc = bVisible;
}

void CSceneMesh::setAttributes(GLuint shaderProgram, bool bUseShading)
{
    GLsizei stride = (glfloat3::dim + CGLColor::dim)*sizeof(GLfloat);
    if(bUseShading)
        stride += glfloat3::dim*sizeof(GLfloat);

    // position
    GLvoid* pPos = reinterpret_cast<GLvoid*>(0);
    GLint inPos = glGetAttribLocation(shaderProgram, "in_Position");
    glEnableVertexAttribArray(inPos);
    glVertexAttribPointer(inPos, glfloat3::dim, GL_FLOAT, GL_FALSE, stride, pPos);

    // color
    GLvoid* pCol = reinterpret_cast<GLvoid*>(glfloat3::dim*sizeof(GLfloat));
    GLint inCol = glGetAttribLocation(shaderProgram, "in_Color");
    glEnableVertexAttribArray(inCol);
    glVertexAttribPointer(inCol, CGLColor::dim, GL_FLOAT, GL_FALSE, stride, pCol);

    // normal
    if(bUseShading)
    {
        GLvoid* pNm = reinterpret_cast<GLvoid*>((glfloat3::dim + CGLColor::dim)*sizeof(GLfloat));
        GLint inNm = glGetAttribLocation(shaderProgram, "in_Normal");
        glEnableVertexAttribArray(inNm);
        glVertexAttribPointer(inNm, glfloat3::dim, GL_FLOAT, GL_FALSE, stride, pNm);
    }

    m_unUseShading = glGetUniformLocation(shaderProgram, "un_UseShading");
}
