#include "lab_m1/tema1/tema1.h"
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;
using namespace m1;


Tema1::Tema1() {
}

Tema1::~Tema1() {
}
void Tema1::Init() {
    // Definim culorile pentru fiecare tanc
    glm::vec3 color1_tank1 = glm::vec3(193.0f / 255.0f, 162.0f / 255.0f, 118.0f / 255.0f); // Culoarea partii superioare a tancului 1
    glm::vec3 color2_tank1 = glm::vec3(0.376f, 0.302f, 0.169f); // Culoarea partii inferioare a tancului 1
    glm::vec3 color1_tank2 = glm::vec3(158.0f / 255.0f, 192.0f / 255.0f, 117.0f / 255.0f); // Culoarea partii superioare a tancului 2
    glm::vec3 color2_tank2 = glm::vec3(0.3569f, 0.4510f, 0.2078f); // Culoarea partii inferioare a tancului 2

    // Cream fiecare tanc folosind culorile definite
    CreateTank(tankLength, color1_tank1, color2_tank1, "Tank1"); // Tancul 1
    CreateTank(tankLength, color1_tank2, color2_tank2, "Tank2"); // Tancul 2

    // Setam pozitia initiala a tancului 2 pe partea dreapta a ecranului
    glm::ivec2 resolution = window->GetResolution();
    tank2X = resolution.x - 100.0f; // Offset de 100 unitati de la marginea dreapta

    // Configuram camera
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();

    // Parametrii pentru generarea terenului folosind functii sinusoidale
    float A1 = 50, A2 = 30, A3 = 20; // Amplitudinile
    float W1 = 0.01f, W2 = 0.02f, W3 = 0.005f; // Frecventele
    int num_points = resolution.x / 9; // Numarul de puncte pentru harta de inaltime

    // Generam harta de inaltime a terenului
    for (int i = 0; i < num_points; i++) {
        float x = i * 9.5f;
        float y = A1 * sin(W1 * x) + A2 * sin(W2 * x) + A3 * sin(W3 * x);
        heightMap.push_back(glm::vec2(x, y + resolution.y / 3)); // Centram terenul vertical
    }

    // Cream mesh-ul pentru proiectile mici si negre
    CreateCircle(5.0f, 12, glm::vec3(0.0f, 0.0f, 0.0f), "projectile");

    // Definim o culoare albastru deschis pentru nori
    glm::vec3 cloudColor = glm::vec3(0.7f, 0.85f, 1.0f);

    // Cream cercurile pentru norii care vor fi folositi in joc
    CreateCircle(30.0f, 15, cloudColor, "cloudLeft");
    CreateCircle(30.0f, 15, cloudColor, "cloudMiddle");
    CreateCircle(30.0f, 15, cloudColor, "cloudRight");
    CreateCircle(30.0f, 15, cloudColor, "cloudTop");

    // Creează mesh-ul pentru proiectilele norilor, mai mari decât celelalte proiectile
    CreateCircle(10.0f, 12, glm::vec3(0.5f, 0.5f, 0.5f), "cloudProjectile");

}

void Tema1::CreateCircle(float radius, int numSegments, glm::vec3 color, std::string circleName) {
    // Vector care va contine vertecsii (punctele) cercului
    std::vector<VertexFormat> circleVertices;

    // Vector pentru indicii care formeaza triunghiurile necesare pentru desenarea cercului
    std::vector<unsigned int> circleIndices;

    // Adaugam centrul cercului ca primul vertex (punct), care va fi folosit pentru toate triunghiurile
    circleVertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));

    // Generam vertecsii pentru circumferinta cercului
    for (int i = 0; i <= numSegments; ++i) { // Parcurgem fiecare segment al cercului
        // Calculam unghiul curent pe circumferinta
        float angle = 2.0f * M_PI * i / numSegments;

        // Calculam coordonatele punctului curent folosind functiile trigonometrice cos si sin
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        // Adaugam vertexul curent in lista de vertecsi
        circleVertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));

        // Daca nu suntem la primul punct, generam indicii pentru triunghiul format
        if (i > 0) {
            circleIndices.push_back(0);      // Centrul cercului
            circleIndices.push_back(i);      // Punctul anterior de pe circumferinta
            circleIndices.push_back(i + 1);  // Punctul curent de pe circumferinta
        }
    }

    Mesh* cerc = new Mesh("cerc" + circleName); // Numele cercului este unic, bazat pe parametru
    cerc->InitFromData(circleVertices, circleIndices);
    AddMeshToList(cerc);
}

