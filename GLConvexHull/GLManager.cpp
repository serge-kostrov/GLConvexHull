#include "stdafx.h"
#include "GLManager.h"
#include "constants.h"
#include "GLColor.h"
#include "IOUtilities.h"
#include "geometrytools.h"
#include "scene.h"
#include "qquickhull.h"


CGLManager* CGLManager::m_instance = nullptr;

CGLManager* CGLManager::instance()
{
	using namespace std;
	if (!m_instance)
	{
		m_instance = new CGLManager();
		m_instance->m_winSize = { 720, 720 };
		m_instance->m_lightPos = glfloat3{ 0.0f, 0.0f, 2.2f };
		m_instance->m_lightCol = CGLColor::white;

        vector<double3> aPt;
        io_util::readXYZFile(aPt, "aPt.txt");
        CQuickHull hull(aPt);
        if(!hull.algo(m_instance->m_mesh))
            cout << "Failed to calculate a hull!" << endl;

        CSceneMesh* sceneMesh = new CSceneMesh(m_instance->m_mesh);
        sceneMesh->setVxVisibility(true);
        sceneMesh->setEdVisibility(true);
        sceneMesh->setFcVisibility(false);
        m_instance->m_object.reset(sceneMesh);

		//m_instance->m_ptSize = 6;
		//m_instance->m_lineWidth = 2;

		//m_instance->m_bDrawVx = true;
		//m_instance->m_bDrawEd = true;
		//m_instance->m_bDrawFc = true;

		// aPt
		//const int nPt = 4;
		//vector<double3> aPt(nPt);
		//{
		//	aPt[0] = double3{ 0.0, 0.5, 1.0 };
		//	aPt[1] = double3{ 0.5, -0.5, 1.0 };
		//	aPt[2] = double3{ -0.5, -0.5, 1.0 };
		//	aPt[3] = double3{ 0.0, 0.0, 1.5 };
		//}

		// Colors
		//CGLColor& vxCol = m_instance->m_vxCol;
		//CGLColor& edCol = m_instance->m_edCol;
		//CGLColor& fcCol = m_instance->m_fcCol;
		//vxCol = CGLColor::red;
		//edCol = CGLColor::black;
		//fcCol = CGLColor::gray;

		//vector<GLfloat>& aVx = m_instance->m_aVxVBO;
		//vector<GLfloat>& aEd = m_instance->m_aEdVBO;
		//vector<GLfloat>& aFc = m_instance->m_aFcVBO;
		//vector<GLuint>& aEdIBO = m_instance->m_aEdIBO;

		// aVx
		//{
		//	for (const auto& pt : aPt)
		//	{
		//		for (auto i : pt)
		//			aVx.push_back(static_cast<GLfloat>(i));
		//		for (auto i : vxCol)
		//			aVx.push_back(i);
		//	}
		//}

		// aEd
		//{
		//	for (const auto& pt : aPt)
		//	{
		//		for (auto i : pt)
		//			aEd.push_back(static_cast<GLfloat>(i));
		//		for (auto i : edCol)
		//			aEd.push_back(i);
		//	}
        //
		//	for (int i = 0; i < nPt; ++i)
		//	{
		//		for (int j = i + 1; j < nPt; ++j)
		//		{
		//			aEdIBO.push_back(i);
		//			aEdIBO.push_back(j);
		//		}
		//	}
		//}

		// aFc
		//{
		//	for (int i = 0; i < nPt; ++i)
		//	{
		//		const int i1 = (i + 1)%nPt;
		//		const int i2 = (i + 2)%nPt;
		//		const int i3 = (i + 3)%nPt;
        //
		//		std::array<double3, 3> tr;
		//		tr[0] = aPt[i];
		//		tr[1] = aPt[i1];
		//		tr[2] = aPt[i2];
        //
		//		const double3& pt = aPt[i3];
        //
		//		double3 nm = ((tr[2] - tr[0])%(tr[1] - tr[0])).normalized();
		//		if (distPtPln(pt, tr[0], nm) > 0)
		//			nm.negate();
        //
		//		for (const auto& pt : tr)
		//		{
		//			for (auto j : pt)
		//				aFc.push_back(static_cast<GLfloat>(j));
		//			for (auto j : fcCol)
		//				aFc.push_back(static_cast<GLfloat>(j));
		//			for (auto j : nm)
		//				aFc.push_back(static_cast<GLfloat>(j));					
		//		}
		//	}
		//}
	}

	return m_instance;
}

