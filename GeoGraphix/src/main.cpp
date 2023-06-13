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
#include "HeightMap/HeightMapDiamondSquare.h"
#include "HeightMap/HeightMapSimplex.h"
#include "HeightMap/HeightMapOctaves.h"
#include "Mesh.h"

    int main()
{
    unsigned int screenWidth = 1280;
    unsigned int screenHeight = 720;
    float aspectRatio = (float) screenWidth / screenHeight;
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
        PERLIN,
        DIAMOND_SQUARE,
        SIMPLEX
    };

    int octave1Mode = PERLIN;
    int octave2Mode = RANDOM;
    int octave3Mode = RANDOM;

    bool octave1Active = true;
    bool octave2Active = true;
    bool octave3Active = true;

    /*HeightMap terrainHeightMap = HeightMapRandom(mapWidth, mapLength);*/
    HeightMap terrainHeightMap1 = HeightMapPerlin(mapWidth, mapLength);
    HeightMap terrainHeightMap2 = HeightMapRandom(mapWidth, mapLength);
    HeightMap terrainHeightMap3 = HeightMapRandom(mapWidth, mapLength);

    HeightMapOctaves octaves = HeightMapOctaves(mapWidth, mapLength);
    octaves.AddOctave(std::pair<bool, HeightMap*>(octave1Active, &terrainHeightMap1));
    octaves.AddOctave(std::pair<bool, HeightMap*>(octave2Active, &terrainHeightMap2));
    octaves.AddOctave(std::pair<bool, HeightMap*>(octave3Active, &terrainHeightMap3));

    Mesh terrainMesh((HeightMap)octaves);

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
    float FOV = 45.0f;
    double yaw = -90.0;
    double pitch = -45.0;
    glm::vec3 cameraPosition = { 0.0f, 20.0f, 25.0f };
    Camera camera(cameraPosition, yaw, pitch);

    float rotationAngle = 45.0f; // rotation angle of the terrain mesh
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projMatrix = glm::perspective(glm::radians(FOV), aspectRatio, 0.1f, 1000.0f);

    // upload uniforms
    shader.Bind();
    shader.SetUniformMat4f("u_Model", modelMatrix);
    shader.SetUniformMat4f("u_View", camera.GetViewMatrix());
    shader.SetUniformMat4f("u_Projection", projMatrix);

    // openGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // wirefrmae mode
    bool wireframe = false;

    // display water
    bool water = false;
    HeightMap waterHeightMap = HeightMapPerlin(100, 100, 0.1f, 0.2f);
    Mesh waterMesh(waterHeightMap);
    VertexArray waterVA;
    VertexBuffer waterVB(waterMesh.m_Vertices, 2 * 3 * 100 * 100 * sizeof(float), DRAW_MODE::STATIC);
    waterVA.AddBuffer(waterVB, layout);
    IndexBuffer waterIB(waterMesh.m_Indices, 6 * 99 * 99, DRAW_MODE::STATIC);

    // auto rotate
    bool rot = false;

    GLFWwindow* windowID = window.GetID();

    // input initialization
    Input::Init(windowID);

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
        if (Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_1) && !ImGui::GetIO().WantCaptureMouse)
        {
            rotationAngle = (float) deltaX * sens;
            rotationAngle > 360.0f ? rotationAngle -= 360.0f : NULL;
            modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else if (rot)
        {
            rotationAngle = 0.05f;
            modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        shader.SetUniformMat4f("u_Model", modelMatrix);
        
        // adjust FOV using vertical scroll
        FOV -= Input::GetScrollY() * 2.0f;
        FOV < 25.0f ? FOV = 25.0f : NULL;
        FOV > 65.0f ? FOV = 65.0f : NULL;
        projMatrix = glm::perspective(glm::radians(FOV), aspectRatio, 0.1f, 1000.0f);
        shader.SetUniformMat4f("u_Projection", projMatrix);
        Input::ResetScroll();

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

        ImGui::SliderInt("Width", &mapWidth, 5, 50);
        ImGui::SliderInt("Length", &mapLength, 5, 50);
        if (ImGui::Button("Regenerate"))
        {
            octaves.Reset();
            switch (octave1Mode)
            {
            case UNIFORM:
                terrainHeightMap1 = HeightMapUniform(mapWidth, mapLength, 0.5f);
                break;
            case RANDOM:
                terrainHeightMap1 = HeightMapRandom(mapWidth, mapLength);
                break;
            case PERLIN:
                terrainHeightMap1 = HeightMapPerlin(mapWidth, mapLength);
                break;
            case DIAMOND_SQUARE:
                terrainHeightMap1 = HeightMapDiamondSquare(mapWidth, mapLength);
                break;
            case SIMPLEX:
                terrainHeightMap1 = HeightMapSimplex(mapWidth, mapLength);
                break;
            }
            octaves.AddOctave(std::pair<bool, HeightMap*>(octave1Active, &terrainHeightMap1));

            switch (octave2Mode)
            {
                case UNIFORM:
                    terrainHeightMap2 = HeightMapUniform(mapWidth, mapLength, 0.5f);
                    break;
                case RANDOM:
                    terrainHeightMap2 = HeightMapRandom(mapWidth, mapLength);
                    break;
                case PERLIN:
                    terrainHeightMap2 = HeightMapPerlin(mapWidth, mapLength);
                    break;
                case DIAMOND_SQUARE:
                    terrainHeightMap2 = HeightMapDiamondSquare(mapWidth, mapLength);
                    break;
                case SIMPLEX:
                    terrainHeightMap2 = HeightMapSimplex(mapWidth, mapLength);
                    break;
            }
            octaves.AddOctave(std::pair<bool, HeightMap*>(octave2Active, &terrainHeightMap2));

            switch (octave3Mode)
            {
                case UNIFORM:
                    terrainHeightMap3 = HeightMapUniform(mapWidth, mapLength, 0.5f);
                    break;
                case RANDOM:
                    terrainHeightMap3 = HeightMapRandom(mapWidth, mapLength);
                    break;
                case PERLIN:
                    terrainHeightMap3 = HeightMapPerlin(mapWidth, mapLength);
                    break;
                case DIAMOND_SQUARE:
                    terrainHeightMap3 = HeightMapDiamondSquare(mapWidth, mapLength);
                    break;
                case SIMPLEX:
                    terrainHeightMap3 = HeightMapSimplex(mapWidth, mapLength);
                    break;
            }
            octaves.AddOctave(std::pair<bool, HeightMap*>(octave3Active, &terrainHeightMap3));

            terrainMesh.Regenerate((HeightMap)octaves);
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
        ImGui::Checkbox("Auto Rotate", &rot); // toggle auto rotate

        ImGui::End();

        ImGui::Begin("Octaves");

        ImGui::Checkbox("Octave 1 active", &octave1Active);
        ImGui::Text("Octave 1 Type:");
        ImGui::RadioButton("Random", &octave1Mode, RANDOM);
        ImGui::RadioButton("Uniform", &octave1Mode, UNIFORM);
        ImGui::RadioButton("Perlin Noise", &octave1Mode, PERLIN);
        ImGui::RadioButton("Diamond Square", &octave1Mode, DIAMOND_SQUARE);
        ImGui::RadioButton("Simplex Noise", &octave1Mode, SIMPLEX);
        ImGui::Separator();

        ImGui::Checkbox("Octave 2 active", &octave2Active);
        ImGui::Text("Octave 2 Type:");
        ImGui::RadioButton("Random 2", &octave2Mode, RANDOM);
        ImGui::RadioButton("Uniform 2", &octave2Mode, UNIFORM);
        ImGui::RadioButton("Perlin Noise 2", &octave2Mode, PERLIN);
        ImGui::RadioButton("Diamond Square 2", &octave2Mode, DIAMOND_SQUARE);
        ImGui::RadioButton("Simplex Noise 2", &octave2Mode, SIMPLEX);
        ImGui::Separator();

        ImGui::Checkbox("Octave 3 active", &octave3Active);
        ImGui::Text("Octave 3 Type:");
        ImGui::RadioButton("Random 3", &octave3Mode, RANDOM);
        ImGui::RadioButton("Uniform 3", &octave3Mode, UNIFORM);
        ImGui::RadioButton("Perlin Noise 3", &octave3Mode, PERLIN);
        ImGui::RadioButton("Diamond Square 3", &octave3Mode, DIAMOND_SQUARE);
        ImGui::RadioButton("Simplex Noise 3", &octave3Mode, SIMPLEX);

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