void Tema1::CreateTank(float l, glm::vec3 color1, glm::vec3 color2, std::string tankName) {
    // Culoare pentru turela tancului
    glm::vec3 turretColor = glm::vec3(0.0f, 0.0f, 0.0f);

    // Cream trapezul inferior al tancului
    std::vector<VertexFormat> bottomTrapezVertices = {
        VertexFormat(glm::vec3(-l, 0, 0), color2),
        VertexFormat(glm::vec3(l, 0, 0), color2),
        VertexFormat(glm::vec3(1.25 * l, 0.275 * l, 0), color2),
        VertexFormat(glm::vec3(-1.25 * l, 0.275 * l, 0), color2),
    };

    std::vector<unsigned int> bottomTrapezIndices = { 0, 1, 2, 0, 3, 2 };
    Mesh* bottomTrapez = new Mesh("bottomTrapez" + tankName);
    bottomTrapez->InitFromData(bottomTrapezVertices, bottomTrapezIndices);
    AddMeshToList(bottomTrapez);

    // Cream trapezul superior al tancului
    std::vector<VertexFormat> topTrapezVertices = {
        VertexFormat(glm::vec3(-1.75 * l, 0.775 * l, 0), color1),
        VertexFormat(glm::vec3(1.75 * l, 0.775 * l, 0), color1),
        VertexFormat(glm::vec3(-1.5 * l, 0.275 * l, 0), color1),
        VertexFormat(glm::vec3(1.5 * l, 0.275 * l, 0), color1),
    };
    std::vector<unsigned int> topTrapezIndices = { 0, 1, 2, 1, 2, 3 };
    Mesh* topTrapez = new Mesh("topTrapez" + tankName);
    topTrapez->InitFromData(topTrapezVertices, topTrapezIndices);
    AddMeshToList(topTrapez);

    // Cream cercul de deasupra tancului
    CreateCircle(17, 36, color1, tankName);

    // Cream turela tancului ca un dreptunghi
    std::vector<VertexFormat> turretVertices = {
        VertexFormat(glm::vec3(-2.0f, 0, 0), turretColor),
        VertexFormat(glm::vec3(2.0f, 0, 0), turretColor),
        VertexFormat(glm::vec3(2.0f, 25.0f, 0), turretColor),
        VertexFormat(glm::vec3(-2.0f, 25.0f, 0), turretColor)
    };
    std::vector<unsigned int> turretIndices = { 0, 1, 2, 0, 2, 3 };
    Mesh* turret = new Mesh("turret" + tankName);
    turret->InitFromData(turretVertices, turretIndices);
    AddMeshToList(turret);
}

// Functie care este apelata la inceputul fiecarui cadru pentru a reseta buffer-ele si a seta culoarea cerului
void Tema1::FrameStart() {
    // Setam culoarea cerului (valori RGBA)
    glClearColor(0.588f, 0.788f, 0.902f, 1.0f); // Culoare albastru deschis
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Golim buffer-ele de culoare si adancime

    // Obtinem rezolutia ferestrei
    glm::ivec2 resolution = window->GetResolution();

    // Setam viewport-ul (fereastra de vizualizare) pentru a ocupa intreaga suprafata a ecranului
    glViewport(0, 0, resolution.x, resolution.y);
}

// Functie pentru a crea un segment de teren folosind doua triunghiuri
void Tema1::CreateTerrainSegment(int index) {
    // Definim culoarea segmentului de teren
    glm::vec3 color = glm::vec3(0.8f, 0.784f, 0.192f);

    glm::mat3 modelMatrix = glm::mat3(1);

    std::string meshNameLeft = "triangleLeft" + std::to_string(index);
    std::string meshNameRight = "triangleRight" + std::to_string(index);

    // Triunghiul stang al segmentului
    std::vector<VertexFormat> vertices1 = {
        VertexFormat(glm::vec3(heightMap[index].x, heightMap[index].y, 0.0f), color), //  stanga-sus
        VertexFormat(glm::vec3(heightMap[index + 1].x, heightMap[index + 1].y, 0.0f), color), //  dreapta-sus
        VertexFormat(glm::vec3(heightMap[index].x, 0.0f, 0.0f), color) // stanga-jos
    };
    std::vector<unsigned int> indices1 = { 0, 1, 2 };

    // Verificam daca mesh-ul nu a fost deja creat
    if (meshes.find(meshNameLeft) == meshes.end()) {
        Mesh* triangleLeft = new Mesh(meshNameLeft);
        triangleLeft->InitFromData(vertices1, indices1);
        AddMeshToList(triangleLeft);
    }

    // Triunghiul drept al segmentului
    std::vector<VertexFormat> vertices2 = {
        VertexFormat(glm::vec3(heightMap[index + 1].x, heightMap[index + 1].y, 0.0f), color), // Dreapta-sus
        VertexFormat(glm::vec3(heightMap[index + 1].x, 0.0f, 0.0f), color), // Dreapta-jos
        VertexFormat(glm::vec3(heightMap[index].x, 0.0f, 0.0f), color) // Stanga-jos
    };
    std::vector<unsigned int> indices2 = { 0, 1, 2 };

    // Verificam daca mesh-ul nu a fost deja creat
    if (meshes.find(meshNameRight) == meshes.end()) {
        Mesh* triangleRight = new Mesh(meshNameRight);
        triangleRight->InitFromData(vertices2, indices2);
        AddMeshToList(triangleRight);
    }

    RenderMesh2D(meshes[meshNameLeft], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes[meshNameRight], shaders["VertexColor"], modelMatrix);
}

