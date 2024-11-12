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
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;


        void CreateHeightMap(int nrPoints)
        {
            glm::ivec2 resolution = window->GetResolution();
            float step = resolution.x / (nrPoints - 1);

            for (int i = 0; i < nrPoints; ++i) {
                float x = i * step;
                // Fourier series used to generate the height map
                float y = sin(x) + 2 * sin(0.5 * x) + 0.5 * sin(3 * x);
                heightMap[x] = y;
            }

            // Print the height map
            for (auto it = heightMap.begin(); it != heightMap.end(); ++it) {
                std::cout << it->first << " " << it->second << std::endl;
            }
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

        std::unordered_map<float, float> heightMap;
        constexpr static int nrPoints = 100;
    };
}   // namespace m1
