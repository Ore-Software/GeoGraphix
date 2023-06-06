#include <iostream>
#include <string>
#include <vector>

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_glfw.h"
#include "external/imgui/imgui_impl_opengl3.h"

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
#include "HeightMap/HeightMapPerlin.h"
#include "Mesh.h"

int main()
{
    float screenWidth = 1280.0f;
    float screenHeight = 720.0f;
    float aspectRatio = screenWidth / screenHeight;
    Window window(screenWidth, screenHeight, "GeoGraphix", NULL);

    VertexBufferLayout layout;
    layout.Push<float>(3); // 3d coordinates
    layout.Push<float>(3); // normals

    int mapWidth = 25;
    int mapLength = 25;

     enum algoMode
    {
        UNIFORM,
        RANDOM,
        PERLIN
    };

    int currMode = RANDOM;

    HeightMap terrainHeightMap = HeightMapRandom(mapWidth, mapLength);

    Mesh terrainMesh(terrainHeightMap);

    VertexArray terrainVA;
    VertexBuffer terrainVB(terrainMesh.m_Vertices, 2 * 3 * mapWidth * mapLength * sizeof(float), DRAW_MODE::STATIC);
    // bind vertex buffer to vertex array
    terrainVA.AddBuffer(terrainVB, layout);
    IndexBuffer terrainIB(terrainMesh.m_Indices, 6 * (mapWidth - 1) * (mapLength - 1), DRAW_MODE::STATIC);

    // shaders
    std::string vertexFilepath = "res/shaders/terrain.vert";
    std::string fragmentFilepath = "res/shaders/terrain.frag";

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

    bool wireframe = false;

    // display water
    bool water = false;
    HeightMap waterHeightMap = HeightMapPerlin(100, 100, 0.1f, 0.2f);
    Mesh waterMesh(waterHeightMap);
    VertexArray waterVA;
    VertexBuffer waterVB(waterMesh.m_Vertices, 2 * 3 * 100 * 100 * sizeof(float), DRAW_MODE::STATIC);
    waterVA.AddBuffer(waterVB, layout);
    IndexBuffer waterIB(waterMesh.m_Indices, 6 * 99 * 99, DRAW_MODE::STATIC);

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

    // Setup Dear ImGui context
    ImGui::CreateContext();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(windowID, true);
    ImGui_ImplOpenGL3_Init();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

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
        if (input.IsMouseButtonDown(GLFW_MOUSE_BUTTON_1) && !ImGui::GetIO().WantCaptureMouse)
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
        terrainVA.Bind();
        shader.Bind();
        shader.SetUniform1i("u_Is_Water", 0);
        glDrawElements(GL_TRIANGLES, terrainIB.GetCount(), GL_UNSIGNED_INT, 0);
        if (water) // render water
        {
            waterVA.Bind();
            shader.SetUniform1i("u_Is_Water", 1);
            glDrawElements(GL_TRIANGLES, waterIB.GetCount(), GL_UNSIGNED_INT, 0);
        }

        // imgui new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Regenerate Terrain");

        ImGui::Text("Height Map Type:");
        ImGui::RadioButton("Random", &currMode, RANDOM);
        ImGui::RadioButton("Uniform", &currMode, UNIFORM);
        ImGui::RadioButton("Perlin Noise", &currMode, PERLIN);

        ImGui::SliderInt("Width", &mapWidth, 5, 50);
        ImGui::SliderInt("Length", &mapLength, 5, 50);
        if (ImGui::Button("Regenerate"))
        {
            switch (currMode)
            {
                case UNIFORM:
                    terrainHeightMap = HeightMapUniform(mapWidth, mapLength, 0.5f);
                    break;
                case RANDOM:
                    terrainHeightMap = HeightMapRandom(mapWidth, mapLength);
                    break;
                case PERLIN:
                    terrainHeightMap = HeightMapPerlin(mapWidth, mapLength);
                    break;
            }
            terrainMesh.Regenerate(terrainHeightMap);
            terrainVA.Bind(); // need to bind correct VA, otherwise it may add to waterVA
            terrainVB.AssignData(terrainMesh.m_Vertices, 2 * 3 * mapWidth * mapLength * sizeof(float), DRAW_MODE::STATIC);
            terrainIB.AssignData(terrainMesh.m_Indices, 6 * (mapWidth - 1) * (mapLength - 1), DRAW_MODE::STATIC);
        }

        ImGui::Text("Options:");
        if (ImGui::Checkbox("Wireframe", &wireframe))
        {
            if (wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // surface mode
        }
        ImGui::Checkbox("Water", &water); // display water

        ImGui::End();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(windowID);

        /* Poll for and process events */
        glfwPollEvents();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    window.~Window();
    return 0;
}