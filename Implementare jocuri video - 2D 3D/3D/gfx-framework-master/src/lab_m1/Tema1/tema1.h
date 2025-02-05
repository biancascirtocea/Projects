#pragma once

#include "components/simple_scene.h"
#include <string>
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

namespace m1
{
    struct Projectile {
        glm::vec2 position;
        glm::vec2 velocity;
        bool active;
    };

    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;

        void RenderTank(float x, float y, float angle, std::string name, float turretAngle);
        void CreateCloud(float centerX, float centerY, glm::vec3 color, std::string cloudName);
        void UpdateCloudPosition(float& cloudX, bool& movingRight, float speed, float deltaTime, float leftBound, float rightBound);
        void UpdateCloudProjectiles(std::vector<Projectile>& projectiles, float deltaTime);
        void LaunchCloudProjectile(float cloudX, float cloudY, std::vector<Projectile>& projectiles);
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void CreateTerrainSegment(int index);
        void LaunchProjectile(float tankX, float tankY,float tankAngle, float turretAngle, std::vector<Projectile>& projectiles, float initialSpeed);
        void CreateTank(float l, glm::vec3 color1, glm::vec3 color2, std::string tankName);
        void CreateCircle(float radius, int numSegments, glm::vec3 color, std::string tankName);
        void UpdateProjectiles(std::vector<Projectile>& projectiles, float deltaTime);
        void DrawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec3& color);
        void DrawTrajectory(float startX, float startY, float angle, float initialSpeed, float tankAngle);
        void DrawHealthBar(float x, float y, float health, float maxHealth, float healthBarWidth, float healthBarHeight);


        // Funcții pentru coliziune și deformare teren
        bool CheckCollision(const glm::vec2& projectilePos, float tankX, float tankY);
        bool CheckTerrainCollision(const glm::vec2& currentPos, const glm::vec2& nextPos);
        void DeformTerrain(const glm::vec2& collisionPoint);
      
    protected:
        // Variabile pentru transformări
        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep, rotation;

        // Harta înălțimilor pentru teren
        std::vector<glm::vec2> heightMap;

        // Pozițiile și dimensiunile tancurilor
        float tank1X = 0.0f;
        float tank2X = 0.0f;
        float tank1Y = 0.0f;
        float tank2Y = 0.0f;
        float tankLength = 25.0f;
        float turretAngle1 = glm::radians(0.0f);
        float turretAngle2 = glm::radians(0.0f);
		float tank1Angle = 0.0f;
		float tank2Angle = 0.0f;

        // Proiectile
        std::vector<Projectile> projectilesTank1;
        std::vector<Projectile> projectilesTank2;
        float projectileSpeed = 200.0f;
        float gravity = 98.0f;
        float projectileRadius = 5.0f;
        float initialSpeed1 = 200.0f;  // Viteza inițială pentru tancul 1
        float initialSpeed2 = 200.0f;  // Viteza inițială pentru tancul 2
        float tankHeight = tankLength * 0.775f;
		float turretLength = 25.0f;

        // Sănătatea tancurilor
        bool tank1Active = true;
        bool tank2Active = true;
        float maxHealth = 30.0f;
        float tank1Health = maxHealth;
        float tank2Health = maxHealth;
   
        // Setările pentru bara de sănătate
        float healthBarWidth = 50.0f;   // Lățimea totală a barei de sănătate când este plină
        float healthBarHeight = 10.0f;  // Înălțimea barei de sănătate

        // Parametri pentru coliziune și deformare
        float collisionThreshold = 50.0f; // Pragul de coliziune cu terenul
        float explosionRadius = 100.0f;   // Raza exploziei
        float heightDifferenceThreshold = 5.0f; // Pragul de diferență de înălțime pentru alunecarea de teren
        float heightTransferRate = 5.0f; // Rata de transfer a înălțimii între puncte
        float timeScale = 2.0f; // Viteza proiectil

        bool isKey1Pressed = false;  // Starea pentru tasta 1
        bool isKey2Pressed = false;  // Starea pentru tasta 2

        // Viteza de mișcare pentru nori
        float cloudSpeed = 50.0f;

        // Direcția de mișcare pentru fiecare nor
        bool cloud1MovingRight = true;
        bool cloud2MovingRight = false;
        bool cloud3MovingRight = true;

        // Pozițiile de pornire pentru nori
        float cloud1X = 200;
        float cloud2X = 600;
        float cloud3X = 1000;
        std::vector<Projectile> projectilesClouds;
    };
}   // namespace m1


namespace transform2D
{
    // Translate matrix
    inline glm::mat3 Translate(float translateX, float translateY)
    {
        return glm::mat3(
            1, 0, 0,
            0, 1, 0,
            translateX, translateY, 1
        );
    }

    // Scale matrix
    inline glm::mat3 Scale(float scaleX, float scaleY)
    {
        return glm::mat3(
            scaleX, 0, 0,
            0, scaleY, 0,
            0, 0, 1
        );
    }

    // Rotate matrix
    inline glm::mat3 Rotate(float radians)
    {
        return glm::mat3(
            cos(radians), sin(radians), 0,
            -sin(radians), cos(radians), 0,
            0, 0, 1
        );
    }
}   // namespace transform2D

