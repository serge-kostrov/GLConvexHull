#pragma once
#include "stdheaders.h"
#include "base.h"
#include <glew/glew.h>
#include <GLFW/glfw3.h>

class CEventManager
{
public:
	~CEventManager() {}

	static CEventManager* instance();
	void setEventHandlers(GLFWwindow* window) const;
	bool willWindowClose() const;

private:
	CEventManager() {}
	CEventManager(const CEventManager& other);
	void operator=(const CEventManager& other);

	static void callbackError(int error, const char* description);
	static void callbackMouseScroll(GLFWwindow* window, double offsetX, double offsetY);
	static void callbackMousePosition(GLFWwindow* window, double posX, double posY);
	static void callbackMouseButton(GLFWwindow* window, int button, int action, int mod);
	static void callbackKeyboardButton(GLFWwindow* window, int key, int scancode, int action, int mod);

private:
	static CEventManager* m_instance;
	static const double m_maxScrollOffset;
	bool m_isMouseLeftButtonPressed;
	bool m_isMouseRightButtonPressed;
	double2 m_posMouse;
	bool m_willWindowClose;
};

