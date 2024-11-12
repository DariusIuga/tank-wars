#pragma once

#include <iostream>

#include "components/simple_scene.h"


namespace m1
{
    class Lab3 : public gfxc::SimpleScene
    {
    public:
        Lab3();
        ~Lab3();

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
            glm::ivec2 resolution = window->GetResolution();

            double step = (double)resolution.x / (double)(nrPoints - 1);
            double periodLenghtening = 300;

            for (int i = 0; i < nrPoints; ++i)
            {
                double x = i * step;
                std::cout << x << " " << std::endl;
                // Increase the period of the function
                double t = x / periodLenghtening;

                // Fourier series used to generate the height map
                double y = 4 * sin(t) + 2 * sin(
                    2 * t) + 1.5 * sin(
                    3 * t) + 1 * sin(5 * t) + 0.5 * sin(10 * t) + 0.2 * sin(
                    20 * t);
                // Make the change in height more drastic
                y *= 30;

                y += (double)resolution.y / 2;

                xValues[i] = x;
                yValues[i] = y;
            }

            // Print the height map
            for (auto point : yValues)
            {
                std::cout << point << " " << std::endl;
            }

            // Print width and height of the window
            std::cout << resolution.x << " " << resolution.y << std::endl;
        }

    protected:
        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;

        // TODO(student): If you need any other class variables, define them here.
        enum Direction
        {
            UP,
            DOWN
        };

        Direction dirY, size;

        std::vector<double> xValues;
        std::vector<double> yValues;
        constexpr static int nrPoints = 300;
    };
} // namespace m1