bool CGLManager::initializeGL()
{
	if (glfwInit() != GLFW_TRUE)
		return false;
	m_window = glfwCreateWindow(m_winSize.x(), m_winSize.y(), "Quickhull", nullptr, nullptr);
	if (!m_window)
		return false;
	glfwMakeContextCurrent(m_window);
	if (glewInit() != GLEW_OK)
		return false;
	CEventManager::instance()->setEventHandlers(m_window);
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	
	glEnable(GL_DEPTH_TEST);
	enableAntialiasing();

	if (!initializeShaderProgram())
		return false;
	//initializeBuffers();
    m_object->initialize(m_shaderProgram);
    setVertexAttributes(true);
	m_MVP.setProjection();

	return true;
}

void CGLManager::releaseGL()
{
	//releaseBuffers();
    m_object->release();
	releaseShaderProgram();
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool CGLManager::handleInput()
{
	if (CEventManager::instance()->willWindowClose() || glfwWindowShouldClose(m_window) == GLFW_TRUE)
		return false;

	return true;
}

void CGLManager::paintGL()
{
	glPointSize(m_ptSize);
	glLineWidth(m_lineWidth);
	glClearColor(CGLColor::white.r(), CGLColor::white.g(), CGLColor::white.b(), CGLColor::white.a());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();
}

void CGLManager::updateGL()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void CGLManager::enableAntialiasing() const
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// points
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
}

void CGLManager::initializeBuffers()
{
	// aVx
	//{
	//	glGenVertexArrays(1, &m_vaoVx);
	//	glBindVertexArray(m_vaoVx);
	//	glGenBuffers(1, &m_vboVx);
	//	glBindBuffer(GL_ARRAY_BUFFER, m_vboVx);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_aVxVBO.size(), m_aVxVBO.data(), GL_STATIC_DRAW);
	//	setVertexAttributes(false);
	//}

	// aEd
	//{
	//	glGenVertexArrays(1, &m_vaoEd);
	//	glBindVertexArray(m_vaoEd);
	//	glGenBuffers(1, &m_vboEd);
	//	glBindBuffer(GL_ARRAY_BUFFER, m_vboEd);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_aEdVBO.size(), m_aEdVBO.data(), GL_STATIC_DRAW);
	//	glGenBuffers(1, &m_iboEd);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboEd);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*m_aEdIBO.size(), m_aEdIBO.data(), GL_STATIC_DRAW);
	//	setVertexAttributes(false);
	//}

	// aFc
	//{
	//	glGenVertexArrays(1, &m_vaoFc);
	//	glBindVertexArray(m_vaoFc);
	//	glGenBuffers(1, &m_vboFc);
	//	glBindBuffer(GL_ARRAY_BUFFER, m_vboFc);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_aFcVBO.size(), m_aFcVBO.data(), GL_STATIC_DRAW);
	//	setVertexAttributes(true);
	//}
}

void CGLManager::releaseBuffers()
{
	//glDeleteBuffers(1, &m_vboFc);
	//glDeleteBuffers(1, &m_iboEd);
	//glDeleteBuffers(1, &m_vboEd);
	//glDeleteBuffers(1, &m_vboVx);
	//glDeleteVertexArrays(1, &m_vaoFc);
	//glDeleteVertexArrays(1, &m_vaoEd);
	//glDeleteVertexArrays(1, &m_vaoVx);
}

bool CGLManager::initializeShaderProgram()
{
	m_shaderProgram = glCreateProgram();
	if (!initializeShader(m_vxShader, GL_VERTEX_SHADER, "vxShader.glsl"))
		return false;
	if (!initializeShader(m_fgShader, GL_FRAGMENT_SHADER, "fgShader.glsl"))
		return false;
	glBindFragDataLocation(m_shaderProgram, 0, "out_Color");
	glLinkProgram(m_shaderProgram);
	glUseProgram(m_shaderProgram);

	return true;
}

bool CGLManager::initializeShader(GLuint& shader, const GLuint& shaderType, const char* shaderSource)
{
	std::string shaderData = "";
	if (!io_util::readAllLines(shaderData, shaderSource))
		return false;
	const char* shaderDataRaw = shaderData.c_str();

	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderDataRaw, nullptr);
	glCompileShader(shader);

	// check shader compilation
	{
		GLint status = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE)
		{
			const int bufSize = 512;
			char buf[bufSize];
			glGetShaderInfoLog(shader, bufSize, nullptr, buf);
			std::cout << "Shader compilation failed\n";
			std::cout << "Source file: " << shaderSource << "\n";
			std::cout << "Info log: " << buf << "\n";
			return false;
		}			
	}

	glAttachShader(m_shaderProgram, shader);

	return true;
}

