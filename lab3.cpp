#include "lab_m1/lab3/lab3.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab3::Lab3()
{
    dirY = UP;
    size = UP;
}


Lab3::~Lab3()
{}


void Lab3::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    CreateHeightMap(nrPoints);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;

    cx = corner.x + (squareSide - corner.x) / 2;
    cy = corner.y + (squareSide - corner.y) / 2;

    translateX = 0;
    translateY = 0;
    scaleX = 1;
    scaleY = 1;
    angularStep = 0;

    // Create vertices for the triangle strip
    vector<VertexFormat> vertices;
    vertices.reserve(yValues.size() * 2);

    glm::vec3 groundColor(1.0 / 255 * 120, 1.0 / 255 * 150, 1.0 / 255 * 100);

    for (int i = 0;i< nrPoints; i++)
    {
        vertices.emplace_back(glm::vec3(xValues[i], yValues[i], 0), groundColor);
        vertices.emplace_back(glm::vec3(xValues[i], 0, 0), groundColor);
    }

    // Define indices for the triangle strip
    vector<unsigned int> indices;
    indices.reserve(vertices.size());
    for (unsigned int i = 0; i < vertices.size(); ++i)
    {
        indices.push_back(i);
    }

    // Create and initialize the triangle strip mesh
    Mesh* triangleStrip = new Mesh("triangle_strip");
    triangleStrip->SetDrawMode(GL_TRIANGLE_STRIP);
    triangleStrip->InitFromData(vertices, indices);
    AddMeshToList(triangleStrip);
}


void Lab3::FrameStart()
{
    glm::vec3 skyColor(1.0 / 255 * 135, 1.0 / 255 * 206, 1.0 / 255 * 255);
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(skyColor.r, skyColor.g, skyColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab3::Update(float deltaTimeSeconds)
{
    // Update translation
    modelMatrix = glm::mat3(1);

    // modelMatrix *= transform2D::Translate(150, 250);

    // switch (dirY)
    // {
    // case UP:
    // {
    //     translateY += 100 * deltaTimeSeconds; // Move up
    //     break;
    // }
    // case DOWN:
    // {
    //     translateY -= 100 * deltaTimeSeconds; // Move down
    //     break;
    // }
    // }
    // if (translateY > 200)
    // {
    //     dirY = DOWN;
    // }
    // if (translateY < -200)
    // {
    //     dirY = UP;
    // }
    // modelMatrix *= transform2D::Translate(0, translateY);

    RenderMesh2D(meshes["triangle_strip"], shaders["VertexColor"], modelMatrix);
}


void Lab3::FrameEnd()
{}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab3::OnInputUpdate(float deltaTime, int mods)
{}


void Lab3::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Lab3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{}


void Lab3::OnWindowResize(int width, int height)
{}