#pragma once

#include "GLFW/glfw3.h"

class Input
{
public:
	bool IsKeyDown(int keycode);
	bool IsMouseButtonDown(int button);

	GLFWwindow* m_WindowID;
};