// Functie pentru lansarea unui proiectil dintr-un tanc
void Tema1::LaunchProjectile(float tankX, float tankY, float tankAngle, float turretAngle, std::vector<Projectile>& projectiles, float initialSpeed) {
    // Calculam pozitia initiala a proiectilului (la varful turelei tancului)
    float startX = tankX + tankHeight * sin(-tankAngle) + turretLength * cos(turretAngle + tankAngle);
    float startY = tankY + tankHeight * cos(abs(tankAngle)) + turretLength * sin(turretAngle + tankAngle);

    // Cream un nou proiectil si ii setam pozitia initiala
    Projectile newProjectile;
    newProjectile.position = glm::vec2(startX, startY);

    // Calculam viteza initiala a proiectilului pe baza unghiului turelei si al tancului
    newProjectile.velocity = glm::vec2(cos(tankAngle + turretAngle), sin(tankAngle + turretAngle)) * initialSpeed;
    newProjectile.active = true; // Setam proiectilul ca fiind activ

    // Adaugam proiectilul in lista proiectilelor
    projectiles.push_back(newProjectile);
}

// Functie pentru actualizarea proiectilelor pe baza timpului scurs
void Tema1::UpdateProjectiles(std::vector<Projectile>& projectiles, float deltaTime) {
    // Iterăm prin toate proiectilele active
    for (auto& projectile : projectiles) {
        if (projectile.active) {
            // Calculăm următoarea poziție a proiectilului bazată pe viteza curentă și timpul trecut
			glm::vec2 nextPosition = projectile.position + projectile.velocity * deltaTime * timeScale; // timeScale pentru a controla viteza proiectilelor

            // Flag pentru a ține evidența dacă proiectilul a lovit ceva
            bool hitSomething = false;

            // Verificăm coliziunea cu Tancul 1
            if (tank1Active && (&projectiles != &projectilesTank1) && CheckCollision(projectile.position, tank1X, tank1Y)) {
                hitSomething = true;
                projectile.active = false;
                // Scădem viața tancului 1 și ne asigurăm că nu scade sub 0
                tank1Health = std::max(0.0f, tank1Health - 5.0f);
                // Dacă tancul nu mai are viață, îl dezactivăm
                if (tank1Health <= 0) {
                    tank1Active = false;
                }
            }

            // Verificăm coliziunea cu Tancul 2, similar cu verificarea pentru Tancul 1
            if (!hitSomething && tank2Active && (&projectiles != &projectilesTank2) && CheckCollision(projectile.position, tank2X, tank2Y)) {
                hitSomething = true;
                projectile.active = false;
                tank2Health = std::max(0.0f, tank2Health - 5.0f);
                if (tank2Health <= 0) {
                    tank2Active = false;
                }
            }

            // Dacă proiectilul nu a lovit niciun tanc, verificăm coliziunea cu terenul
            if (!hitSomething) {
                if (CheckTerrainCollision(projectile.position, nextPosition)) {
                    projectile.active = false;  // Dezactivăm proiectilul
                    DeformTerrain(projectile.position);  // Deformăm terenul la punctul de impact
                }
                else {
                    // Dacă nu există coliziune, actualizăm poziția proiectilului
                    projectile.position = nextPosition;
                }
            }

            // Aplicăm gravitația asupra proiectilului dacă este încă activ
            if (projectile.active) {
                projectile.velocity.y -= gravity * deltaTime * timeScale;
            }

            // Desenăm proiectilul dacă este activ
            if (projectile.active) {
                glm::mat3 modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(projectile.position.x, projectile.position.y);
                RenderMesh2D(meshes["cercprojectile"], shaders["VertexColor"], modelMatrix);
            }

            // Dezactivăm proiectilul dacă iese din ecran
            if (projectile.position.y < 0 || projectile.position.x < 0 ||
                projectile.position.x > window->GetResolution().x) {
                projectile.active = false;
            }
        }
    }
}

