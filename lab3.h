#pragma once

#include <iostream>

#include "components/simple_scene.h"


namespace m1
{
    class Lab3 : public gfxc::SimpleScene
    {
    public:
        Lab3();
        ~Lab3() override;

        void Init() override;

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

            const float step = static_cast<float>(resolution.x) / static_cast<
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
            tank1X = xValues[nrPoints / 10];
            tank1Y = yValues[nrPoints / 10];
            tank2X = xValues[nrPoints - nrPoints / 10];
            tank2Y = yValues[nrPoints - nrPoints / 10];
        }

        // Function that takes 3 color arguments and returns a glm::vec3
        // glm::vec3 groundColor(1.0 / 255 * 120, 1.0 / 255 * 150, 1.0 / 255 * 100);
        static glm::vec3 rgbToVec3(const int r, const int g, const int b)
        {
            return {r / 256.0, g / 256.0, b / 256.0};
        }

        void changeTank1Orientation()
        {
            // Find the 2 points whose x values surround the tank
            int i = 0;
            while (xValues[i] < tank1X)
            {
                i++;
            }
            // Point A is at i-1, point B is at i
            // The tank is between A and B

            // Update the tank's y coordinate to remain on the ground
            // float t = (tank1Y - yValues[i - 1]) / (yValues[i] - yValues[i - 1]);
            // tank1Y = yValues[i - 1] + t * (yValues[i] - yValues[i - 1]);
            tank1Y = std::min(yValues[i - 1], yValues[i]);

            // We calculate the angle of the tangent in the point i
            angleTank1 = atan2(yValues[i] - yValues[i - 1],
                               xValues[i] - xValues[i - 1]);
        }

        // Do the same for the second tank
        void changeTank2Orientation()
        {
            int i = 0;
            while (xValues[i] < tank2X)
            {
                i++;
            }
            // Point A is at i-1, point B is at i
            // The tank is between A and B

            tank2Y = std::min(yValues[i - 1], yValues[i]);

            angleTank2 = atan2(yValues[i] - yValues[i - 1],
                               xValues[i] - xValues[i - 1]);
        }

    protected:
        float cx{}, cy{};
        glm::mat3 modelMatrix{};
        float translateX{}, translateY{};
        float scaleX{}, scaleY{};
        float angularStep{};

        // TODO(student): If you need any other class variables, define them here.
        glm::ivec2 resolution{};

        // Used for the height map
        std::vector<float> xValues;
        std::vector<float> yValues;
        constexpr static int nrPoints = 300;

        constexpr static int nrTrianglesCircle = 100;
        // First trapezoid
        constexpr static int trackWidth = 30;
        constexpr static int trackHeight = 10;

        // Second trapezoid
        constexpr static int armorWidth = 45;
        constexpr static int armorHeight = 15;

        constexpr static int turretRadius = 20;
        constexpr static int barrelWidth = 3;
        constexpr static int barrelLength = 40;
        constexpr static int projectileRadius = 5;

        // Barrel rotation
        float angleBarrel1;
        float angleBarrel2;

        // Coordinates of the tanks (from the middle of the bottom side)
        float tank1X;
        float tank1Y;
        float tank2X;
        float tank2Y;

        // Angles of the tanks
        float angleTank1;
        float angleTank2;
    };
} // namespace m1
