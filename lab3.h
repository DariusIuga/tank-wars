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

            const float step = static_cast<float>(resolution.x) / static_cast<float>(nrPoints - 1);

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
        }

        // Function that takes 3 color arguments and returns a glm::vec3
        // glm::vec3 groundColor(1.0 / 255 * 120, 1.0 / 255 * 150, 1.0 / 255 * 100);
        static glm::vec3 rgbToVec3(const int r, const int g, const int b)
        {
            return {r / 256.0, g / 256.0, b / 256.0};
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
        constexpr static int trackWidth = 60;
        constexpr static int trackHeight = 20;

        // Second trapezoid
        constexpr static int armorWidth = 90;
        constexpr static int armorHeight = 30;

        constexpr static int turretRadius = 30;
        constexpr static int barrelWidth = 3;
        constexpr static int barrelLength = 100;
        constexpr static int projectileRadius = 5;

        // Barrel rotation
        float radiansBarrel1;
        float radiansBarrel2;
    };
} // namespace m1