// Functie pentru desenarea barei de sanatate
void Tema1::DrawHealthBar(float x, float y, float health, float maxHealth, float healthBarWidth, float healthBarHeight) {
    // Calculam procentul de sanatate ramas
    float healthPercentage = health / maxHealth;
    float filledWidth = healthBarWidth * healthPercentage; // Lungimea barei umplute

    // Stabilim culoarea barei pe baza nivelului de sanatate
    glm::vec3 fillColor;
    if (health <= 15.0f) {
        fillColor = glm::vec3(1.0f, 0.0f, 0.0f);  // Rosu pentru sanatate critica
    }
    else {
        fillColor = glm::vec3(1.0f, 1.0f, 1.0f);  // Alb pentru sanatate normala
    }

    // Cream un dreptunghi wireframe pentru conturul barei
    glm::vec3 wireframeColor = glm::vec3(1.0f, 1.0f, 1.0f);
    std::vector<VertexFormat> wireframeVertices = {
        VertexFormat(glm::vec3(x - healthBarWidth / 2, y, 0), wireframeColor),                  // stanga-jos
        VertexFormat(glm::vec3(x + healthBarWidth / 2, y, 0), wireframeColor),                  // dreapta-jos
        VertexFormat(glm::vec3(x + healthBarWidth / 2, y + healthBarHeight, 0), wireframeColor), // dreapta-sus
        VertexFormat(glm::vec3(x - healthBarWidth / 2, y + healthBarHeight, 0), wireframeColor)  // stanga-sus
    };
    std::vector<unsigned int> wireframeIndices = { 0, 1, 2, 3, 0 };

    Mesh* wireframe = new Mesh("healthWireframe");
    wireframe->SetDrawMode(GL_LINE_LOOP);
    wireframe->InitFromData(wireframeVertices, wireframeIndices);
    RenderMesh2D(wireframe, shaders["VertexColor"], glm::mat3(1));

    // Cream dreptunghiul umplut pentru sanatatea curenta
    std::vector<VertexFormat> filledVertices = {
        VertexFormat(glm::vec3(x - healthBarWidth / 2, y, 0), fillColor),                      // stanga-jos
        VertexFormat(glm::vec3(x - healthBarWidth / 2 + filledWidth, y, 0), fillColor),        // dreapta-jos
        VertexFormat(glm::vec3(x - healthBarWidth / 2 + filledWidth, y + healthBarHeight, 0), fillColor), // dreapta-sus
        VertexFormat(glm::vec3(x - healthBarWidth / 2, y + healthBarHeight, 0), fillColor)     // stanga-sus
    };
    std::vector<unsigned int> filledIndices = { 0, 1, 2, 2, 3, 0 };

    Mesh* filled = new Mesh("healthFilled");
    filled->SetDrawMode(GL_TRIANGLES);
    filled->InitFromData(filledVertices, filledIndices);
    RenderMesh2D(filled, shaders["VertexColor"], glm::mat3(1));
}

// Functie care verifica coliziunea dintre proiectil si un tanc
bool Tema1::CheckCollision(const glm::vec2& projectilePos, float tankX, float tankY) {
    // Dimensiunile aproximative ale tancului
    float tankWidth = 70.0f;  // Latimea tancului (de la stanga la dreapta)

    // Calculam marginile dreptunghiului de coliziune al tancului
    float leftBound = tankX - tankWidth / 2;
    float rightBound = tankX + tankWidth / 2;
    float topBound = tankY + tankHeight;

    // Verificam daca proiectilul se afla in interiorul dreptunghiului de coliziune
    return (projectilePos.x >= leftBound && projectilePos.x <= rightBound &&
        projectilePos.y >= tankY && projectilePos.y <= topBound);
}

// Functie care deseneaza traiectoria unui proiectil
void Tema1::DrawTrajectory(float tankX, float tankY, float turretAngle, float initialSpeed, float tankAngle) {
    int numPoints = 50;    // Numarul de puncte care definesc traiectoria
    float timeStep = 0.05f; // Pasul de timp intre puncte pentru precizie

    // Calculam pozitia initiala a proiectilului la varful turelei
    float startX = tankX + tankHeight * sin(-tankAngle) + turretLength * cos(turretAngle + tankAngle);
    float startY = tankY + tankHeight * cos(abs(tankAngle)) + turretLength * sin(turretAngle + tankAngle);

    std::vector<glm::vec2> trajectoryPoints;

    // Calculam punctele traiectoriei pe baza miscarii balistice
    for (int i = 0; i < numPoints; i++) {
        float t = i * timeStep * timeScale;

        // Coordonatele x si y ale proiectilului la momentul t
        float x = startX + initialSpeed * cos(turretAngle + tankAngle) * t;
        float y = startY + initialSpeed * sin(turretAngle + tankAngle) * t - 0.5f * gravity * t * t;

        // Adaugam punctul in traiectorie
        trajectoryPoints.push_back(glm::vec2(x, y));

        // Ne oprim daca punctul coboara sub nivelul solului (y < 0)
        if (y < 0) break;
    }

    // Desenam linia traiectoriei intre punctele calculate
    for (int i = 0; i < trajectoryPoints.size() - 1; i++) {
        glm::vec2 p1 = trajectoryPoints[i];
        glm::vec2 p2 = trajectoryPoints[i + 1];
        DrawLine(p1, p2, glm::vec3(1, 1, 1));
    }
}

