#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <cmath>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Tema1::Tema1() : tank1(xValues, yValues), tank2(xValues, yValues)
{
    isDay = true;

    // Reserve some memory for the projectiles
    projectiles1.reserve(100);
    projectiles2.reserve(100);
}

Tema1::~Tema1()
= default;

void Tema1::Init()
{
    resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, static_cast<float>(resolution.x), 0,
                            static_cast<float>(resolution.y),
                            0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    CreateHeightMap(nrPoints);

    tank1.xValues = xValues;
    tank1.yValues = yValues;
    tank2.xValues = xValues;
    tank2.yValues = yValues;

    createGroundMesh();

    vector<VertexFormat> tank1BaseVertices;
    vector<VertexFormat> tank2BaseVertices;
    glm::vec3 tank1TrackColor = rgbToVec3(3, 1, 77);
    glm::vec3 tank2TrackColor = rgbToVec3(77, 1, 3);

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

    glm::vec3 tank1ArmorColor = rgbToVec3(96, 93, 245);
    glm::vec3 tank2ArmorColor = rgbToVec3(245, 93, 96);

    // Add points for the upper trapezoid
    tank1BaseVertices.emplace_back(glm::vec3(-armorWidth, trackHeight, 0),
                                   tank1TrackColor);
    tank1BaseVertices.emplace_back(glm::vec3(armorWidth, trackHeight, 0),
                                   tank1TrackColor);
    tank1BaseVertices.emplace_back(
        glm::vec3(-armorWidth + 20, armorHeight + trackHeight, 0),
        tank1ArmorColor);
    tank1BaseVertices.emplace_back(
        glm::vec3(armorWidth - 20, armorHeight + trackHeight, 0),
        tank1ArmorColor);

    tank2BaseVertices.emplace_back(glm::vec3(-armorWidth, trackHeight, 0),
                                   tank2TrackColor);
    tank2BaseVertices.emplace_back(glm::vec3(armorWidth, trackHeight, 0),
                                   tank2TrackColor);
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

    Mesh* tank1_base = new Mesh("tank1_base");
    tank1_base->SetDrawMode(GL_TRIANGLES);
    tank1_base->InitFromData(tank1BaseVertices, tankBaseIndices);
    AddMeshToList(tank1_base);

    Mesh* tank2_base = new Mesh("tank2_base");
    tank2_base->SetDrawMode(GL_TRIANGLES);
    tank2_base->InitFromData(tank2BaseVertices, tankBaseIndices);
    AddMeshToList(tank2_base);

    // Create another mesh for the turret of the tank
    // It will be a semicircle on top of the tank base
    // Use the GL_TRIANGLE_FAN draw mode and the nrTrianglesCircle variable

    vector<VertexFormat> turret1Vertices;
    vector<VertexFormat> turret2Vertices;
    turret1Vertices.emplace_back(glm::vec3(0, trackHeight + armorHeight, 0),
                                 tank1ArmorColor);
    turret2Vertices.emplace_back(glm::vec3(0, trackHeight + armorHeight, 0),
                                 tank2ArmorColor);
    for (int i = 0; i <= nrTrianglesCircle; i++)
    {
        float angle = M_PI / nrTrianglesCircle * (i + 0.5);
        turret1Vertices.emplace_back(
            glm::vec3(turretRadius * cos(angle),
                      turretRadius * sin(angle) + trackHeight + armorHeight - 5,
                      0),
            tank1ArmorColor);
        turret2Vertices.emplace_back(
            glm::vec3(turretRadius * cos(angle),
                      turretRadius * sin(angle) + trackHeight + armorHeight - 5,
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

    // Mesh for health bar wireframe
    vector<VertexFormat> healthBarFrameVertices;
    glm::vec3 healthBarFrameColor = rgbToVec3(0, 0, 0);
    healthBarFrameVertices.emplace_back(
        glm::vec3(0, trackHeight + armorHeight + 10, 0),
        healthBarFrameColor);
    healthBarFrameVertices.emplace_back(
        glm::vec3(0, trackHeight + armorHeight, 0),
        healthBarFrameColor);
    healthBarFrameVertices.emplace_back(
        glm::vec3(maxHealth, trackHeight + armorHeight, 0),
        healthBarFrameColor);
    healthBarFrameVertices.emplace_back(
        glm::vec3(maxHealth, trackHeight + armorHeight + 10, 0),
        healthBarFrameColor);

    vector<unsigned int> healthBarFrameIndices = {
        0, 1, 2, 3
    };

    tank1.frameHealthbar = new Mesh("healthBarFrame1");
    tank1.frameHealthbar->SetDrawMode(GL_LINE_LOOP);
    tank1.frameHealthbar->InitFromData(healthBarFrameVertices,
                                       healthBarFrameIndices);
    AddMeshToList(tank1.frameHealthbar);

    tank2.frameHealthbar = new Mesh("healthBarFrame2");
    tank2.frameHealthbar->SetDrawMode(GL_LINE_LOOP);
    tank2.frameHealthbar->InitFromData(healthBarFrameVertices,
                                       healthBarFrameIndices);
    AddMeshToList(tank2.frameHealthbar);


    // Mesh for health bar fill
    vector<VertexFormat> healthBarFillVertices;
    glm::vec3 healthBarFillColor = rgbToVec3(0, 255, 0);
    healthBarFillVertices.emplace_back(
        glm::vec3(0, trackHeight + armorHeight + 10, 0),
        healthBarFillColor);
    healthBarFillVertices.emplace_back(
        glm::vec3(0, trackHeight + armorHeight, 0),
        healthBarFillColor);
    healthBarFillVertices.emplace_back(
        glm::vec3(maxHealth, trackHeight + armorHeight + 10, 0),
        healthBarFillColor);
    healthBarFillVertices.emplace_back(
        glm::vec3(maxHealth, trackHeight + armorHeight, 0),
        healthBarFillColor);

    vector<unsigned int> healthBarFillIndices = {
        0, 1, 2, 3
    };

    tank1.healthbar = new Mesh("healthBarFill1");
    tank1.healthbar->SetDrawMode(GL_TRIANGLE_STRIP);
    tank1.healthbar->InitFromData(healthBarFillVertices, healthBarFillIndices);
    AddMeshToList(tank1.healthbar);

    tank2.healthbar = new Mesh("healthBarFill2");
    tank2.healthbar->SetDrawMode(GL_TRIANGLE_STRIP);
    tank2.healthbar->InitFromData(healthBarFillVertices, healthBarFillIndices);
    AddMeshToList(tank2.healthbar);


    // Draw the sun
    glm::vec3 sunColor = rgbToVec3(255, 255, 0);
    vector<VertexFormat> sunVertices;
    sunVertices.emplace_back(glm::vec3(0, 0, 0), sunColor);
    for (int i = 0; i <= nrTrianglesCircle; i++)
    {
        float angle = 2 * M_PI / nrTrianglesCircle * i;
        sunVertices.emplace_back(
            glm::vec3(sunRadius * cos(angle), sunRadius * sin(angle), 0),
            sunColor);
    }

    vector<unsigned int> sunIndices;
    for (int i = 1; i <= nrTrianglesCircle; i++)
    {
        sunIndices.push_back(i);
    }

    Mesh* sun = new Mesh("sun");
    sun->SetDrawMode(GL_TRIANGLE_FAN);
    sun->InitFromData(sunVertices, sunIndices);
    AddMeshToList(sun);

    // Draw the moon
    glm::vec3 moonColor = rgbToVec3(220, 220, 220);
    vector<VertexFormat> moonVertices;
    moonVertices.emplace_back(glm::vec3(0, 0, 0), moonColor);
    for (int i = 0; i <= nrTrianglesCircle; i++)
    {
        float angle = 2 * M_PI / nrTrianglesCircle * i;
        moonVertices.emplace_back(
            glm::vec3(moonRadius * cos(angle), moonRadius * sin(angle), 0),
            moonColor);
    }

    vector<unsigned int> moonIndices;
    for (int i = 1; i <= nrTrianglesCircle; i++)
    {
        moonIndices.push_back(i);
    }

    Mesh* moon = new Mesh("moon");
    moon->SetDrawMode(GL_TRIANGLE_FAN);
    moon->InitFromData(moonVertices, moonIndices);
    AddMeshToList(moon);

    // Draw a star
    // It's a rhombus made of 2 triangles
    glm::vec3 starColor = rgbToVec3(255, 255, 255);
    vector<VertexFormat> starVertices;
    starVertices.emplace_back(glm::vec3(-starSideLength, 0, 0), starColor);
    starVertices.emplace_back(glm::vec3(starSideLength, 0, 0), starColor);
    starVertices.emplace_back(glm::vec3(0, starSideLength, 0), starColor);
    starVertices.emplace_back(glm::vec3(0, -starSideLength, 0), starColor);

    vector<unsigned int> starIndices = {
        2, 0, 1, 0, 3, 1
    };

    Mesh* star = new Mesh("star");
    star->SetDrawMode(GL_TRIANGLES);
    star->InitFromData(starVertices, starIndices);
    AddMeshToList(star);

    // Mesh for the tank's projectile
    glm::vec3 projectileColor = rgbToVec3(0, 0, 0);
    vector<VertexFormat> projectileVertices;
    projectileVertices.emplace_back(glm::vec3(0, 0, 0),
                                    projectileColor);
    for (int i = 0; i <= nrTrianglesCircle; i++)
    {
        float angle = 2 * M_PI / nrTrianglesCircle * i;
        projectileVertices.emplace_back(
            glm::vec3(projectileRadius * cos(angle),
                      projectileRadius * sin(angle), 0),
            projectileColor);
    }

    vector<unsigned int> projectileIndices;
    for (int i = 1; i <= nrTrianglesCircle; i++)
    {
        projectileIndices.push_back(i);
    }

    Mesh* projectile = new Mesh("projectile");
    projectile->SetDrawMode(GL_TRIANGLE_FAN);
    projectile->InitFromData(projectileVertices, projectileIndices);
    AddMeshToList(projectile);

    // Initialize trajectory meshes for both tanks
    tank1.trajectoryMesh = new Mesh("trajectory1");
    tank1.trajectoryMesh->SetDrawMode(GL_LINE_STRIP);
    AddMeshToList(tank1.trajectoryMesh);

    tank2.trajectoryMesh = new Mesh("trajectory2");
    tank2.trajectoryMesh->SetDrawMode(GL_LINE_STRIP);
    AddMeshToList(tank2.trajectoryMesh);
}

void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glm::vec3 skyDayColor = rgbToVec3(45, 175, 250);
    glm::vec3 skyNightColor = rgbToVec3(11, 33, 46);
    if (isDay)
    {
        glClearColor(skyDayColor.r, skyDayColor.g, skyDayColor.b, 1);
    }
    else
    {
        glClearColor(skyNightColor.r, skyNightColor.g, skyNightColor.b, 1);
    }
    // glm::vec3 clearColor = rgbToVec3(45, 175, 250);
    // glClearColor(clearColor.r, clearColor.g, clearColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    std::vector<unsigned int> healthBarFillIndices = {0, 1, 2, 3};
    // Set healthbar for Tank 1
    float healthX1 = tank1.health;
    if (tank1.health <= 0)
    {
        healthX1 = 0;
    }
    tank1.healthbar->vertices[2].position.x = healthX1;
    tank1.healthbar->vertices[3].position.x = healthX1;
    // Reinitialize the healthbar mesh to apply changes
    tank1.healthbar->InitFromData(tank1.healthbar->vertices,
                                  healthBarFillIndices);

    // Set healthbar for Tank 2
    float healthX2 = tank2.health;
    if (tank2.health <= 0)
    {
        healthX2 = 0;
    }
    tank2.healthbar->vertices[2].position.x = healthX2;
    tank2.healthbar->vertices[3].position.x = healthX2;
    // Reinitialize the healthbar mesh to apply changes
    tank2.healthbar->InitFromData(tank2.healthbar->vertices,
                                  healthBarFillIndices);


    // Calculate shooting angles
    tank1.projectileAngle = tank1.angleBarrel + tank1.angleTank + M_PI / 2;
    tank2.projectileAngle = tank2.angleBarrel + tank2.angleTank + M_PI / 2;

    // Update the terrain to simulate the landslide effect
    landslide();
}

void Tema1::Update(float deltaTimeSeconds)
{
    // Update translation
    modelMatrix = glm::mat3(1);

    // Render triangle strip
    RenderMesh2D(meshes["triangle_strip"], shaders["VertexColor"], modelMatrix);

    updateProjectiles(deltaTimeSeconds);

    if (!tank1.isDead)
    {
        // Model matrix for the first tank
        glm::mat3 tank1ModelMatrix = glm::mat3(1);
        tank1ModelMatrix *= transform2D::Translate(tank1.x, tank1.y);
        glm::mat3 healthBarModelMatrix1 = tank1ModelMatrix;

        // Rotate the tank around its center
        tank1ModelMatrix *= transform2D::Rotate(tank1.angleTank);

        // Render tank 1 base
        RenderMesh2D(meshes["tank1_base"], shaders["VertexColor"],
                     tank1ModelMatrix);
        // Render tank 1 turret
        RenderMesh2D(meshes["turret1"], shaders["VertexColor"],
                     tank1ModelMatrix);

        glm::mat3 cannon1ModelMatrix = tank1ModelMatrix;
        cannon1ModelMatrix *= transform2D::Translate(
            0, trackHeight + armorHeight);
        cannon1ModelMatrix *= transform2D::Rotate(tank1.angleBarrel);
        // Render tank 1 cannon
        RenderMesh2D(meshes["cannon"], shaders["VertexColor"],
                     cannon1ModelMatrix);

        // Render health bar for tank 1
        healthBarModelMatrix1 *= transform2D::Translate(-maxHealth / 2, 30);
        RenderMesh2D(meshes["healthBarFrame1"], shaders["VertexColor"],
                     healthBarModelMatrix1);
        RenderMesh2D(meshes["healthBarFill1"], shaders["VertexColor"],
                     healthBarModelMatrix1);

        // Render all projectiles of the first tank
        for (auto& projectile : projectiles1)
        {
            if (projectile.isExploded)
            {
                continue;
            }
            glm::mat3 projectileModelMatrix = glm::mat3(1);
            projectileModelMatrix *= transform2D::Translate(
                projectile.coordinates.x,
                projectile.coordinates.y);
            RenderMesh2D(meshes["projectile"], shaders["VertexColor"],
                         projectileModelMatrix);
        }

        // Calculate and render trajectory
        tank1.calculateTrajectory(1);
        RenderMesh2D(tank1.trajectoryMesh, shaders["VertexColor"], glm::mat3(1));
    }

    if (!tank2.isDead)
    {
        // Model matrix for the second tank
        glm::mat3 tank2ModelMatrix = glm::mat3(1);
        tank2ModelMatrix *= transform2D::Translate(tank2.x, tank2.y);
        glm::mat3 healthBarModelMatrix2 = tank2ModelMatrix;

        // Rotate the tank around its center
        tank2ModelMatrix *= transform2D::Rotate(tank2.angleTank);

        // Render tank 2 base
        RenderMesh2D(meshes["tank2_base"], shaders["VertexColor"],
                     tank2ModelMatrix);
        // Render tank 2 turret
        RenderMesh2D(meshes["turret2"], shaders["VertexColor"],
                     tank2ModelMatrix);

        glm::mat3 cannon2ModelMatrix = tank2ModelMatrix;
        cannon2ModelMatrix *= transform2D::Translate(
            0, trackHeight + armorHeight);
        cannon2ModelMatrix *= transform2D::Rotate(tank2.angleBarrel);
        // Render tank 2 cannon
        RenderMesh2D(meshes["cannon"], shaders["VertexColor"],
                     cannon2ModelMatrix);

        // Render health bar for tank 2
        healthBarModelMatrix2 *= transform2D::Translate(-maxHealth / 2, 30);
        RenderMesh2D(meshes["healthBarFrame2"], shaders["VertexColor"],
                     healthBarModelMatrix2);
        RenderMesh2D(meshes["healthBarFill2"], shaders["VertexColor"],
                     healthBarModelMatrix2);

        // Render all projectiles of the second tank
        for (auto& projectile : projectiles2)
        {
            if (projectile.isExploded)
            {
                continue;
            }
            glm::mat3 projectileModelMatrix = glm::mat3(1);
            projectileModelMatrix *= transform2D::Translate(
                projectile.coordinates.x,
                projectile.coordinates.y);
            RenderMesh2D(meshes["projectile"], shaders["VertexColor"],
                         projectileModelMatrix);
        }

        // Calculate and render trajectory
        tank2.calculateTrajectory(2);
        RenderMesh2D(tank2.trajectoryMesh, shaders["VertexColor"], glm::mat3(1));
    }

    if (isDay)
    {
        // Render the sun
        glm::mat3 sunModelMatrix = glm::mat3(1);
        sunModelMatrix *= transform2D::Translate(resolution.x, resolution.y);
        RenderMesh2D(meshes["sun"], shaders["VertexColor"], sunModelMatrix);
    }
    else
    {
        // Render the moon
        glm::mat3 moonModelMatrix = glm::mat3(1);
        moonModelMatrix *= transform2D::Translate(0, resolution.y);
        RenderMesh2D(meshes["moon"], shaders["VertexColor"], moonModelMatrix);

        for (auto star : starModelMatrices)
        {
            // Render each star
            RenderMesh2D(meshes["star"], shaders["VertexColor"], star);
        }
    }
}

void Tema1::FrameEnd()
{
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    // Barrel rotation
    // Rotate the cannon of the first tank counterclockwise (lift it)
    if (window->KeyHold(GLFW_KEY_W) && !tank1.isDead)
    {
        tank1.angleBarrel += deltaTime;
    }
    // Rotate the cannon of the first tank clockwise (lower it)
    if (window->KeyHold(GLFW_KEY_S) && !tank1.isDead)
    {
        tank1.angleBarrel -= deltaTime;
    }

    // Rotate the cannon of the second tank clockwise (lift it)
    if (window->KeyHold(GLFW_KEY_UP) && !tank2.isDead)
    {
        tank2.angleBarrel -= deltaTime;
    }
    // Rotate the cannon of the second tank counterclockwise (lower it)
    if (window->KeyHold(GLFW_KEY_DOWN) && !tank2.isDead)
    {
        tank2.angleBarrel += deltaTime;
    }

    // Tank movement
    // Move the first tank to the left
    if (window->KeyHold(GLFW_KEY_A) && !tank1.isDead)
    {
        tank1.x -= deltaTime * 100;
        tank1.changeOrientation();
    }
    // Move the first tank to the right
    if (window->KeyHold(GLFW_KEY_D) && !tank1.isDead)
    {
        tank1.x += deltaTime * 100;
        tank1.changeOrientation();
    }

    // Move the second tank to the left
    if (window->KeyHold(GLFW_KEY_LEFT) && !tank2.isDead)
    {
        tank2.x -= deltaTime * 100;
        tank2.changeOrientation();
    }
    // Move the second tank to the right
    if (window->KeyHold(GLFW_KEY_RIGHT))
    {
        tank2.x += deltaTime * 100;
        tank2.changeOrientation();
    }
}

void Tema1::OnKeyPress(int key, int mods)
{
    // Toggle day/night
    if (window->KeyHold(GLFW_KEY_LEFT_SHIFT))
    {
        if (isDay)
        {
            isDay = false;
            // Render a random number of stars (between 10 and 30)
            // Scale them by a random factor (between 0.5 and 1.5)
            // Draw them at random positions on the upper half of the screen
            int numStars = rand() % 21 + 10;
            for (int i = 0; i < numStars; ++i)
            {
                float scale = (rand() % 11 + 5) / 10.0f;
                glm::mat3 starModelMatrix = glm::mat3(1);
                starModelMatrix *= transform2D::Translate(
                    rand() % resolution.x,
                    rand() % (resolution.y / 2) + (resolution.y / 2));
                starModelMatrix *= transform2D::Scale(scale, scale);
                // Save the star mesh in a vector
                starModelMatrices.push_back(starModelMatrix);
                RenderMesh2D(meshes["star"], shaders["VertexColor"],
                             starModelMatrix);
            }
        }
        else
        {
            starModelMatrices.clear();
            isDay = true;
        }
    }

    // Shoot a projectile from the first tank
    if (key == GLFW_KEY_SPACE && !tank1.isDead)
    {
        // Add a new projectile to the list
        // float projectileAngle = angleBarrel1 + angleTank1 + M_PI / 2;
        // Calculate the tip of the barrel position
        float barrelTipX = tank1.x + barrelLength * cos(
            tank1.projectileAngle + tank1.angleTank);
        float barrelTipY = tank1.y + trackHeight + armorHeight + barrelLength *
            sin(tank1.projectileAngle);

        glm::vec2 projectileCoordinates = glm::vec2(barrelTipX, barrelTipY);
        float projectileMagnitude = 500;
        Projectile projectile(this, projectileCoordinates,
                              tank1.projectileAngle,
                              projectileMagnitude, tank1.xValues, tank1.yValues,
                              1);

        projectiles1.emplace_back(projectile);
    }

    // Shoot a projectile from the second tank
    if (key == GLFW_KEY_ENTER && !tank2.isDead)
    {
        // Add a new projectile to the list
        // float projectileAngle = angleBarrel1 + angleTank1 + M_PI / 2;
        // Calculate the tip of the barrel position
        float barrelTipX = tank2.x + barrelLength * cos(
            tank2.projectileAngle + tank2.angleTank);
        float barrelTipY = tank2.y + trackHeight + armorHeight + barrelLength *
            sin(tank2.projectileAngle);

        glm::vec2 projectileCoordinates = glm::vec2(barrelTipX, barrelTipY);
        float projectileMagnitude = 500;
        Projectile projectile(this, projectileCoordinates,
                              tank2.projectileAngle,
                              projectileMagnitude, tank1.xValues, tank1.yValues,
                              2);

        projectiles2.emplace_back(projectile);
    }
}

void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}

void m1::Tema1::Tank::calculateTrajectory(unsigned short type) const
{
    std::vector<VertexFormat> trajectoryVertices;
    glm::vec3 trajectoryColor; // Declare trajectoryColor without initializing

    if (type == 1)
    {
        trajectoryColor = rgbToVec3(0, 0, 255); // Blue color for tank 1
    }
    else if (type == 2)
    {
        trajectoryColor = rgbToVec3(255, 0, 0); // Red color for tank 2
    }

    // Initial position at the tip of the barrel
    glm::vec2 position = glm::vec2(
        x + barrelLength * cos(projectileAngle + angleTank),
        y + trackHeight + armorHeight + barrelLength * sin(projectileAngle));

    // Initial velocity
    float speed = 500.0f; // Same as projectile magnitude
    glm::vec2 direction = glm::vec2(cos(projectileAngle),
                                    sin(projectileAngle)) * speed;

    constexpr float maxTime = 10.0f;
    float time = 0.0f;

    // Gravity acceleration

    glm::vec2 acceleration(0, 530);

    // Add initial point
    trajectoryVertices.emplace_back(glm::vec3(position, 0), trajectoryColor);

    while (time < maxTime)
    {
        constexpr float timeStep = 0.1f;
        time += timeStep;
        // Update position
        position += direction * timeStep;
        // Update velocity
        direction -= acceleration * timeStep;

        // Check if projectile has hit the ground
        if (position.y <= 0)
            break;

        // Add new point
        trajectoryVertices.
            emplace_back(glm::vec3(position, 0), trajectoryColor);
    }

    // Indices
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < trajectoryVertices.size(); ++i)
    {
        indices.push_back(i);
    }

    // Initialize trajectory mesh
    trajectoryMesh->InitFromData(trajectoryVertices, indices);
}

// Definition of static constexpr members
constexpr int m1::Tema1::Projectile::triggerRadius;
constexpr int m1::Tema1::Projectile::explosionRadius;