void CGLManager::setVertexAttributes(bool bUseShading)
{
	//GLsizei stride = (glfloat3::dim + CGLColor::dim)*sizeof(GLfloat);
	//if (bUseShading)
	//	stride += glfloat3::dim*sizeof(GLfloat);
    //
	//// position
	//GLvoid* pPos = reinterpret_cast<GLvoid*>(0);
	//GLint inPos = glGetAttribLocation(m_shaderProgram, "in_Position");
	//glEnableVertexAttribArray(inPos);
	//glVertexAttribPointer(inPos, glfloat3::dim, GL_FLOAT, GL_FALSE, stride, pPos);
    //
	//// color
	//GLvoid* pCol = reinterpret_cast<GLvoid*>(glfloat3::dim*sizeof(GLfloat));
	//GLint inCol = glGetAttribLocation(m_shaderProgram, "in_Color");
	//glEnableVertexAttribArray(inCol);
	//glVertexAttribPointer(inCol, CGLColor::dim, GL_FLOAT, GL_FALSE, stride, pCol);
    //
	//// normal
	//if (bUseShading)
	//{
	//	GLvoid* pNm = reinterpret_cast<GLvoid*>((glfloat3::dim + CGLColor::dim)*sizeof(GLfloat));
	//	GLint inNm = glGetAttribLocation(m_shaderProgram, "in_Normal");
	//	glEnableVertexAttribArray(inNm);
	//	glVertexAttribPointer(inNm, glfloat3::dim, GL_FLOAT, GL_FALSE, stride, pNm);
	//}

	//m_unUseShading = glGetUniformLocation(m_shaderProgram, "un_UseShading");
	m_unLightPos = glGetUniformLocation(m_shaderProgram, "un_LightPosition");
	m_unLightCol = glGetUniformLocation(m_shaderProgram, "un_LightColor");
	m_unMVP = glGetUniformLocation(m_shaderProgram, "un_MVP");
}

void CGLManager::releaseShaderProgram()
{
	//glDisableVertexAttribArray(m_inPos);
	//glDisableVertexAttribArray(m_inNm);
	//glDisableVertexAttribArray(m_inCol);
	glDetachShader(m_shaderProgram, m_fgShader);
	glDetachShader(m_shaderProgram, m_vxShader);
	glDeleteProgram(m_shaderProgram);
	glDeleteShader(m_fgShader);
	glDeleteShader(m_vxShader);
}

void CGLManager::drawScene()
{
	glUniform3fv(m_unLightPos, 1, m_lightPos.begin());
	glUniform4fv(m_unLightCol, 1, m_lightCol.begin());
	glUniformMatrix4fv(m_unMVP, 1, GL_FALSE, &m_MVP.mvp()[0][0]);
	//glUniform1ui(m_unUseShading, 0);

	//if (m_bDrawVx)
	//{
	//	glBindVertexArray(m_vaoVx);
	//	glDrawArrays(GL_POINTS, 0, 4);
	//}

	//if (m_bDrawEd)
	//{
	//	glBindVertexArray(m_vaoEd);
	//	glDrawElements(GL_LINES, 12, GL_UNSIGNED_INT, 0);
	//}

	//if (m_bDrawFc)
	//{
	//	glUniform1ui(m_unUseShading, 1);
	//	glBindVertexArray(m_vaoFc);
	//	glDrawArrays(GL_TRIANGLES, 0, 12);
	//}

    m_object->draw();
}

void CGLManager::scale(double factor)
{
	if (factor > 0)
		m_MVP.addScaling(vec3(static_cast<float>(factor*16)));
	else
		m_MVP.addScaling(vec3(static_cast<float>(-1/factor/16)));
}

void CGLManager::move(const double2& shift)
{
	m_MVP.addTranslation(vec3(-shift.x() / m_winSize.x(), -shift.y() / m_winSize.y(), 0.0));
}

void CGLManager::rotate(const double2& shift)
{
	m_MVP.addRotation(vec3(dToR(shift.y()), dToR(shift.x()), 0.0));
}