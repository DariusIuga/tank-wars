#pragma once

#include <iostream>

#include "components/simple_scene.h"

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1() override;

        void Init() override;

        class Projectile
        {
        public:
            Projectile(Tema1* lab3Instance, glm::vec2 coordinates, float angle,
                float magnitude,
                std::vector<float>& xValues, std::vector<float>& yValues, unsigned short tankOrigin)
                : coordinates(coordinates), angle(angle), magnitude(magnitude),
                xValues(xValues),
                yValues(yValues), tankOrigin(tankOrigin), lab3(lab3Instance)
            {
                direction = glm::vec2(cos(angle), sin(angle)) * magnitude;
            }

            void checkCollision()
            {
                // Detect collision with a tank
                if (tankOrigin != 1 && glm::distance(coordinates, glm::vec2(lab3->tank1.x,
                    lab3->tank1.y)) <
                    std::max(triggerRadius,
                        lab3->tank1.collisionCircleRadius))
                {
                    // Trigger explosion
                    explode();
                    lab3->tank1.takeDamage(10);
                }
                // Do the same for the second tank
                if (tankOrigin != 2 && glm::distance(coordinates, glm::vec2(lab3->tank2.x,
                    lab3->tank2.y)) <
                    std::max(triggerRadius,
                        lab3->tank2.collisionCircleRadius))
                {
                    explode();
                    lab3->tank2.takeDamage(10);
                }

                for (int i = 0; i < xValues.capacity(); ++i)
                {
                    // Detect collision with the ground
                    if (glm::distance(glm::vec2(xValues[i], yValues[i]),
                        coordinates) < triggerRadius)
                    {
                        // Trigger explosion
                        explode();
                    }
                }
            }

            void explode()
            {
                for (int i = 0; i < xValues.capacity(); ++i)
                {
                    if (glm::distance(glm::vec2(xValues[i], yValues[i]),
                        coordinates) <
                        explosionRadius)
                    {
                        // Use the circle equation to find the height of the lower half
                        // of the circle
                        float new_y = -sqrt(pow(explosionRadius, 2) -
                            pow(xValues[i] - coordinates.x, 2)) + coordinates.y;
                        yValues[i] = new_y;
                    }
                }
                lab3->createGroundMesh();
                isExploded = true;
            }

            bool isExploded = false;
            glm::vec2 coordinates;
            float angle;
            float magnitude;
            glm::vec2 direction{};
            std::vector<float>& xValues;
            std::vector<float>& yValues;

            constexpr static int triggerRadius = 5;
            constexpr static int explosionRadius = 60;
            unsigned short tankOrigin;

        private:
            Tema1* lab3; // Pointer to the Tema1 instance
        };

        class Tank
        {
        public:
            Tank(float x, float y, float angleTank, float angleBarrel,
                std::vector<float>& xValues, std::vector<float>& yValues)
                : angleBarrel(angleBarrel), x(x), y(y), angleTank(angleTank),
                projectileAngle(0.0f), xValues(xValues), yValues(yValues)
            {
                isDead = false;
            }

            Tank(std::vector<float>& xVals, std::vector<float>& yVals)
                : angleBarrel(0), x(0), y(0), angleTank(0), projectileAngle(0),
                xValues(xVals), yValues(yVals)
            {}

            void changeOrientation()
            {
                // Find the 2 points whose x values surround the tank
                int i = 0;
                while (xValues[i] < x)
                {
                    i++;
                }
                // Point A is at i-1, point B is at i
                // The tank is between A and B

                // Update the tank's y coordinate to remain on the ground
                y = std::min(yValues[i - 1], yValues[i]);

                // We calculate the angle of the tangent in the point i
                angleTank = atan2(yValues[i] - yValues[i - 1],
                    xValues[i] - xValues[i - 1]);
            }

            void takeDamage(unsigned short damage)
            {
                health -= damage;
                if(health <= 0)
                {
                    isDead = true;
                }
            }

            // Barrel rotation
            float angleBarrel{};

            // Coordinates of the tanks (from the middle of the bottom side)
            float x{}, y{};

            // Angles of the tanks
            float angleTank{};

            // Shooting angles
            float projectileAngle{};

            // Keep track of projectiles for the tanks
            std::vector<Projectile> projectiles;

            // References to the height map values
            std::vector<float>& xValues;
            std::vector<float>& yValues;

            // Method to calculate and create trajectory mesh
            void calculateTrajectory(unsigned short tankIndex) const;

            // Mesh to represent the trajectory
            Mesh* trajectoryMesh{};

            short health = maxHealth;
            // Framewire for the healthbar
            Mesh* frameHealthbar{};
            // Full rectangle that will be lowered as the health decreases
            Mesh* healthbar{};
            const int collisionCircleRadius = 50;

            bool isDead = false;
        };

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX,
            int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button,
            int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button,
            int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX,
            int offsetY) override;
        void OnWindowResize(int width, int height) override;


        void CreateHeightMap(int nrPoints)
        {
            xValues.reserve(nrPoints);
            yValues.reserve(nrPoints);

            const float step = static_cast<float>(resolution.x) /
                static_cast<
                float>(nrPoints - 1);

            for (int i = 0; i < nrPoints; ++i)
            {
                constexpr float periodLengthening = 300;
                const float x = static_cast<float>(i) * step;
                // Increase the period of the function
                const float t = x / periodLengthening;

                // Fourier series used to generate the height map
                float y = 4 * sin(t) + 2 * sin(
                    2 * t) + 1.5 * sin(
                        3 * t) + 1 * sin(5 * t) + 0.5 * sin(10 * t) + 0.2 * sin(
                            20 * t);
                // Make the change in height more drastic
                y *= 30;

                y += static_cast<float>(resolution.y) / 2;

                xValues[i] = x;
                yValues[i] = y;
            }

            // Now we calculate the coordinates of the tanks
            // The first one will be placed on the left, the second one on the right
            tank1.x = xValues[nrPoints / 10];
            tank1.y = yValues[nrPoints / 10];
            tank2.x = xValues[nrPoints - nrPoints / 10];
            tank2.y = yValues[nrPoints - nrPoints / 10];
        }

        void createGroundMesh()
        {
            // Create vertices for the triangle strip
            std::vector<VertexFormat> vertices;
            vertices.reserve(yValues.size() * 2);

            glm::vec3 groundColor = rgbToVec3(30, 117, 32);

            for (int i = 0; i < nrPoints; i++)
            {
                vertices.emplace_back(glm::vec3(xValues[i], yValues[i], 0),
                    groundColor);
                vertices.emplace_back(glm::vec3(xValues[i], 0, 0), groundColor);
            }

            // Define indices for the triangle strip
            std::vector<unsigned int> indices;
            indices.reserve(vertices.size());
            for (unsigned int i = 0; i < vertices.size(); ++i)
            {
                indices.push_back(i);
            }

            // Create and initialize the triangle strip mesh
            ground = new Mesh("triangle_strip");
            ground->SetDrawMode(GL_TRIANGLE_STRIP);
            ground->InitFromData(vertices, indices);
            AddMeshToList(ground);
        }

        // Function that takes 3 color arguments and returns a glm::vec3
        // glm::vec3 groundColor(1.0 / 255 * 120, 1.0 / 255 * 150, 1.0 / 255 * 100);
        static glm::vec3 rgbToVec3(const int r, const int g, const int b)
        {
            return { r / 256.0, g / 256.0, b / 256.0 };
        }

        void updateProjectiles(float deltaTimeSeconds)
        {
            // Update the position of all the projectiles and their direction vector
            for (auto& projectile : projectiles1)
            {
                if (projectile.isExploded)
                {
                    continue;
                }

                projectile.coordinates += projectile.direction *
                    deltaTimeSeconds;
                // Adjust direction vector
                projectile.direction -= gravity * deltaTimeSeconds;
                // Check for collision
                projectile.checkCollision();
            }

            for (auto& projectile : projectiles2)
            {
                if (projectile.isExploded)
                {
                    continue;
                }

                projectile.coordinates += projectile.direction *
                    deltaTimeSeconds;
                projectile.direction -= gravity * deltaTimeSeconds;
                projectile.checkCollision();
            }
        }

        void landslide()
        {
            for (int i = 0; i < nrPoints - 1; i++)
            {
                float difference = fabs(yValues[i + 1] - yValues[i]);
                if (difference > heightThreshold)
                {
                    if (yValues[i + 1] > yValues[i])
                    {
                        yValues[i] += epsilon;
                        yValues[i + 1] -= epsilon;
                    }
                    else
                    {
                        yValues[i] -= epsilon;
                        yValues[i + 1] += epsilon;
                    }
                }
            }

            // Redraw the ground
            createGroundMesh();
        }

    protected:
        glm::mat3 modelMatrix{};

        glm::ivec2 resolution{};

        // Mesh for the ground
        Mesh* ground{};
        // Value used for landslide calculations
        constexpr static unsigned int epsilon = 1;
        constexpr static float heightThreshold = 3;

        // Used for the height map
        std::vector<float> xValues;
        std::vector<float> yValues;
        constexpr static int nrPoints = 1000;

        constexpr static int nrTrianglesCircle = 100;
        // First trapezoid
        constexpr static int trackWidth = 30;
        constexpr static int trackHeight = 10;

        // Second trapezoid
        constexpr static int armorWidth = 50;
        constexpr static int armorHeight = 20;

        constexpr static int turretRadius = 20;
        constexpr static int barrelWidth = 3;
        constexpr static int barrelLength = 40;
        constexpr static int projectileRadius = 5;

        // The tanks
        Tank tank1;
        Tank tank2;

        constexpr static unsigned short maxHealth = 100;

        // Keep track of projectiles for the tanks
        std::vector<Projectile> projectiles1;
        std::vector<Projectile> projectiles2;
        const glm::vec2 gravity{ 0, 500 };

        constexpr static int sunRadius = 200;
        constexpr static int moonRadius = 100;
        bool isDay;
        constexpr static int starSideLength = 10;
        std::vector<glm::mat3> starModelMatrices;
    };
};