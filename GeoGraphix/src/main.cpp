#include <iostream>
#include <string>
#include <vector>

#include "Window.h"
#include "renderer/VertexBuffer.h"
#include "renderer/VertexArray.h"
#include "renderer/Shader.h"
#include "renderer/Camera.h"

int main()
{
    float screenWidth = 1280.0f;
    float screenHeight = 720.0f;
    float aspectRatio = screenWidth / screenHeight;
    Window window(screenWidth, screenHeight, "GeoGraphix", NULL);

    VertexBufferLayout layout;
    layout.Push<float>(3); // 3d coordinates
    layout.Push<float>(4); // colors

    static const float rect[]
    {
        -10.0f,  0.0f, -10.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        10.0f,  0.0f, -10.0f, 
        1.0f, 0.0f, 0.0f, 1.0f,
        10.0f,  0.0f, 10.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        -10.0f,  0.0f, 10.0f,
        1.0f, 0.0f, 0.0f, 1.0f
    };

    unsigned int indices[]
    {
        0, 1, 2,
        2, 3, 0
    };
    
    VertexArray rectVA;
    VertexBuffer rectVB(rect, sizeof(rect), MODE::STATIC);
    // bind vertex buffer to vertex array
    rectVA.AddBuffer(rectVB, layout);

    unsigned int rectIB;
    glGenBuffers(1, &rectIB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectIB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // shaders
    std::string vertexFilepath = "res/shaders/vertex.shader";
    std::string fragmentFilepath = "res/shaders/fragment.shader";

    Shader shader(vertexFilepath, fragmentFilepath);

    // camera setup
    double yaw = -135.0; // initially looks at the origin
    double pitch = -30.0;
    glm::vec3 cameraPosition = { 5.0f, 5.0f, 5.0f };
    Camera camera(cameraPosition, yaw, pitch);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 projMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

    // upload uniforms
    shader.Bind();
    shader.SetUniformMat4f("u_Model", modelMatrix);
    shader.SetUniformMat4f("u_View", camera.GetViewMatrix());
    shader.SetUniformMat4f("u_Projection", projMatrix);

    // openGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLFWwindow* windowID = window.GetID();

    // keyboard movement variables
    double currentTime = 0.0;
    double lastTime = 0.0;
    float deltaTime = 0.0f;
    float forwardSpeed = 5.0f;
    float strafeSpeed = 50.0f;

    // mouse movement variables
    double currXpos, currYpos, deltaX, deltaY;
    double lastXpos = 0.0;
    double lastYpos = 0.0;
    double sens = 15.0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(windowID))
    {
        lastTime = currentTime;
        currentTime = glfwGetTime();
        deltaTime = float(currentTime - lastTime);

        // camera updates per frame
        camera.LookAt(yaw, pitch);
        camera.SetViewMatrix();

        // upload uniforms
        shader.Bind();
        shader.SetUniformMat4f("u_View", camera.GetViewMatrix());

        // keyboard input
        // Move forward
        if (glfwGetKey(windowID, GLFW_KEY_W) == GLFW_PRESS)
        {
            /*cameraPosition += cameraFront * deltaTime * forwardSpeed;*/
            camera.MoveCamera(camera.GetCameraFront(), deltaTime * forwardSpeed);
        }
        // Move backward
        if (glfwGetKey(windowID, GLFW_KEY_S) == GLFW_PRESS)
        {
            camera.MoveCamera(-camera.GetCameraFront(), deltaTime * forwardSpeed);
        }
        // Strafe left
        if (glfwGetKey(windowID, GLFW_KEY_A) == GLFW_PRESS)
        {
            /*cameraPosition += cameraRight * deltaTime * strafeSpeed;*/
            camera.MoveCamera(camera.GetCameraRight(), deltaTime* forwardSpeed);
        }
        // Strafe right
        if (glfwGetKey(windowID, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera.MoveCamera(-camera.GetCameraRight(), deltaTime * forwardSpeed);
        }
        // fly up
        if (glfwGetKey(windowID, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            camera.MoveCamera(camera.GetCameraUp(), deltaTime * forwardSpeed);
        }
        // drop down
        if (glfwGetKey(windowID, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            camera.MoveCamera(-camera.GetCameraUp(), deltaTime * forwardSpeed);
        }

        // pitch up
        if (glfwGetKey(windowID, GLFW_KEY_UP) == GLFW_PRESS)
        {
            pitch += sens * deltaTime;
        }
        // pitch down
        if (glfwGetKey(windowID, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            pitch -= sens * deltaTime;
        }
        // yaw left
        if (glfwGetKey(windowID, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            yaw -= sens * deltaTime;
        }
        // yaw right
        if (glfwGetKey(windowID, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            yaw += sens * deltaTime;
        }

        // mouse movement
        glfwGetCursorPos(windowID, &currXpos, &currYpos);
        deltaX = (currXpos - lastXpos) / screenWidth;  // it is bounded by -1 and 1
        deltaY = (currYpos - lastYpos) / screenHeight; // it is bounded by -1 and 1
        lastXpos = currXpos;
        lastYpos = currYpos;

        if (glfwGetMouseButton(windowID, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            yaw -= deltaX * sens;
            pitch += deltaY * sens;
        }

        // clearing per frame
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Render here */
        rectVA.Bind();
        shader.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(windowID);

        /* Poll for and process events */
        glfwPollEvents();
    }

    window.~Window();
    return 0;
}