// Functie care deseneaza o linie intre doua puncte
void Tema1::DrawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec3& color) {
    static std::string lineMeshName = "line";

    // Cream mesh-ul pentru linie daca nu exista deja
    if (meshes.find(lineMeshName) == meshes.end()) {
        Mesh* line = new Mesh(lineMeshName);
        line->SetDrawMode(GL_LINES); // Setam modul de desenare pentru linii
        AddMeshToList(line);
    }

    // Actualizam vertecsii mesh-ului existent cu coordonatele liniei
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(start.x, start.y, 0), color), // inceput
        VertexFormat(glm::vec3(end.x, end.y, 0), color)      // final
    };

    meshes[lineMeshName]->InitFromData(vertices, std::vector<unsigned int>{0, 1});
    RenderMesh2D(meshes[lineMeshName], shaders["VertexColor"], glm::mat3(1));
}

// Functie pentru deformarea terenului la locul unei coliziuni
void Tema1::DeformTerrain(const glm::vec2& collisionPoint) {
    float explosionRadius = 100.0f; // Raza de actiune a deformarii
    float minTerrainHeight = 0.0f; // Inaltimea minima a terenului dupa deformare

    // Iteram prin toate punctele din harta de inaltime
    for (auto& point : heightMap) {
        float distance = glm::distance(point, collisionPoint); // Calculam distanta dintre punct si locul coliziunii
        if (distance < explosionRadius) {
            // Calculam deformarea in functie de distanta fata de centru
            float deformation = (explosionRadius - distance) * 0.5f;
            point.y = std::max(point.y - deformation, minTerrainHeight); // Aplicam deformarea fara sa coboram sub limita minima
        }
    }

    // Stergem toate mesh-urile asociate segmentelor de teren
    auto isTerrainMesh = [](const std::pair<std::string, Mesh*>& pair) {
        return pair.first.find("triangleLeft") != std::string::npos ||
            pair.first.find("triangleRight") != std::string::npos;  
        };

    // Iteram prin map-ul de mesh-uri pentru a elimina mesh-urile care apartin terenului
    for (auto it = meshes.begin(); it != meshes.end();) {
        if (isTerrainMesh(*it)) {
            // Daca mesh-ul curent este identificat ca parte a terenului, il stergem din map
            it = meshes.erase(it); // erase returneaza un iterator care indica urmatorul element
        }
        else {
            // Daca mesh-ul curent nu este parte a terenului, trecem la urmatorul element
            ++it;
        }
    }
}

// Functie pentru verificarea coliziunii unui proiectil cu terenul
bool Tema1::CheckTerrainCollision(const glm::vec2& currentPos, const glm::vec2& nextPos) {
    int indexCurrent = static_cast<int>(currentPos.x / 9.5f);
    int indexNext = static_cast<int>(nextPos.x / 9.5f);

    // Verificam daca indexii sunt validi si se afla in limitele hartii
    if (indexCurrent >= 0 && indexCurrent < heightMap.size() - 1 && indexNext < heightMap.size()) {
        // Calculam inaltimea terenului intre punctele curent si urmator
        float tCurrent = (currentPos.x - heightMap[indexCurrent].x) / (heightMap[indexCurrent + 1].x - heightMap[indexCurrent].x);
        float terrainHeightCurrent = heightMap[indexCurrent].y * (1 - tCurrent) + heightMap[indexCurrent + 1].y * tCurrent;

        float tNext = (nextPos.x - heightMap[indexNext].x) / (heightMap[indexNext + 1].x - heightMap[indexNext].x);
        float terrainHeightNext = heightMap[indexNext].y * (1 - tNext) + heightMap[indexNext + 1].y * tNext;

        // Returnam true daca proiectilul a trecut sub inaltimea terenului
        return (currentPos.y >= terrainHeightCurrent && nextPos.y <= terrainHeightNext);
    }
    return false; // Nicio coliziune detectata
}

