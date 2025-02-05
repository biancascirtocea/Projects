#pragma once

#include "components/simple_scene.h"
#include "drone_camera.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct Package {
            glm::vec3 position;
            bool isCollected;
            float size;
        };

        struct DeliveryPoint {
            glm::vec3 position;
            float radius;
        };
        
        struct Cloud {
            glm::vec3 position;
            float speed;
        };
        

        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Render();
        bool CheckCollision(const glm::vec3& position, const glm::vec3& size, const glm::vec3& dronePosition);
        bool CheckCollisionSphere(const glm::vec3& objectPosition, const glm::vec3& objectSize, const glm::vec3& sphereCenter, float sphereRadius);
        bool IsColliding(const glm::vec3& newPosition);
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        Mesh* CreatePropeller(const std::string& name, glm::vec3 color);
        Mesh* CreateTree(const std::string& name);
        Mesh* CreateTerrain(const std::string& name, int gridSize);
        Mesh* CreateCube(const std::string& name, glm::vec3 color);
        Mesh* CreateBuilding(const std::string& name);
       Mesh* CreateCylinder(const std::string& name, glm::vec3 color);


        // Methods
        Mesh* CreatePackage(const std::string& name);
        Mesh* CreateDeliveryPoint(const std::string& name);
        Mesh* CreateDirectionArrow(const std::string& name);
        void SpawnNewPackage();
        void SpawnNewDeliveryPoint();
        void UpdateDirectionArrow();
        bool CheckPackageCollision();
        bool CheckDeliveryPointReached();
        Mesh* CreateCloud(const std::string& name);
        void RenderGameUI();

    protected:
        glm::mat4 modelMatrix;
        float translateX, translateY, translateZ;
        float scaleX, scaleY, scaleZ;
        float angularStepOX, angularStepOY, angularStepOZ;
        GLenum polygonMode;
        ViewportArea miniViewportArea;
        bool firstPersonView = true;
        float distanceToTarget = 2.0f;
        implemented::DroneCamera* camera;
        glm::mat4 projectionMatrix;
        float propellerRotation;  // pentru rotația elicelor
        std::vector<glm::vec3> treePositions;  // Pentru poziții copaci
        std::vector<glm::vec3> buildingPositions;  // Pentru poziții clădiri
        std::vector<glm::vec3> buildingSizes;      // Pentru dimensiuni diferite
        float mouseSensitivity;
        float droneScale = 0.5f;  // Factor de scalare global pentru dronă
		std::vector<float> treeSizes;  // Pentru dimensiuni diferite
        float totalTime = 0;  // pentru animația terenului
        float lateralDirection = 1.0f;  // Pentru A și D
        // Game state
        Package currentPackage;
        DeliveryPoint deliveryPoint;
        int score;
        bool packageAttached;
        float arrowRotation;
        std::vector<Cloud> clouds;

        float pitchAngle;  // unghi înclinare față-spate (Sus/Jos)
        float rollAngle;   // unghi înclinare stânga-dreapta (Left/Right)
        const float MAX_TILT = 15.0f; // înclinare maximă în grade
        const float TILT_SPEED = 45.0f; // viteza de înclinare în grade pe secundă

    };
}   // namespace m1
