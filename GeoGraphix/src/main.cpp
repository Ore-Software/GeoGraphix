#include <iostream>
#include <string>
#include <vector>

#include "Window.h"
#include "Input.h"
#include "renderer/VertexArray.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Shader.h"
#include "renderer/Camera.h"
#include "HeightMap/HeightMap.h"
#include "HeightMap/HeightMapUniform.h"
#include "HeightMap/HeightMapRandom.h"

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

    //HeightMap map = HeightMapUniform(mapWidth, mapLength, 1.0f);
    HeightMap map = HeightMapRandom(mapWidth, mapLength);

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
    double yaw = -90.0;
    double pitch = -45.0;
    glm::vec3 cameraPosition = { 0.0f, 20.0f, 25.0f };
    Camera camera(cameraPosition, yaw, pitch);

    float rotationAngle = 45.0f; // rotation angle of the terrain mesh
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

    GLFWwindow* windowID = window.GetID();

    // input initialization
    Input input;
    input.m_WindowID = windowID;

    // keyboard movement variables
    double currentTime = 0.0;
    double lastTime = 0.0;
    float deltaTime = 0.0f;

    // mouse movement variables
    double currXpos, currYpos, deltaX, deltaY;
    double lastXpos = 0.0;
    double lastYpos = 0.0;
    double sens = 200.0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(windowID))
    {
        lastTime = currentTime;
        currentTime = glfwGetTime();
        deltaTime = float(currentTime - lastTime);

        // mouse movement
        glfwGetCursorPos(windowID, &currXpos, &currYpos);
        deltaX = (currXpos - lastXpos) / screenWidth;  // it is bounded by -1 and 1
        deltaY = (currYpos - lastYpos) / screenHeight; // it is bounded by -1 and 1
        lastXpos = currXpos;
        lastYpos = currYpos;

        // rotate model according to mouse movement
        if (input.IsMouseButtonDown(GLFW_MOUSE_BUTTON_1))
        {
            rotationAngle = deltaX * sens;
            rotationAngle > 360.0f ? rotationAngle -= 360.0f : NULL;
            modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        shader.SetUniformMat4f("u_Model", modelMatrix);

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