// Functie care actualizeaza si deseneaza componentele jocului la fiecare cadru
void Tema1::Update(float deltaTimeSeconds) {
    // Redam terenul segment cu segment
    for (int i = 0; i < heightMap.size() - 1; i++) {
        CreateTerrainSegment(i);
    }

    // Limitele de miscare pentru nori
    float leftBound = 100.0f;
    float rightBound = 1200.0f;

    // Actualizam pozitiile norilor pe baza vitezei si directiei lor de miscare
    UpdateCloudPosition(cloud1X, cloud1MovingRight, cloudSpeed, deltaTimeSeconds, leftBound, rightBound);
    UpdateCloudPosition(cloud2X, cloud2MovingRight, cloudSpeed, deltaTimeSeconds, leftBound, rightBound);
    UpdateCloudPosition(cloud3X, cloud3MovingRight, cloudSpeed, deltaTimeSeconds, leftBound, rightBound);

    // Culoarea norilor
    glm::vec3 cloudColor = glm::vec3(0.7f, 0.85f, 1.0f);

    // Cream si desenam norii
    CreateCloud(cloud1X, 500, cloudColor, "Cloud1");
    CreateCloud(cloud2X, 550, cloudColor, "Cloud2");
    CreateCloud(cloud3X, 500, cloudColor, "Cloud3");

    // Calculam pozitia si unghiul pentru Tank 1
    int index1 = static_cast<int>(tank1X / 9.5f); // Determinam indexul punctului din heightMap
    if (index1 >= 0 && index1 < heightMap.size() - 1) {
        float x1 = heightMap[index1].x;
        float y1 = heightMap[index1].y;
        float x2 = heightMap[index1 + 1].x;
        float y2 = heightMap[index1 + 1].y;

        float t = (tank1X - x1) / (x2 - x1); // Interpolare liniara pentru determinarea pozitiei pe curba
        tank1Y = y1 + t * (y2 - y1);
        tank1Angle = atan2(y2 - y1, x2 - x1);
    }

    // Calculam pozitia si unghiul pentru Tank 2
    int index2 = static_cast<int>(tank2X / 9.5f); // Determinam indexul punctului din heightMap
    if (index2 >= 0 && index2 < heightMap.size() - 1) {
        float x1 = heightMap[index2].x;
        float y1 = heightMap[index2].y;
        float x2 = heightMap[index2 + 1].x;
        float y2 = heightMap[index2 + 1].y;

        float t = (tank2X - x1) / (x2 - x1); // Interpolare liniara pentru determinarea pozitiei pe curba
        tank2Y = y1 + t * (y2 - y1);
        tank2Angle = atan2(y2 - y1, x2 - x1);
    }

    // Daca tancul 1 este activ, il desenam impreuna cu traiectoria si bara de viata
    if (tank1Active) {
        RenderTank(tank1X, tank1Y, tank1Angle, "Tank1", turretAngle1);
        DrawTrajectory(tank1X, tank1Y, turretAngle1, initialSpeed1, tank1Angle);
        DrawHealthBar(tank1X, tank1Y + 50.0f, tank1Health, maxHealth, healthBarWidth, healthBarHeight);
    }

    // Daca tancul 2 este activ, il desenam impreuna cu traiectoria si bara de viata
    if (tank2Active) {
        RenderTank(tank2X, tank2Y, tank2Angle, "Tank2", turretAngle2);
        DrawTrajectory(tank2X, tank2Y, turretAngle2, initialSpeed2, tank2Angle);
        DrawHealthBar(tank2X, tank2Y + 50.0f, tank2Health, maxHealth, healthBarWidth, healthBarHeight);
    }

    // Actualizam proiectilele pentru ambele tancuri
    UpdateProjectiles(projectilesTank1, deltaTimeSeconds);
    UpdateProjectiles(projectilesTank2, deltaTimeSeconds);

    // Actualizăm proiectilele norilor
    UpdateCloudProjectiles(projectilesClouds, deltaTimeSeconds);
}

