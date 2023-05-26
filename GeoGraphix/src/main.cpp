#include <iostream>
#include <string>
#include <vector>

#include "Window.h"
#include "renderer/VertexArray.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Shader.h"
#include "renderer/Camera.h"
#include "HeightMap/HeightMap.h"
#include "HeightMap/HeightMapUniform.h"

int main()
{
    float screenWidth = 1280.0f;
    float screenHeight = 720.0f;
    float aspectRatio = screenWidth / screenHeight;
    Window window(screenWidth, screenHeight, "GeoGraphix", NULL);

    VertexBufferLayout layout;
    layout.Push<float>(3); // 3d coordinates

    const int mapWidth = 100;
    const int mapLength = 100;

    HeightMapUniform map = HeightMapUniform(mapWidth, mapLength, 1.0f);

    // make the map from -10.0f to 10.0f, regardless of how many sample points are on the map
    float heightMapVert[3 * mapWidth * mapLength]{}; // 3D location of each point
    for (int j = 0; j < mapLength; j++)
    {
        for (int i = 0; i < mapWidth; i++)
        {
            // x value of the (i,j) point
            heightMapVert[3 * mapWidth * j + 3 * i + 0] = 20.0f * i / (mapWidth - 1) - 10.0f;
            // y value of the (i,j) point
            heightMapVert[3 * mapWidth * j + 3 * i + 1] = map.m_Map[j * mapWidth + i];
            // z value of the (i,j) point
            heightMapVert[3 * mapWidth * j + 3 * i + 2] = 20.0f * j / (mapLength - 1) - 10.0f;
        }
    }

    // if there are mapWidth x mapLength points, there will be (mapWidth - 1) x (mapLength - 1) squares, so 6 * (mapWidth - 1) x (mapLength - 1) indices
    unsigned int heightMapIndices[6 * (mapWidth - 1) * (mapLength - 1)]{};
    for (int j = 0; j < mapLength - 1; j++)
    {
        for (int i = 0; i < mapWidth - 1; i++)
        {
            // first triangle of the quad (bottom right)
            heightMapIndices[6 * (mapWidth - 1) * j + 6 * i + 0] = j * mapWidth + i;
            heightMapIndices[6 * (mapWidth - 1) * j + 6 * i + 1] = j * mapWidth + (i + 1);
            heightMapIndices[6 * (mapWidth - 1) * j + 6 * i + 2] = (j + 1) * mapWidth + (i + 1);

            // second triangle of the quad (top left)
            heightMapIndices[6 * (mapWidth - 1) * j + 6 * i + 3] = (j + 1) * mapWidth + (i + 1);
            heightMapIndices[6 * (mapWidth - 1) * j + 6 * i + 4] = (j + 1) * mapWidth + i;
            heightMapIndices[6 * (mapWidth - 1) * j + 6 * i + 5] = j * mapWidth + i;
        }
    }

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
    
    VertexArray VA;
    VertexBuffer VB(heightMapVert, sizeof(heightMapVert), DRAW_MODE::STATIC);
    // bind vertex buffer to vertex array
    VA.AddBuffer(VB, layout);
    IndexBuffer IB(heightMapIndices, sizeof(heightMapIndices) / sizeof(unsigned int), DRAW_MODE::STATIC);

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
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        glClearColor(0.80f, 0.90f, 0.96f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Render here */
        VA.Bind();
        shader.Bind();
        glDrawElements(GL_TRIANGLES, IB.GetCount(), GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(windowID);

        /* Poll for and process events */
        glfwPollEvents();
    }

    window.~Window();
    return 0;
}