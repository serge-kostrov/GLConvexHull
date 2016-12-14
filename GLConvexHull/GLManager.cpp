#include "stdafx.h"
#include "GLManager.h"
#include "constants.h"
#include "GLColor.h"
#include "IOUtilities.h"
#include "geometrytools.h"


namespace
{;

void errorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

}

CGLManager* CGLManager::m_instance = nullptr;

CGLManager* CGLManager::instance()
{
	using namespace std;
	if (!m_instance)
	{
		m_instance = new CGLManager();
		m_instance->m_winSize = { 720, 720 };
		vector<GLfloat>& aVx = m_instance->m_aVx;
		vector<tuple<glfloat3, CGLColor, glfloat3>> aVxAttr;

		const int nPt = 4;
		array<glfloat3, nPt> aPt;
		aPt[0] = { 0.0f, 0.5f, 1.0f };
		aPt[1] = { 0.5f, -0.5f, 1.0f };
		aPt[2] = { -0.5f, -0.5f, 1.0f };
		aPt[3] = { 0.0f, 0.0f, 1.5f };
		const int nNm = 4;
		array<glfloat3, nNm> aNm;
		
		aNm[0] = ((aPt[1] - aPt[0]) % (aPt[2] - aPt[0])).normalized();
		aNm[1] = ((aPt[3] - aPt[0]) % (aPt[1] - aPt[0])).normalized();
		aNm[2] = ((aPt[2] - aPt[0]) % (aPt[3] - aPt[0])).normalized();
		aNm[3] = ((aPt[1] - aPt[2]) % (aPt[3] - aPt[2])).normalized();

		aVxAttr.push_back(make_tuple(glfloat3{ 0.0, 0.5, 1.0 }, CGLColor::gray, aNm[0]));
		aVxAttr.push_back(make_tuple(glfloat3{ 0.5, -0.5, 1.0 }, CGLColor::gray, aNm[1]));
		aVxAttr.push_back(make_tuple(glfloat3{ -0.5, -0.5, 1.0 }, CGLColor::gray, aNm[2]));
		aVxAttr.push_back(make_tuple(glfloat3{ 0.0, 0.0, 1.5 }, CGLColor::gray, aNm[3]));

		aVxAttr.push_back(make_tuple(glfloat3{ 0.0, 0.5, 1.0 }, CGLColor::black, aNm[0]));
		aVxAttr.push_back(make_tuple(glfloat3{ 0.5, -0.5, 1.0 }, CGLColor::black, aNm[1]));
		aVxAttr.push_back(make_tuple(glfloat3{ -0.5, -0.5, 1.0 }, CGLColor::black, aNm[2]));
		aVxAttr.push_back(make_tuple(glfloat3{ 0.0, 0.0, 1.5 }, CGLColor::black, aNm[3]));

		vector<GLuint>& aInd = m_instance->m_aInd;
		aInd = { 0, 1, 2, 0, 1, 3, 0, 3, 2, 3, 2, 1 };

		vector<GLuint>& aIndEd = m_instance->m_aIndEd;
		aIndEd = { 4, 5, 5, 6, 6, 4, 4, 7, 5, 7, 6, 7 };

		aInd.insert(aInd.end(), aIndEd.begin(), aIndEd.end());

		for (int i = 0; i < aVxAttr.size(); ++i)
		{
			const auto& vx = aVxAttr[i];
			for (auto j : get<0>(vx))
				aVx.push_back(j);
			for (auto j : get<1>(vx))
				aVx.push_back(j);
			for (auto j : get<2>(vx))
				aVx.push_back(j);
		}

		m_instance->m_lightPos = glfloat3{ 0.0f, 0.0f, 0.9f };
		m_instance->m_lightCol = CGLColor{ 1.0f, 1.0f, 1.0f, 1.0f };
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
	initializeBuffers();
	if (!initializeShaderProgram())
		return false;

	m_MVP.setProjection();

	return true;
}

void CGLManager::releaseGL()
{
	releaseShaderProgram();
	releaseBuffers();	
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*m_aInd.size(), m_aInd.data(), GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_aVx.size(), m_aVx.data(), GL_DYNAMIC_DRAW);
	glClearColor(CGLColor::white.r(), CGLColor::white.g(), CGLColor::white.b(), CGLColor::white.a());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();
}

void CGLManager::updateGL()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void CGLManager::initializeBuffers()
{
	glGenBuffers(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
}

void CGLManager::releaseBuffers()
{
	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
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

	const GLsizei stride = (glfloat3::dim + CGLColor::dim + glfloat3::dim)*sizeof(GLfloat);
	GLint inPos = glGetAttribLocation(m_shaderProgram, "in_Position");
	glEnableVertexAttribArray(inPos);
	glVertexAttribPointer(inPos, glfloat3::dim, GL_FLOAT, GL_FALSE, stride, 0);

	GLint inCol = glGetAttribLocation(m_shaderProgram, "in_Color");
	glEnableVertexAttribArray(inCol);
	glVertexAttribPointer(inCol, CGLColor::dim, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(glfloat3::dim*sizeof(GLfloat)));	

	GLint inNm = glGetAttribLocation(m_shaderProgram, "in_Normal");
	glEnableVertexAttribArray(inNm);
	glVertexAttribPointer(inNm, glfloat3::dim, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>((glfloat3::dim + CGLColor::dim)*sizeof(GLfloat)));

	m_unLightPos = glGetUniformLocation(m_shaderProgram, "un_LightPosition");
	m_unLightCol = glGetUniformLocation(m_shaderProgram, "un_LightColor");
	m_unMVP = glGetUniformLocation(m_shaderProgram, "un_MVP");

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

void CGLManager::releaseShaderProgram()
{
	glDeleteProgram(m_shaderProgram);
	glDeleteShader(m_fgShader);
	glDeleteShader(m_vxShader);
}

void CGLManager::drawScene()
{
	glUniform3fv(m_unLightPos, 1, m_lightPos.begin());
	glUniform4fv(m_unLightCol, 1, m_lightCol.begin());
	glUniformMatrix4fv(m_unMVP, 1, GL_FALSE, &m_MVP.mvp()[0][0]);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_LINES, 12, GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(12*sizeof(GLuint)));
}

void CGLManager::scaleVx(double factor)
{
	if (factor > 0)
		m_MVP.addScaling(vec3(static_cast<float>(factor*16)));
	else
		m_MVP.addScaling(vec3(static_cast<float>(-1/factor/16)));
}

void CGLManager::moveVx(const double2& shift)
{
	m_MVP.addTranslation(vec3(shift.x() / m_winSize.x(), -shift.y() / m_winSize.y(), 0.0));
}

void CGLManager::rotateVx(const double2& shift)
{
	m_MVP.addRotation(vec3(dToR(shift.y()), dToR(shift.x()), 0.0));
}