// Functie care actualizeaza input-urile de la tastatura
void Tema1::OnInputUpdate(float deltaTime, int mods) {
    glm::ivec2 resolution = window->GetResolution();
    float screenWidth = resolution.x;

    // Miscarea Tank 1 cu tastele A si D
    if (window->KeyHold(GLFW_KEY_D)) {
        tank1X += 100.0f * deltaTime;
        if (tank1X > screenWidth) {
            tank1X = screenWidth;
        }
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        tank1X -= 100.0f * deltaTime;
        if (tank1X < 0.0f) {
            tank1X = 0.0f;
        }
    }

    // Ajustarea unghiului turelei Tank 1 cu tastele W si S
    if (window->KeyHold(GLFW_KEY_W)) {
        turretAngle1 += 1.5f * deltaTime;
        if (turretAngle1 > glm::radians(180.0f)) {
            turretAngle1 = glm::radians(180.0f);
        }
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        turretAngle1 -= 1.5f * deltaTime;
        if (turretAngle1 < glm::radians(0.0f)) {
            turretAngle1 = glm::radians(0.0f);
        }
    }

    // Miscarea Tank 2 cu sagetile stanga si dreapta
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        tank2X += 100.0f * deltaTime;
        if (tank2X > screenWidth) {
            tank2X = screenWidth;
        }
    }
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        tank2X -= 100.0f * deltaTime;
        if (tank2X < 0.0f) {
            tank2X = 0.0f;
        }
    }

    // Ajustarea unghiului turelei Tank 2 cu sagetile sus si jos
    if (window->KeyHold(GLFW_KEY_UP)) {
        turretAngle2 += 1.5f * deltaTime;
        if (turretAngle2 > glm::radians(180.0f)) {
            turretAngle2 = glm::radians(180.0f);
        }
    }
    if (window->KeyHold(GLFW_KEY_DOWN)) {
        turretAngle2 -= 1.5f * deltaTime;
        if (turretAngle2 < glm::radians(0.0f)) {
            turretAngle2 = glm::radians(0.0f);
        }
    }
}

// Functie care detecteaza apasarile de taste pentru lansarea proiectilelor
void Tema1::OnKeyPress(int key, int mods) {
    if (key == GLFW_KEY_SPACE && tank1Active) { // Lansare proiectil pentru Tank 1
        LaunchProjectile(tank1X, tank1Y, tank1Angle, turretAngle1, projectilesTank1, initialSpeed1);
    }
    if (key == GLFW_KEY_ENTER && tank2Active) { // Lansare proiectil pentru Tank 2
        LaunchProjectile(tank2X, tank2Y, tank2Angle, turretAngle2, projectilesTank2, initialSpeed2);
    }
    if (key == GLFW_KEY_P) { // Lansare proiectil din nor
        // Selectăm un nor aleatoriu pentru lansare
        int randomCloud = rand() % 3;
        float cloudX;
        if (randomCloud == 0) {
            cloudX = cloud1X;
        }
        else if (randomCloud == 1) {
            cloudX = cloud2X;
        }
        else {
            cloudX = cloud3X;
        }
        float cloudY = 500.0f; // Înălțimea de unde se află norii

        // Lansăm proiectilul
        LaunchCloudProjectile(cloudX, cloudY, projectilesClouds);
    }
}

// Functie pentru desenarea componentelor unui tanc (corp, turela, etc.)
void Tema1::RenderTank(float x, float y, float angle, std::string name, float turretAngle) {
    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(x, y); // Translatie la pozitia tancului
    modelMatrix *= transform2D::Rotate(angle); // Rotire dupa unghiul tancului

    // Renderizam componentele tancului
    RenderMesh2D(meshes["bottomTrapez" + name], shaders["VertexColor"], modelMatrix);
    RenderMesh2D(meshes["topTrapez" + name], shaders["VertexColor"], modelMatrix);

    // Renderizam cercul din partea superioara a tancului
    glm::mat3 circleMatrix = modelMatrix;
    circleMatrix *= transform2D::Translate(0, 0.775f * tankLength);
    RenderMesh2D(meshes["cerc" + name], shaders["VertexColor"], circleMatrix);

    // Renderizam turela tancului
    glm::mat3 turretMatrix = circleMatrix;
    turretMatrix *= transform2D::Rotate(glm::radians(-90.0f)); // Aliniere initiala pe Ox
    turretMatrix *= transform2D::Rotate(turretAngle); // Rotire in functie de unghiul turelei
    turretMatrix *= transform2D::Translate(0, 10.0f);
    RenderMesh2D(meshes["turret" + name], shaders["VertexColor"], turretMatrix);
}

