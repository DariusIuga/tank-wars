#include "lab_m1/lab3/lab3.h"

#include <vector>
#include <cmath>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"
#include "lab_m1/lab4/transform3D.h"

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Lab3::Lab3()
{
    angleBarrel1 = 0;
    angleBarrel2 = 0;

    angleTank1 = 0;
    angleTank2 = 0;
}

Lab3::~Lab3()
= default;

void Lab3::Init()
{
    resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, static_cast<float>(resolution.x), 0, static_cast<float>(resolution.y),
                            0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    CreateHeightMap(nrPoints);

    // Create vertices for the triangle strip
    vector<VertexFormat> vertices;
    vertices.reserve(yValues.size() * 2);

    glm::vec3 groundColor = rgbToVec3(120, 150, 100);

    for (int i = 0; i < nrPoints; i++)
    {
        vertices.emplace_back(glm::vec3(xValues[i], yValues[i], 0),
                              groundColor);
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

    // Draw 2 trapezoids in opposite directions, one on top of the other
    // The trapezoids are drawn using 2 triangles each. They should look like
    // the base of a tank.

    vector<VertexFormat> tank1BaseVertices;
    vector<VertexFormat> tank2BaseVertices;
    glm::vec3 tank1TrackColor = rgbToVec3(3, 1, 77);
    glm::vec3 tank2TrackColor = rgbToVec3(77, 1, 1);

    // Add points for the lower trapezoid
    tank1BaseVertices.emplace_back(glm::vec3(-trackWidth, 0, 0),
                                   tank1TrackColor);
    tank1BaseVertices.
        emplace_back(glm::vec3(trackWidth, 0, 0), tank1TrackColor);
    tank1BaseVertices.emplace_back(glm::vec3(-trackWidth - 10, trackHeight, 0),
                                   tank1TrackColor);
    tank1BaseVertices.emplace_back(glm::vec3(trackWidth + 10, trackHeight, 0),
                                   tank1TrackColor);

    tank2BaseVertices.emplace_back(glm::vec3(-trackWidth, 0, 0),
                                   tank2TrackColor);
    tank2BaseVertices.
        emplace_back(glm::vec3(trackWidth, 0, 0), tank2TrackColor);
    tank2BaseVertices.emplace_back(glm::vec3(-trackWidth - 10, trackHeight, 0),
                                   tank2TrackColor);
    tank2BaseVertices.emplace_back(glm::vec3(trackWidth + 10, trackHeight, 0),
                                   tank2TrackColor);

    glm::vec3 tank1ArmorColor = rgbToVec3(46, 41, 196);
    glm::vec3 tank2ArmorColor = rgbToVec3(196, 41, 62);

    // Add points for the upper trapezoid
    tank1BaseVertices.emplace_back(glm::vec3(-armorWidth, trackHeight, 0),
                                   tank1ArmorColor);
    tank1BaseVertices.emplace_back(glm::vec3(armorWidth, trackHeight, 0),
                                   tank1ArmorColor);
    tank1BaseVertices.emplace_back(
        glm::vec3(-armorWidth + 20, armorHeight + trackHeight, 0),
        tank1ArmorColor);
    tank1BaseVertices.emplace_back(
        glm::vec3(armorWidth - 20, armorHeight + trackHeight, 0),
        tank1ArmorColor);

    tank2BaseVertices.emplace_back(glm::vec3(-armorWidth, trackHeight, 0),
                                   tank2ArmorColor);
    tank2BaseVertices.emplace_back(glm::vec3(armorWidth, trackHeight, 0),
                                   tank2ArmorColor);
    tank2BaseVertices.emplace_back(
    glm::vec3(-armorWidth + 20, armorHeight + trackHeight, 0),
    tank2ArmorColor);
    tank2BaseVertices.emplace_back(
        glm::vec3(armorWidth - 20, armorHeight + trackHeight, 0),
        tank2ArmorColor);

    // Indices for both bases
    vector<unsigned int> tankBaseIndices = {
        2, 1, 0, 2, 3, 1,
        6, 5, 4, 6, 7, 5
    };

    Mesh* tank1 = new Mesh("tank1_base");
    tank1->SetDrawMode(GL_TRIANGLES);
    tank1->InitFromData(tank1BaseVertices, tankBaseIndices);
    AddMeshToList(tank1);

    Mesh* tank2 = new Mesh("tank2_base");
    tank2->SetDrawMode(GL_TRIANGLES);
    tank2->InitFromData(tank2BaseVertices, tankBaseIndices);
    AddMeshToList(tank2);

    // Create another mesh for the turret of the tank
    // It will be a semicircle on top of the tank base
    // Use the GL_TRIANGLE_FAN draw mode and the nrTrianglesCircle variable

    vector<VertexFormat> turret1Vertices;
    vector<VertexFormat> turret2Vertices;
    turret1Vertices.emplace_back(glm::vec3(0, trackHeight + armorHeight, 0), tank1ArmorColor);
    turret2Vertices.emplace_back(glm::vec3(0, trackHeight + armorHeight, 0), tank2ArmorColor);
    for (int i = 0; i <= nrTrianglesCircle; i++)
    {
        float angle = M_PI / nrTrianglesCircle * (i + 0.5);
        turret1Vertices.emplace_back(
            glm::vec3(turretRadius * cos(angle), turretRadius * sin(angle) + trackHeight + armorHeight - 5,
                      0),
            tank1ArmorColor);
        turret2Vertices.emplace_back(
            glm::vec3(turretRadius * cos(angle), turretRadius * sin(angle) + trackHeight + armorHeight - 5,
                      0),
            tank2ArmorColor);
    }

    // Indices for both turrets
    vector<unsigned int> turretIndices;
    for (int i = 1; i <= nrTrianglesCircle; i++)
    {
        turretIndices.push_back(i);
    }
    turretIndices.push_back(1);

    Mesh* turret1 = new Mesh("turret1");
    turret1->SetDrawMode(GL_TRIANGLE_FAN);
    turret1->InitFromData(turret1Vertices, turretIndices);
    AddMeshToList(turret1);

    Mesh* turret2 = new Mesh("turret2");
    turret2->SetDrawMode(GL_TRIANGLE_FAN);
    turret2->InitFromData(turret2Vertices, turretIndices);
    AddMeshToList(turret2);

    // Create a mesh for the cannon of the tank
    // It will be a slim rectangle that starts from the turret center, made of
    // 2 triangles
    vector<VertexFormat> cannonVertices;
    glm::vec3 cannonColor = rgbToVec3(50, 50, 50);

    cannonVertices.emplace_back(glm::vec3(-barrelWidth, 0, 0), cannonColor);
    cannonVertices.emplace_back(glm::vec3(barrelWidth, 0, 0), cannonColor);
    cannonVertices.emplace_back(glm::vec3(-barrelWidth, barrelLength, 0),
                                cannonColor);
    cannonVertices.emplace_back(glm::vec3(barrelWidth, barrelLength, 0),
                                cannonColor);

    vector<unsigned int> cannonIndices = {
        2, 1, 0, 2, 3, 1
    };

    Mesh* cannon = new Mesh("cannon");
    cannon->SetDrawMode(GL_TRIANGLES);
    cannon->InitFromData(cannonVertices, cannonIndices);
    AddMeshToList(cannon);
}

void Lab3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    // Sky color
    const glm::vec3 clearColor = rgbToVec3(100, 200, 256);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Lab3::Update(float deltaTimeSeconds)
{
    // Update translation
    modelMatrix = glm::mat3(1);

    // Render triangle strip
    RenderMesh2D(meshes["triangle_strip"], shaders["VertexColor"], modelMatrix);

    // Model matrix for the first tank
    glm::mat3 tank1ModelMatrix = glm::mat3(1);
    tank1ModelMatrix *= transform2D::Translate(tank1X, tank1Y);
    // Rotate the tank around its center
    tank1ModelMatrix *= transform2D::Rotate(angleTank1);

    // Render tank 1 base
    RenderMesh2D(meshes["tank1_base"], shaders["VertexColor"],
                 tank1ModelMatrix);
    // Render tank 1 turret
    RenderMesh2D(meshes["turret1"], shaders["VertexColor"], tank1ModelMatrix);

    glm::mat3 cannon1ModelMatrix = tank1ModelMatrix;
    cannon1ModelMatrix *= transform2D::Translate(0, trackHeight + armorHeight);
    cannon1ModelMatrix *= transform2D::Rotate(angleBarrel1);
    // Render tank 1 cannon
    RenderMesh2D(meshes["cannon"], shaders["VertexColor"], cannon1ModelMatrix);

    // Model matrix for the second tank
    glm::mat3 tank2ModelMatrix = glm::mat3(1);
    tank2ModelMatrix *= transform2D::Translate(tank2X, tank2Y);
    // Rotate the tank around its center
    tank2ModelMatrix *= transform2D::Rotate(angleTank2);

    // Render tank 2 base
    RenderMesh2D(meshes["tank2_base"], shaders["VertexColor"],
                 tank2ModelMatrix);
    // Render tank 2 turret
    RenderMesh2D(meshes["turret2"], shaders["VertexColor"], tank2ModelMatrix);

    glm::mat3 cannon2ModelMatrix = tank2ModelMatrix;
    cannon2ModelMatrix *= transform2D::Translate(0, trackHeight + armorHeight);
    cannon2ModelMatrix *= transform2D::Rotate(angleBarrel2);
    // Render tank 2 cannon
    RenderMesh2D(meshes["cannon"], shaders["VertexColor"], cannon2ModelMatrix);
}

void Lab3::FrameEnd()
{
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Lab3::OnInputUpdate(float deltaTime, int mods)
{
    // Barrel rotation
    // Rotate the cannon of the first tank counterclockwise (lift it)
    if(window->KeyHold(GLFW_KEY_W))
    {
        angleBarrel1 += deltaTime;
    }
    // Rotate the cannon of the first tank clockwise (lower it)
    if(window->KeyHold(GLFW_KEY_S))
    {
        angleBarrel1 -= deltaTime;
    }

    // Rotate the cannon of the second tank clockwise (lift it)
    if(window->KeyHold(GLFW_KEY_UP))
    {
        angleBarrel2 -= deltaTime;
    }
    // Rotate the cannon of the second tank counterclockwise (lower it)
    if(window->KeyHold(GLFW_KEY_DOWN))
    {
        angleBarrel2 += deltaTime;
    }

    // Tank movement
    // Move the first tank to the left
    if(window->KeyHold(GLFW_KEY_A))
    {
        tank1X -= deltaTime * 100;
        changeTank1Orientation();
    }
    // Move the first tank to the right
    if(window->KeyHold(GLFW_KEY_D))
    {
        tank1X += deltaTime * 100;
        changeTank1Orientation();
    }

    // Move the second tank to the left
    if(window->KeyHold(GLFW_KEY_LEFT))
    {
        tank2X -= deltaTime * 100;
        changeTank2Orientation();
    }
    // Move the second tank to the right
    if(window->KeyHold(GLFW_KEY_RIGHT))
    {
        tank2X += deltaTime * 100;
        changeTank2Orientation();
    }
}

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
{
}

void Lab3::OnWindowResize(int width, int height)
{
}
