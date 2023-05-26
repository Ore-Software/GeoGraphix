#include "Input.h"

bool Input::IsKeyDown(int keycode)
{
    return glfwGetKey(m_WindowID, keycode) == GLFW_PRESS;
}

bool Input::IsMouseButtonDown(int button)
{
    return glfwGetMouseButton(m_WindowID, button) == GLFW_PRESS;
}