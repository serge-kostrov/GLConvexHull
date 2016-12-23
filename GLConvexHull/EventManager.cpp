#include "EventManager.h"
#include "GLManager.h"


CEventManager* CEventManager::m_instance = nullptr;
const double CEventManager::m_maxScrollOffset = 15.0;

CEventManager* CEventManager::instance()
{
	if (!m_instance)
	{
		m_instance = new CEventManager();
		m_instance->m_isMouseLeftButtonPressed = false;
		m_instance->m_isMouseRightButtonPressed = false;
		m_instance->m_posMouse = { 0.0 };
		m_instance->m_willWindowClose = false;
	}

	return m_instance;
}

void CEventManager::setEventHandlers(GLFWwindow* window) const
{
	glfwSetErrorCallback(callbackError);
	glfwSetScrollCallback(window, callbackMouseScroll);
	glfwSetCursorPosCallback(window, callbackMousePosition);
	glfwSetMouseButtonCallback(window, callbackMouseButton);
	glfwSetKeyCallback(window, callbackKeyboardButton);
}

bool CEventManager::willWindowClose() const
{
	return m_willWindowClose;
}

void CEventManager::callbackError(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void CEventManager::callbackMouseScroll(GLFWwindow* window, double offsetX, double offsetY)
{
	const double scale = offsetY/m_maxScrollOffset;
	CGLManager::instance()->scale(scale);
}

void CEventManager::callbackMousePosition(GLFWwindow* window, double posX, double posY)
{
	double2& posOld = m_instance->m_posMouse;
	const double2 shift = { posX - posOld.x(), posY - posOld.y() };
	if (m_instance->m_isMouseLeftButtonPressed)
		CGLManager::instance()->rotate(shift);
	else if (m_instance->m_isMouseRightButtonPressed)
		CGLManager::instance()->move(shift);
	posOld = { posX, posY };
}

void CEventManager::callbackMouseButton(GLFWwindow* window, int button, int action, int mod)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		bool& isMousePressed = m_instance->m_isMouseLeftButtonPressed;
		if (action == GLFW_PRESS)
			isMousePressed = true;
		else if (action == GLFW_RELEASE)
			isMousePressed = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		bool& isMousePressed = m_instance->m_isMouseRightButtonPressed;
		if (action == GLFW_PRESS)
			isMousePressed = true;
		else if (action == GLFW_RELEASE)
			isMousePressed = false;
	}
}

void CEventManager::callbackKeyboardButton(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		m_instance->m_willWindowClose = true;
}