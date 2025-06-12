#pragma once

#include "framework.h"

class Window {

public:
	Window(const char* title, const int width, const int height);
	virtual ~Window();

	GLFWwindow* GetWindow();


protected:

	GLFWwindow* m_window;
	const char* m_title;
	const int m_width;
	const int m_height;

};