// Functie pentru crearea unui nor cu componentele sale (forme ovale)
void Tema1::CreateCloud(float centerX, float centerY, glm::vec3 color, std::string cloudName) {
    float baseRadiusX = 45.0f; // Latimea bazei norului
    float baseRadiusY = 30.0f; // Inaltimea bazei norului
    float topRadius = 30.0f;   // Raza cercului superior

    // Crearea partii stanga a norului
    glm::mat3 leftModelMatrix = glm::mat3(1);
    leftModelMatrix *= transform2D::Translate(centerX - baseRadiusX, centerY);
    leftModelMatrix *= transform2D::Scale(1.5f, 1.0f); // Scalare pentru un aspect oval
    RenderMesh2D(meshes["cerccloudLeft"], shaders["VertexColor"], leftModelMatrix);

    // Crearea partii centrale a norului
    glm::mat3 middleModelMatrix = glm::mat3(1);
    middleModelMatrix *= transform2D::Translate(centerX, centerY);
    middleModelMatrix *= transform2D::Scale(1.5f, 1.0f); // Scalare pentru un aspect oval
    RenderMesh2D(meshes["cerccloudMiddle"], shaders["VertexColor"], middleModelMatrix);

    // Crearea partii dreapta a norului
    glm::mat3 rightModelMatrix = glm::mat3(1);
    rightModelMatrix *= transform2D::Translate(centerX + baseRadiusX, centerY);
    rightModelMatrix *= transform2D::Scale(1.5f, 1.0f); // Scalare pentru un aspect oval
    RenderMesh2D(meshes["cerccloudRight"], shaders["VertexColor"], rightModelMatrix);

    // Crearea partii superioare a norului
    glm::mat3 topModelMatrix = glm::mat3(1);
    topModelMatrix *= transform2D::Translate(centerX, centerY + topRadius);
    topModelMatrix *= transform2D::Scale(1.3f, 1.05f); // Scalare pentru a taia o parte din cerc
    RenderMesh2D(meshes["cerccloudTop"], shaders["VertexColor"], topModelMatrix);
}

// Functie pentru actualizarea pozitiei norilor in functie de limite
void Tema1::UpdateCloudPosition(float& cloudX, bool& movingRight, float speed, float deltaTime, float leftBound, float rightBound) {
    if (movingRight) { // Daca norul se misca spre dreapta
        cloudX += speed * deltaTime;
        if (cloudX > rightBound) movingRight = false; // Inversam directia la limita dreapta
    }
    else { // Daca norul se misca spre stanga
        cloudX -= speed * deltaTime;
        if (cloudX < leftBound) movingRight = true; // Inversam directia la limita stanga
    }
}

// Functie pentru actualizarea proiectilelor norilor
void Tema1::UpdateCloudProjectiles(std::vector<Projectile>& projectiles, float deltaTime) {
    for (auto& projectile : projectiles) {
        if (projectile.active) {
            // Calculăm poziția următoare a proiectilului
            glm::vec2 nextPosition = projectile.position + projectile.velocity * deltaTime;

            // Verificăm dacă proiectilul lovește terenul
            if (CheckTerrainCollision(projectile.position, nextPosition)) {
                projectile.active = false;
                DeformTerrain(projectile.position);
                continue;
            }

            // Verificăm dacă proiectilul lovește Tank 1
            if (tank1Active && CheckCollision(projectile.position, tank1X, tank1Y)) {
                projectile.active = false; // Dezactivăm proiectilul
                tank1Health = std::max(0.0f, tank1Health - 10.0f); // Scădem 10 puncte de viață din Tank 1
                if (tank1Health <= 0) {
                    tank1Active = false; // Dezactivăm Tank 1 dacă viața ajunge la 0
                }
                continue;
            }

            // Verificăm dacă proiectilul lovește Tank 2
            if (tank2Active && CheckCollision(projectile.position, tank2X, tank2Y)) {
                projectile.active = false; // Dezactivăm proiectilul
                tank2Health = std::max(0.0f, tank2Health - 10.0f); // Scădem 10 puncte de viață din Tank 2
                if (tank2Health <= 0) {
                    tank2Active = false; // Dezactivăm Tank 2 dacă viața ajunge la 0
                }
                continue;
            }

            // Dezactivăm proiectilul dacă iese din ecran
            if (projectile.position.y < 0) {
                projectile.active = false;
                continue;
            }

            // Actualizăm poziția proiectilului
            projectile.position = nextPosition;

            // Renderizăm proiectilul dacă este activ
            glm::mat3 modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(projectile.position.x, projectile.position.y);
            RenderMesh2D(meshes["cerccloudProjectile"], shaders["VertexColor"], modelMatrix);
        }
    }
}

// Functie pentru lansarea unui proiectil dintr-un nor
void Tema1::LaunchCloudProjectile(float cloudX, float cloudY, std::vector<Projectile>& projectiles) {
    // Creăm un nou proiectil și îi setăm poziția inițială (centrul norului)
    Projectile newProjectile;
    newProjectile.position = glm::vec2(cloudX, cloudY);

    // Setăm viteza proiectilului în jos
    newProjectile.velocity = glm::vec2(0, -projectileSpeed);
    newProjectile.active = true;

    // Adăugăm proiectilul în lista de proiectile
    projectiles.push_back(newProjectile);
}
