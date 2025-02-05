#include "lab_m1/Tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/Tema2/tema2_transform3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}

// Initializarea obiectelor si setarilor pentru scena
void Tema2::Init() {
    polygonMode = GL_FILL; // Mod de desenare initial (plin)

    // Crearea componentelor dronei (cub si elice)
    meshes["cube"] = CreateCube("cube", glm::vec3(0.5f, 0.5f, 0.5f));
    meshes["propeller"] = CreatePropeller("propeller", glm::vec3(0.0f, 0.0f, 0.0f));

    // Initializare camera (camera dronei)
    camera = new implemented::DroneCamera();

    // Initializare variabile pentru pozitia, scalarea si rotatia dronei
    translateX = translateY = translateZ = 0; // Pozitie initiala
    scaleX = scaleY = scaleZ = 1; // Scalare initiala
    angularStepOX = angularStepOY = angularStepOZ = 0; // Rotatii initiale

    // Configurare matrice de proiectie si viewport pentru camera principala
    float fov = 60; // Unghiul de vizualizare al camerei (Field of View)
    projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);

    // Configurare pentru minimap
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);

    propellerRotation = 0;      // Rotatia initiala a elicei
    mouseSensitivity = 0.002f;  // Sensibilitatea mouse-ului pentru rotatia camerei

    // Crearea shaderului pentru teren (Shader custom pentru iluminare si detalii)
    Shader* shader = new Shader("TerrainShader");
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    // Crearea terenului (grid pentru suprafata de jos) si altor obiecte
    meshes["terrain"] = CreateTerrain("terrain", 150); // Grid mare pentru mai multe detalii
    meshes["tree"] = CreateTree("tree");
    meshes["building"] = CreateBuilding("building");

    // Crearea unei zone sigure in jurul dronei la inceputul jocului
    float safeZoneRadius = 4.0f;

    // Generare cladiri in scena
    int numBuildings = 9;
    float minBuildingDistance = 6.0f; // Distanta minima intre cladiri

    for (int i = 0; i < numBuildings; i++) {
        bool validPosition = false;
        glm::vec3 newPos;
        int maxAttempts = 100; // Numarul maxim de incercari pentru a gasi o pozitie valida
        int attempts = 0;

        // Cauta o pozitie valida pentru cladire
        while (!validPosition && attempts < maxAttempts) {
            float x = rand() % 20 - 10; // Pozitie random pe axa X
            float z = rand() % 20 - 10; // Pozitie random pe axa Z
            newPos = glm::vec3(x, 0, z);

            // Verifica daca pozitia este in afara zonei sigure
            if (glm::distance(glm::vec2(x, z), glm::vec2(0, 0)) < safeZoneRadius) {
                attempts++;
                continue;
            }

            validPosition = true;
            // Verifica daca pozitia nu se suprapune cu alte cladiri
            for (const auto& pos : buildingPositions) {
                if (glm::distance(glm::vec2(x, z), glm::vec2(pos.x, pos.z)) < minBuildingDistance) {
                    validPosition = false;
                    break;
                }
            }
            attempts++;
        }

        if (validPosition) {
            buildingPositions.push_back(newPos); // Adauga pozitia la lista cladirilor
            // Dimensiuni aleatorii pentru cladiri
            float width = 2.0f + static_cast<float>(rand()) / RAND_MAX * 1.0f;    // Intre 2.0 si 3.0
            float height = 3.0f + static_cast<float>(rand()) / RAND_MAX * 2.0f;   // Intre 3.0 si 5.0
            float depth = 2.0f + static_cast<float>(rand()) / RAND_MAX * 1.0f;    // Intre 2.0 si 3.0
            buildingSizes.push_back(glm::vec3(width, height, depth)); // Salveaza dimensiunea
        }
    }

    // Generare copaci in scena
    int numTrees = 20; // Numarul de copaci generati
    float minTreeDistance = 4.0f; // Distanta minima intre copaci si alte obiecte

    for (int i = 0; i < numTrees; i++) {
        bool validPosition = false;
        glm::vec3 newPos;
        int maxAttempts = 100; // Numarul maxim de incercari pentru a gasi o pozitie valida
        int attempts = 0;

        // Cauta o pozitie valida pentru copac
        while (!validPosition && attempts < maxAttempts) {
            float x = rand() % 20 - 10; // Pozitie random pe axa X
            float z = rand() % 20 - 10; // Pozitie random pe axa Z
            newPos = glm::vec3(x, 0, z);

            // Verifica daca pozitia este in afara zonei sigure
            if (glm::distance(glm::vec2(x, z), glm::vec2(0, 0)) < safeZoneRadius) {
                attempts++;
                continue;
            }

            validPosition = true;
            // Verifica daca pozitia nu se suprapune cu cladiri
            for (const auto& pos : buildingPositions) {
                if (glm::distance(glm::vec2(x, z), glm::vec2(pos.x, pos.z)) < minTreeDistance) {
                    validPosition = false;
                    break;
                }
            }

            // Verifica daca pozitia nu se suprapune cu alti copaci
            if (validPosition) {
                for (const auto& pos : treePositions) {
                    if (glm::distance(glm::vec2(x, z), glm::vec2(pos.x, pos.z)) < minTreeDistance) {
                        validPosition = false;
                        break;
                    }
                }
            }
            attempts++;
        }

        if (validPosition) {
            treePositions.push_back(newPos); // Adauga pozitia la lista copacilor
            // Dimensiuni aleatorii pentru copaci
            float scale = 1.2f + static_cast<float>(rand()) / RAND_MAX * 0.8f; // Intre 1.2 si 2.0
            treeSizes.push_back(scale); // Salveaza dimensiunea copacului
        }
    }

    // Crearea obiectelor pentru gameplay
    meshes["package"] = CreatePackage("package"); // Obiect pachet pentru livrare
    meshes["delivery-point"] = CreateDeliveryPoint("delivery-point"); // Punct de livrare
    meshes["direction-arrow"] = CreateDirectionArrow("direction-arrow"); // Sageata directionala

    // Initializare stare de joc
    score = 0; // Scor initial
    packageAttached = false; // Pachetul nu este atasat initial
    SpawnNewPackage(); // Genereaza primul pachet pentru livrare

    // Crearea markerelor pentru pachet si punctul de livrare
    meshes["pickup-marker"] = CreateCylinder("pickup-marker", glm::vec3(0.5f, 0.7f, 1.0f)); // Marker pentru pachet
    meshes["delivery-marker"] = CreateCylinder("delivery-marker", glm::vec3(0.3f, 0.9f, 0.3f)); // Marker pentru livrare

    // Crearea norilor 
    meshes["cloud"] = CreateCloud("cloud");
    for (int i = 0; i < 17; i++) {
        Cloud cloud;
        cloud.position = glm::vec3(
            rand() % 60 - 30, // Pozitie random pe X
            8.0f + rand() % 8, // Pozitie random pe Y
            rand() % 60 - 30 // Pozitie random pe Z
        );
        cloud.speed = 0.3f + static_cast<float>(rand()) / RAND_MAX * 0.5f; // Viteza random pentru nori
        clouds.push_back(cloud);
    }

    // Seteaza modul de vedere third-person initial
    firstPersonView = false;
    camera->SetFirstPerson(false);
}

// Pregătirea frame-ului curent (culoare fundal și adâncime)
void Tema2::FrameStart()
{
    // Culoare albastru deschis pentru cer
    glClearColor(0.529f, 0.808f, 0.922f, 1); // Sky blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Șterge buffer-ul pentru culoare și adâncime

}

// Crearea unui pachet pentru livrare
Mesh* m1::Tema2::CreateCube(const std::string& name, glm::vec3 color) {

    // Vertecsii unui cub
    std::vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(-0.5f, -0.5f,  0.5f), color), // 0
        VertexFormat(glm::vec3(0.5f, -0.5f,  0.5f), color), // 1
        VertexFormat(glm::vec3(-0.5f,  0.5f,  0.5f), color), // 2
        VertexFormat(glm::vec3(0.5f,  0.5f,  0.5f), color), // 3
        VertexFormat(glm::vec3(-0.5f, -0.5f, -0.5f), color), // 4
        VertexFormat(glm::vec3(0.5f, -0.5f, -0.5f), color), // 5
        VertexFormat(glm::vec3(-0.5f,  0.5f, -0.5f), color), // 6
        VertexFormat(glm::vec3(0.5f,  0.5f, -0.5f), color)  // 7
    };

    // Indicii pentru desenarea cubului
    std::vector<unsigned int> indices =
    {
        0, 1, 2,  1, 3, 2,  // Fața față
        2, 3, 6,  3, 7, 6,  // Fața sus
        1, 5, 3,  5, 7, 3,  // Fața dreapta
        4, 0, 6,  0, 2, 6,  // Fața stânga
        4, 5, 0,  5, 1, 0,  // Fața jos
        5, 4, 7,  4, 6, 7   // Fața spate
    };

    // Creare mesh
    Mesh* cube = new Mesh(name);
    cube->InitFromData(vertices, indices);
    return cube;
}

// Creare pachet (cub)
Mesh* Tema2::CreatePackage(const std::string& name) {
    glm::vec3 packageColor(0.8f, 0.6f, 0.4f);  // Brown box
    return CreateCube(name, packageColor);
}

// Crearea elicelor pentru drona
Mesh* m1::Tema2::CreatePropeller(const std::string& name, glm::vec3 color) {
    // Vertecsii unei elice (paralelipiped subțire)
    std::vector<VertexFormat> vertices
    {
        VertexFormat(glm::vec3(-0.5f, -0.05f,  0.2f), color), // 0
        VertexFormat(glm::vec3(0.5f, -0.05f,  0.2f), color), // 1
        VertexFormat(glm::vec3(-0.5f,  0.05f,  0.2f), color), // 2
        VertexFormat(glm::vec3(0.5f,  0.05f,  0.2f), color), // 3
        VertexFormat(glm::vec3(-0.5f, -0.05f, -0.2f), color), // 4
        VertexFormat(glm::vec3(0.5f, -0.05f, -0.2f), color), // 5
        VertexFormat(glm::vec3(-0.5f,  0.05f, -0.2f), color), // 6
        VertexFormat(glm::vec3(0.5f,  0.05f, -0.2f), color)  // 7
    };

    // Indicii pentru elice
    std::vector<unsigned int> indices =
    {
        0, 1, 2,  1, 3, 2,  // Fața față
        2, 3, 6,  3, 7, 6,  // Fața sus
        1, 5, 3,  5, 7, 3,  // Fața dreapta
        4, 0, 6,  0, 2, 6,  // Fața stânga
        4, 5, 0,  5, 1, 0,  // Fața jos
        5, 4, 7,  4, 6, 7   // Fața spate
    };

    // Creare mesh
    Mesh* propeller = new Mesh(name);
    propeller->InitFromData(vertices, indices);
    return propeller;
}

// Creare copac (trunchi și frunze)
Mesh* m1::Tema2::CreateTree(const std::string& name) {
    glm::vec3 trunkColor(0.5f, 0.35f, 0.05f);  // Maro pentru trunchi
    glm::vec3 leavesColor(0.0f, 0.8f, 0.0f);   // Verde pentru frunze

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Trunchi
    vertices.push_back(VertexFormat(glm::vec3(-0.1f, 0.0f, 0.1f), trunkColor));   // 0
    vertices.push_back(VertexFormat(glm::vec3(0.1f, 0.0f, 0.1f), trunkColor));    // 1
    vertices.push_back(VertexFormat(glm::vec3(-0.1f, 0.5f, 0.1f), trunkColor));   // 2
    vertices.push_back(VertexFormat(glm::vec3(0.1f, 0.5f, 0.1f), trunkColor));    // 3
    vertices.push_back(VertexFormat(glm::vec3(-0.1f, 0.0f, -0.1f), trunkColor));  // 4
    vertices.push_back(VertexFormat(glm::vec3(0.1f, 0.0f, -0.1f), trunkColor));   // 5
    vertices.push_back(VertexFormat(glm::vec3(-0.1f, 0.5f, -0.1f), trunkColor));  // 6
    vertices.push_back(VertexFormat(glm::vec3(0.1f, 0.5f, -0.1f), trunkColor));   // 7

    // Indici pentru trunchi
    std::vector<unsigned int> trunkIndices = {
        0, 1, 2,    1, 3, 2,  // față
        2, 3, 6,    3, 7, 6,  // sus
        1, 5, 3,    5, 7, 3,  // dreapta
        4, 0, 6,    0, 2, 6,  // stânga
        4, 5, 0,    5, 1, 0,  // jos
        5, 4, 7,    4, 6, 7   // spate
    };
    indices.insert(indices.end(), trunkIndices.begin(), trunkIndices.end());

    // Conul de jos (mai mare)
    int segments = 20;
    float bottomRadius = 0.5f;
    float bottomHeight = 0.8f;
    float bottomBase = 0.5f;  // înălțimea de la care începe conul

    // Vârful conului de jos
    vertices.push_back(VertexFormat(glm::vec3(0, bottomBase + bottomHeight, 0), leavesColor));  // 8

    // Baza conului de jos (cercul)
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = bottomRadius * cos(angle);
        float z = bottomRadius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, bottomBase, z), leavesColor));
    }

    // Conul de sus (mai mic)
    float topRadius = 0.3f;
    float topHeight = 0.6f;
    float topBase = bottomBase + bottomHeight * 0.7f;  // Se suprapune cu conul de jos

    // Vârful conului de sus
    vertices.push_back(VertexFormat(glm::vec3(0, topBase + topHeight, 0), leavesColor));

    // Baza conului de sus (cercul)
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = topRadius * cos(angle);
        float z = topRadius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, topBase, z), leavesColor));
    }

    // Indici pentru conul de jos
    int bottomTipIndex = 8;
    int bottomBaseStart = 9;
    for (int i = 0; i < segments; i++) {
        indices.push_back(bottomTipIndex);
        indices.push_back(bottomBaseStart + i);
        indices.push_back(bottomBaseStart + i + 1);
    }

    // Indici pentru conul de sus
    int topTipIndex = bottomBaseStart + segments + 1;
    int topBaseStart = topTipIndex + 1;
    for (int i = 0; i < segments; i++) {
        indices.push_back(topTipIndex);
        indices.push_back(topBaseStart + i);
        indices.push_back(topBaseStart + i + 1);
    }

    // Creare mesh
    Mesh* tree = new Mesh(name);
    tree->InitFromData(vertices, indices);
    return tree;
}

// Randarea obiectelor in scena
void Tema2::Render() {
    // Brațul 1 al dronei - Rotit la +45°
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::RotateOY(angularStepOY);  // Rotația în jurul propriei axe
    modelMatrix *= transform3D::RotateOY(glm::radians(45.0f));
    modelMatrix *= transform3D::Scale(0.7f, 0.1f, 0.05f);  
    RenderMesh(meshes["cube"], shaders["VertexColor"], modelMatrix);

    // Brațul 2 al dronei - Rotit la -45°
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::RotateOY(angularStepOY);
    modelMatrix *= transform3D::RotateOY(glm::radians(-45.0f));
    modelMatrix *= transform3D::Scale(0.7f, 0.1f, 0.05f);
    RenderMesh(meshes["cube"], shaders["VertexColor"], modelMatrix);

	// Randare cuburi si elice pentru drona
    float armLength = 0.35f;
	float lung = 0.07f;  // offset pentru a pozitiona bine cuburile si elicele pe lungilea bratelor dronei
    float raport = 1.5f;
	float rap_elice = 0.025f; // offset pentru a pozitiona bine elicele fix deasupra cuburilor

    for (int i = 0; i < 4; i++) {
        float x = (i < 2 ? armLength / raport : -(armLength / raport));
        float z = (i % 2 == 0 ? armLength / raport : -(armLength / raport));

        // Mai întâi rotim punctele relative la centrul dronei
        glm::mat4 rotationMatrix = transform3D::RotateOY(angularStepOY);
        glm::vec4 localPos = glm::vec4(x, 0, z, 1);
        glm::vec4 rotatedPos = rotationMatrix * localPos;

        // Cuburi
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(translateX + rotatedPos.x, translateY + lung, translateZ + rotatedPos.z);
        modelMatrix *= transform3D::RotateOY(angularStepOY);
        modelMatrix *= transform3D::Scale(0.03f, 0.05f, 0.05f);
        RenderMesh(meshes["cube"], shaders["VertexColor"], modelMatrix);

        // Elice 1
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(translateX + rotatedPos.x, translateY + lung + rap_elice, translateZ + rotatedPos.z);
        modelMatrix *= transform3D::RotateOY(angularStepOY);
        modelMatrix *= transform3D::RotateOY(propellerRotation);
        modelMatrix *= transform3D::Scale(0.15f, 0.01f, 0.05f);
        RenderMesh(meshes["propeller"], shaders["VertexColor"], modelMatrix);

        // Elice 2
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(translateX + rotatedPos.x, translateY + lung + rap_elice, translateZ + rotatedPos.z);
        modelMatrix *= transform3D::RotateOY(angularStepOY);
        modelMatrix *= transform3D::RotateOY(propellerRotation + glm::radians(90.0f));
        modelMatrix *= transform3D::Scale(0.15f, 0.01f, 0.05f);
        RenderMesh(meshes["propeller"], shaders["VertexColor"], modelMatrix);
    }

	// Randare teren
    glm::mat4 terrainMatrix = glm::mat4(1);
    terrainMatrix *= transform3D::Translate(0, 0, 0); // Asigură-te că e la nivel 0
    terrainMatrix *= transform3D::Scale(4.0f, 1.0f, 4.0f); // Scalare mai mare pentru teren
    RenderMesh(meshes["terrain"], shaders["TerrainShader"], terrainMatrix);

	// Randam copacii
    for (size_t i = 0; i < treePositions.size(); i++) {
        glm::mat4 treeMatrix = glm::mat4(1);
        treeMatrix *= transform3D::Translate(treePositions[i].x, treePositions[i].y, treePositions[i].z);
        treeMatrix *= transform3D::Scale(treeSizes[i], treeSizes[i], treeSizes[i]);
        RenderMesh(meshes["tree"], shaders["VertexColor"], treeMatrix);
    }

	// Rendam cladirile
    for (size_t i = 0; i < buildingPositions.size(); i++) {
        glm::mat4 buildingMatrix = glm::mat4(1);
        buildingMatrix *= transform3D::Translate(
            buildingPositions[i].x,
            buildingPositions[i].y,
            buildingPositions[i].z
        );
        buildingMatrix *= transform3D::Scale(
            buildingSizes[i].x,
            buildingSizes[i].y,
            buildingSizes[i].z
        );
        RenderMesh(meshes["building"], shaders["VertexColor"], buildingMatrix);
    }

	// Rendam pachetul daca nu este colectat
    if (!currentPackage.isCollected) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(
            currentPackage.position.x,
            currentPackage.position.y,
            currentPackage.position.z
        );
        modelMatrix *= transform3D::Scale(
            currentPackage.size,
            currentPackage.size,
            currentPackage.size
        );
        RenderMesh(meshes["package"], shaders["VertexColor"], modelMatrix);
    }

	// Randam punctul de livrare
    if (packageAttached) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(
            deliveryPoint.position.x,
            deliveryPoint.position.y,
            deliveryPoint.position.z
        );
        modelMatrix *= transform3D::Scale(
            deliveryPoint.radius,
            1.0f,
            deliveryPoint.radius
        );
        RenderMesh(meshes["delivery-point"], shaders["VertexColor"], modelMatrix);
    }

	// Rendam pachetul atasat la drona
    if (packageAttached) {
        glm::mat4 packageMatrix = glm::mat4(1);
        packageMatrix *= transform3D::Translate(translateX, translateY - 0.7f, translateZ);
        packageMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
        RenderMesh(meshes["package"], shaders["VertexColor"], packageMatrix);
    }


	// Inainte de a randarea cilindrilor
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // Modificare aspect compact pentru cilindri (sa fie putin transparenti)

	// Randam locul de colectare a pachetului
    if (!currentPackage.isCollected) {
        glm::mat4 markerMatrix = glm::mat4(1);
        markerMatrix *= transform3D::Translate(currentPackage.position.x, 0, currentPackage.position.z);
        markerMatrix *= transform3D::Scale(1.0f, 1.0f, 1.0f);

		// Randam cilindrul sa fie putin transparent
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        RenderMesh(meshes["pickup-marker"], shaders["VertexColor"], markerMatrix);
        glDisable(GL_BLEND);
    }

	// Randam cilindrul pentru punctul de livrare
    if (packageAttached) {
        glm::mat4 markerMatrix = glm::mat4(1);
        markerMatrix *= transform3D::Translate(deliveryPoint.position.x, 0, deliveryPoint.position.z);
        markerMatrix *= transform3D::Scale(1.0f, 1.0f, 1.0f);
        
		// Randam cilindrul sa fie putin transparent
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        RenderMesh(meshes["delivery-marker"], shaders["VertexColor"], markerMatrix);
        glDisable(GL_BLEND);
    }

	// Dupa randarea cilindrilor
	glDepthMask(GL_TRUE);   // Revenire la modul normal de randare
    glDisable(GL_BLEND);

	// Vedem catre ce va indica sageata (pachetul sau punctul de livrare)
    glm::vec3 target;
    if (packageAttached) {
		target = deliveryPoint.position;
    }
    else {
        target = currentPackage.position;
    }
    
    glm::vec3 dronePos = glm::vec3(translateX, translateY, translateZ);

	// Calculam directia dintre drona si planul de livrare sau pachet pentru sageata
    glm::vec2 directionToTarget = glm::normalize(glm::vec2(target.x - dronePos.x, target.z - dronePos.z));

    // Pozitionam sageata si in functie de directia camerei 
    glm::mat4 arrowModelMatrix = glm::mat4(1);
    glm::vec3 cameraPosition = camera->position;
    glm::vec3 cameraForward = camera->forward;

	// Calculam pozitia si rotatia sagetii
    glm::vec3 arrowPosition = cameraPosition + cameraForward * 3.0f;
    arrowPosition.y = translateY; // Menținem săgeata la aceeași înălțime ca drona
    arrowModelMatrix *= transform3D::Translate(arrowPosition.x, arrowPosition.y - 0.2f, arrowPosition.z);
    arrowModelMatrix *= transform3D::RotateOY(arrowRotation); 
    arrowModelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    RenderMesh(meshes["direction-arrow"], shaders["VertexColor"], arrowModelMatrix);

}

// Crearea terenului (grid pentru suprafața de jos)
Mesh* Tema2::CreateTerrain(const std::string& name, int gridSize) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float cellSize = 0.1f;
    float halfSize = (gridSize * cellSize) / 2.0f;

    // Generare vertices pentru grid plat
    for (int i = 0; i <= gridSize; i++) {
        for (int j = 0; j <= gridSize; j++) {
            float x = (j * cellSize) - halfSize;
            float z = (i * cellSize) - halfSize;

            // Culoare de bază verde pentru teren
            glm::vec3 color = glm::vec3(0.133f, 0.545f, 0.133f);

            // Normal inițial în sus
            glm::vec3 normal = glm::vec3(0, 1, 0);

            vertices.push_back(VertexFormat(glm::vec3(x, 0, z), color, normal));
        }
    }

    // Generare indices pentru triunghiuri
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            int topLeft = i * (gridSize + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * (gridSize + 1) + j;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    Mesh* terrain = new Mesh(name);
    terrain->InitFromData(vertices, indices);
    return terrain;
}

// Creare de cladiri
Mesh* m1::Tema2::CreateBuilding(const std::string& name) {
    glm::vec3 buildingColor(0.7f, 0.7f, 0.7f);    // Gri pentru clădiri
    glm::vec3 windowColor(0.3f, 0.5f, 1.0f);      // Albastru pentru ferestre
    glm::vec3 doorColor(0.45f, 0.25f, 0.15f);     // Maro pentru ușă

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Pereții principali
    // Față
    vertices.push_back(VertexFormat(glm::vec3(-0.5f, 0.0f, 0.5f), buildingColor));  // 0
    vertices.push_back(VertexFormat(glm::vec3(0.5f, 0.0f, 0.5f), buildingColor));   // 1
    vertices.push_back(VertexFormat(glm::vec3(-0.5f, 1.0f, 0.5f), buildingColor));  // 2
    vertices.push_back(VertexFormat(glm::vec3(0.5f, 1.0f, 0.5f), buildingColor));   // 3

    // Spate
    vertices.push_back(VertexFormat(glm::vec3(-0.5f, 0.0f, -0.5f), buildingColor));  // 4
    vertices.push_back(VertexFormat(glm::vec3(0.5f, 0.0f, -0.5f), buildingColor));   // 5
    vertices.push_back(VertexFormat(glm::vec3(-0.5f, 1.0f, -0.5f), buildingColor));  // 6
    vertices.push_back(VertexFormat(glm::vec3(0.5f, 1.0f, -0.5f), buildingColor));   // 7

    // Indici pentru pereți
    indices = {
        // Fața frontală
        0, 1, 2,    1, 3, 2,
        // Fața din spate
        4, 5, 6,    5, 7, 6,
        // Fața stângă
        4, 0, 6,    0, 2, 6,
        // Fața dreaptă
        1, 5, 3,    5, 7, 3,
        // Partea de sus
        2, 3, 6,    3, 7, 6,
        // Partea de jos
        0, 1, 4,    1, 5, 4,
    };

    // Ușă față (dreptunghi maro pe fața frontală)
    int doorBaseIndex = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(-0.15f, 0.0f, 0.501f), doorColor));
    vertices.push_back(VertexFormat(glm::vec3(0.15f, 0.0f, 0.501f), doorColor));
    vertices.push_back(VertexFormat(glm::vec3(-0.15f, 0.25f, 0.501f), doorColor));  // înălțime redusă la 0.25
    vertices.push_back(VertexFormat(glm::vec3(0.15f, 0.25f, 0.501f), doorColor));   // în loc de 0.4

    indices.push_back(doorBaseIndex);
    indices.push_back(doorBaseIndex + 1);
    indices.push_back(doorBaseIndex + 2);
    indices.push_back(doorBaseIndex + 1);
    indices.push_back(doorBaseIndex + 3);
    indices.push_back(doorBaseIndex + 2);

    // Ușă spate (dreptunghi maro pe fața din spate)
    doorBaseIndex = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(-0.15f, 0.0f, -0.501f), doorColor));
    vertices.push_back(VertexFormat(glm::vec3(0.15f, 0.0f, -0.501f), doorColor));
    vertices.push_back(VertexFormat(glm::vec3(-0.15f, 0.25f, -0.501f), doorColor));  // aceeași înălțime
    vertices.push_back(VertexFormat(glm::vec3(0.15f, 0.25f, -0.501f), doorColor));

    indices.push_back(doorBaseIndex);
    indices.push_back(doorBaseIndex + 1);
    indices.push_back(doorBaseIndex + 2);
    indices.push_back(doorBaseIndex + 1);
    indices.push_back(doorBaseIndex + 3);
    indices.push_back(doorBaseIndex + 2);

    // Adăugăm 3 rânduri de ferestre pe fiecare fațadă
    float windowHeights[] = { 0.3f, 0.5f, 0.7f };
    float windowWidth = 0.15f;
    float windowOffset = 0.25f;

    for (float height : windowHeights) {
        // Ferestre față
        int windowBaseIndex = vertices.size();
        vertices.push_back(VertexFormat(glm::vec3(-windowOffset - windowWidth, height, 0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(-windowOffset + windowWidth, height, 0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(-windowOffset - windowWidth, height + 0.15f, 0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(-windowOffset + windowWidth, height + 0.15f, 0.501f), windowColor));

        indices.push_back(windowBaseIndex);
        indices.push_back(windowBaseIndex + 1);
        indices.push_back(windowBaseIndex + 2);
        indices.push_back(windowBaseIndex + 1);
        indices.push_back(windowBaseIndex + 3);
        indices.push_back(windowBaseIndex + 2);

        // A doua fereastră pe același nivel
        windowBaseIndex = vertices.size();
        vertices.push_back(VertexFormat(glm::vec3(windowOffset - windowWidth, height, 0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(windowOffset + windowWidth, height, 0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(windowOffset - windowWidth, height + 0.15f, 0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(windowOffset + windowWidth, height + 0.15f, 0.501f), windowColor));

        indices.push_back(windowBaseIndex);
        indices.push_back(windowBaseIndex + 1);
        indices.push_back(windowBaseIndex + 2);
        indices.push_back(windowBaseIndex + 1);
        indices.push_back(windowBaseIndex + 3);
        indices.push_back(windowBaseIndex + 2);

        // Ferestre spate (offset ușor pentru a evita z-fighting)
        windowBaseIndex = vertices.size();
        vertices.push_back(VertexFormat(glm::vec3(-windowOffset - windowWidth, height, -0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(-windowOffset + windowWidth, height, -0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(-windowOffset - windowWidth, height + 0.15f, -0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(-windowOffset + windowWidth, height + 0.15f, -0.501f), windowColor));

        indices.push_back(windowBaseIndex);
        indices.push_back(windowBaseIndex + 1);
        indices.push_back(windowBaseIndex + 2);
        indices.push_back(windowBaseIndex + 1);
        indices.push_back(windowBaseIndex + 3);
        indices.push_back(windowBaseIndex + 2);

        // A doua fereastră pe același nivel
        windowBaseIndex = vertices.size();
        vertices.push_back(VertexFormat(glm::vec3(windowOffset - windowWidth, height, -0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(windowOffset + windowWidth, height, -0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(windowOffset - windowWidth, height + 0.15f, -0.501f), windowColor));
        vertices.push_back(VertexFormat(glm::vec3(windowOffset + windowWidth, height + 0.15f, -0.501f), windowColor));

        indices.push_back(windowBaseIndex);
        indices.push_back(windowBaseIndex + 1);
        indices.push_back(windowBaseIndex + 2);
        indices.push_back(windowBaseIndex + 1);
        indices.push_back(windowBaseIndex + 3);
        indices.push_back(windowBaseIndex + 2);
    }

    Mesh* building = new Mesh(name);
    building->InitFromData(vertices, indices);
    return building;
}

// Creare cilindru (folosit pentru markeri pentru locul pachetului si locul de livrare)
Mesh* Tema2::CreateCylinder(const std::string& name, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Dimensiuni cilindru
    int segments = 20;
    float height = 1.0f;
    float radius = 1.5f;

    // Culoare cu transparență
    glm::vec4 colorWithAlpha(color.r, color.g, color.b, 0.03f);

    // Partea de jos a cilindrului
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.push_back(VertexFormat(glm::vec3(x, 0, z), colorWithAlpha));
        vertices.push_back(VertexFormat(glm::vec3(x, height, z), colorWithAlpha));
    }

    // Sageata deasupra cilindrului
    float arrowHeight = height + 1.5f;  // Deasupra cilindrului
    float triangleHeight = 0.8f;
    float triangleWidth = 1.0f;
    float rectHeight = 0.4f;
    float rectWidth = 0.3f;
    glm::vec4 arrowColor(1.0f, 1.0f, 1.0f, 0.8f);

    // Vertices pentru sageata
    // Dreptunghi
    vertices.push_back(VertexFormat(glm::vec3(-rectWidth / 2, arrowHeight, 0), arrowColor));          // 0
    vertices.push_back(VertexFormat(glm::vec3(rectWidth / 2, arrowHeight, 0), arrowColor));           // 1
    vertices.push_back(VertexFormat(glm::vec3(-rectWidth / 2, arrowHeight - rectHeight, 0), arrowColor)); // 2
    vertices.push_back(VertexFormat(glm::vec3(rectWidth / 2, arrowHeight - rectHeight, 0), arrowColor));  // 3

    // Triunghi
    vertices.push_back(VertexFormat(glm::vec3(0, arrowHeight - rectHeight - triangleHeight, 0), arrowColor));        // 4 (tip)
    vertices.push_back(VertexFormat(glm::vec3(-triangleWidth / 2, arrowHeight - rectHeight, 0), arrowColor));    // 5 (left)
    vertices.push_back(VertexFormat(glm::vec3(triangleWidth / 2, arrowHeight - rectHeight, 0), arrowColor));     // 6 (right)

    // Generare indici pentru cilindru
    for (int i = 0; i < segments; i++) {
        int current = i * 2;
        int next = ((i + 1) % segments) * 2;

        indices.push_back(current);
        indices.push_back(next);
        indices.push_back(current + 1);

        indices.push_back(next);
        indices.push_back(next + 1);
        indices.push_back(current + 1);
    }

    // Indici pentru dreptunghi
    int rectBase = segments * 2;
    indices.push_back(rectBase);
    indices.push_back(rectBase + 1);
    indices.push_back(rectBase + 2);
    indices.push_back(rectBase + 1);
    indices.push_back(rectBase + 3);
    indices.push_back(rectBase + 2);

    // Indici pentru triunghi
    int triBase = rectBase + 4;
    indices.push_back(triBase);    // tip
    indices.push_back(triBase + 1); // left
    indices.push_back(triBase + 2); // right

    Mesh* cylinder = new Mesh(name);
    cylinder->SetDrawMode(GL_TRIANGLES);
    cylinder->InitFromData(vertices, indices);
    return cylinder;
}

// Creare sageata de direcție
Mesh* Tema2::CreateDirectionArrow(const std::string& name) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 arrowColor(1.0f, 0.0f, 0.0f);  // Roșu

    // Triunghi isoscel cu vârf ascuțit și bază lată
    vertices.push_back(VertexFormat(glm::vec3(-0.25f, 0, -0.2f), arrowColor));
    vertices.push_back(VertexFormat(glm::vec3(0, 0, 1), arrowColor));
    vertices.push_back(VertexFormat(glm::vec3(0.25f, 0, -0.2f), arrowColor));

    // Indici pentru triunghi
    indices = {
        0, 1, 2
    };

    Mesh* arrow = new Mesh(name);
    arrow->InitFromData(vertices, indices);
    return arrow;
}

// Creare punct de livrare
Mesh* Tema2::CreateDeliveryPoint(const std::string& name) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 circleColor(0.2f, 0.8f, 0.2f);  // Culoarea verde

    // Creare cerc cu 32 de segmente
    int segments = 32;
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        vertices.push_back(VertexFormat(
            glm::vec3(cos(angle), 0.01f, sin(angle)),
            circleColor
        ));
    }

    // Vertex pentru centrul cercului
    vertices.push_back(VertexFormat(glm::vec3(0, 0.01f, 0), circleColor));

    // Indici pentru cerc
    for (int i = 0; i < segments; i++) {
        indices.push_back(segments);  // center
        indices.push_back(i);
        indices.push_back((i + 1) % segments);
    }

    Mesh* circle = new Mesh(name);
    circle->InitFromData(vertices, indices);
    return circle;
}

// Creare nori
Mesh* Tema2::CreateCloud(const std::string& name) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 cloudColor(1.0f, 1.0f, 1.0f);  // Alb

    int segments = 20;
    float radius = 1.7f; // Raza norului

    // Offset-uri pentru fiecare nor pentru a nu se suprapune
    glm::vec3 offsets[] = {
        glm::vec3(0, 0, 0),
        glm::vec3(2.0f, 0.4f, 0),
        glm::vec3(-2.0f, 0.2f, 0),
        glm::vec3(1.5f, -0.2f, 0.8f),
        glm::vec3(-2.5f, 0.3f, -0.8f)
    };

    for (const auto& offset : offsets) {
        int baseIndex = vertices.size();

        // Generăm vertices pentru sferă
        for (int i = 0; i <= segments; i++) {
            float phi = M_PI * i / segments;
            for (int j = 0; j <= segments; j++) {
                float theta = 2.0f * M_PI * j / segments;
                float x = radius * sin(phi) * cos(theta);
                float y = radius * sin(phi) * sin(theta);
                float z = radius * cos(phi);
                vertices.push_back(VertexFormat(glm::vec3(x, y, z) + offset, cloudColor));
            }
        }

        // Generăm indices pentru sferă
        for (int i = 0; i < segments; i++) {
            for (int j = 0; j < segments; j++) {
                int current = baseIndex + i * (segments + 1) + j;
                indices.push_back(current);
                indices.push_back(current + segments + 1);
                indices.push_back(current + 1);

                indices.push_back(current + 1);
                indices.push_back(current + segments + 1);
                indices.push_back(current + segments + 2);
            }
        }
    }

    Mesh* cloud = new Mesh(name);
    cloud->InitFromData(vertices, indices);
    return cloud;
}

// Renderizează un mesh folosind un shader și o matrice de model
void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    if (shader == shaders["TerrainShader"]) {
        // Locația uniformei Time în shader
        int location = glGetUniformLocation(shader->program, "Time");
        if (location != -1) {
            glUniform1f(location, totalTime);
        }
    }

    mesh->Render();
}

// Calculați distanța dintre punctele pozitiei dronei (vizualizata ipotetic ca o sfera) si un obiect
bool Tema2::CheckCollisionSphere(const glm::vec3& objectPosition, const glm::vec3& objectSize, const glm::vec3& sphereCenter, float sphereRadius) {
    // Calculăm cel mai apropiat punct de pe obiect la sferă
    float closestX = glm::clamp(sphereCenter.x, objectPosition.x - objectSize.x / 2.0f, objectPosition.x + objectSize.x / 2.0f);

    float closestY = glm::clamp(sphereCenter.y, objectPosition.y - objectSize.y / 2.0f, objectPosition.y + objectSize.y / 2.0f);

    float closestZ = glm::clamp(sphereCenter.z, objectPosition.z - objectSize.z / 2.0f, objectPosition.z + objectSize.z / 2.0f);

    // Calculăm distanța de la centrul sferei până la cel mai apropiat punct
    float distanceX = sphereCenter.x - closestX;
    float distanceY = sphereCenter.y - closestY;
    float distanceZ = sphereCenter.z - closestZ;

    // Calculăm distanța la pătrat pentru a evita rădăcina pătrată
    float distanceSquared = distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ;

    // Verificăm dacă distanța este mai mică decât raza sferei
    return distanceSquared < (sphereRadius * sphereRadius);
}

// Verifică coliziunea dronei cu obiectele din scenă
bool Tema2::IsColliding(const glm::vec3& dronePosition) {
    float sphereRadius = 0.35f; // Raza sferei de coliziune a dronei

    // Verifică coliziunea cu clădirile
    for (size_t i = 0; i < buildingPositions.size(); i++) {
        // Coordonatele de bază ale clădirii
        glm::vec3 buildingCenter = buildingPositions[i];
        glm::vec3 buildingSize = buildingSizes[i];

        // Verifică dacă drona este în perimetrul proiecției clădirii pe XZ
        bool isInBuildingPerimeter =
            dronePosition.x + sphereRadius >= buildingCenter.x - buildingSize.x / 2.0f &&
            dronePosition.x - sphereRadius <= buildingCenter.x + buildingSize.x / 2.0f &&
            dronePosition.z + sphereRadius >= buildingCenter.z - buildingSize.z / 2.0f &&
            dronePosition.z - sphereRadius <= buildingCenter.z + buildingSize.z / 2.0f;

        if (isInBuildingPerimeter) {
            // Dacă drona este sub înălțimea clădirii, este coliziune
            if (dronePosition.y < buildingCenter.y + buildingSize.y + 0.05f) {
                return true;
            }
        }
        else {
            // Verifică coliziunea folosind sfera pentru laturile clădirii
            if (CheckCollisionSphere(buildingCenter, buildingSize, dronePosition, sphereRadius)) {
                return true;
            }
        }
    }

    // Verifică coliziunea cu copacii
    // Verifică coliziunea cu copacii
    for (size_t i = 0; i < treePositions.size(); i++) {
        // Dimensiunile trunchiului
        glm::vec3 trunkSize(
            0.2f * treeSizes[i],  // lățime
            0.5f * treeSizes[i],  // înălțime
            0.2f * treeSizes[i]   // adâncime
        );

        // Verifică coliziunea cu trunchiul
        if (CheckCollisionSphere(treePositions[i], trunkSize, dronePosition, sphereRadius)) {
            return true;
        }

        // Parametrii pentru conul de jos
        float bottomConeHeight = 0.8f * treeSizes[i];
        float bottomConeRadius = 0.5f * treeSizes[i];
        float bottomConeBase = 0.5f * treeSizes[i];  // înălțimea de la care începe conul

        // Parametrii pentru conul de sus
        float topConeHeight = 0.6f * treeSizes[i];
        float topConeRadius = 0.3f * treeSizes[i];
        float topConeBase = bottomConeBase + bottomConeHeight * 0.7f;

        // Verifică coliziunea cu conurile
        // Pentru conul de jos
        glm::vec3 relativePos = dronePosition - treePositions[i];
        // Înălțimea în con
        float heightInCone = relativePos.y - bottomConeBase;

        // Verifică dacă drona este în interiorul conului
        if (heightInCone >= 0 && heightInCone <= bottomConeHeight) {
            // Calculează raza conului la înălțimea curentă
            float radiusAtHeight = bottomConeRadius * (1 - heightInCone / bottomConeHeight);
            float distanceXZ = sqrt(relativePos.x * relativePos.x + relativePos.z * relativePos.z);
            if (distanceXZ < (radiusAtHeight + sphereRadius)) {
                return true;
            }
        }

        // Pentru conul de sus
        heightInCone = relativePos.y - topConeBase;
        if (heightInCone >= 0 && heightInCone <= topConeHeight) {
            // Calculează raza conului la înălțimea curentă
            float radiusAtHeight = topConeRadius * (1 - heightInCone / topConeHeight);
            float distanceXZ = sqrt(relativePos.x * relativePos.x + relativePos.z * relativePos.z);
            if (distanceXZ < (radiusAtHeight + sphereRadius)) {
                return true;
            }
        }
    }

    return false;
}

// Aparitie a unui nou pachet random
void Tema2::SpawnNewPackage() {
    // Generează o poziție random pentru pachet
    bool validPosition = false;

    // Generează poziție până când este validă
    while (!validPosition) {
        // Generează poziție random în intervalul [-10, 10]
        float x = (rand() % 20) - 10;
        float z = (rand() % 20) - 10;
        glm::vec3 pos(x, 0.5f, z);

        // Verifică dacă poziția este liberă 
        if (!IsColliding(pos)) {
            currentPackage.position = pos;
            currentPackage.isCollected = false;
            currentPackage.size = 0.5f;
            validPosition = true;
        }
    }
}

// Apariție a unui nou punct de livrare
void Tema2::SpawnNewDeliveryPoint() {
    // Generează o poziție random pentru punctul de livrare
    bool validPosition = false;

    // Generează poziție până când este validă
    while (!validPosition) {
        float x = (rand() % 20) - 10;
        float z = (rand() % 20) - 10;
        glm::vec3 pos(x, 0.01f, z);

        // Verifică dacă poziția este liberă și suficient de departe de pachet
        if (!IsColliding(pos) && glm::distance(pos, currentPackage.position) > 5.0f) {
            deliveryPoint.position = pos;
            deliveryPoint.radius = 2.0f;
            validPosition = true;
        }
    }
}

// Actualizează săgeata de direcție pentru a arăta către pachet sau punct de livrare
void Tema2::UpdateDirectionArrow() {

    // Obține poziția țintei (fie pachet, fie loc de livrare)
    glm::vec3 targetPosition;
    if (packageAttached) {
        targetPosition = deliveryPoint.position;
    }
    else {
        targetPosition = currentPackage.position;
    }

    // Poziția dronei
    glm::vec3 dronePosition = glm::vec3(translateX, translateY, translateZ);

    // Calculează vectorul direcțional către țintă în plan XZ
    glm::vec2 directionToTarget = glm::vec2(
        targetPosition.x - dronePosition.x,
        targetPosition.z - dronePosition.z
    );

    // Normalizăm vectorul
    directionToTarget = glm::normalize(directionToTarget);

    // Calculăm unghiul în radiani folosind atan2
    arrowRotation = atan2(directionToTarget.y, directionToTarget.x) + 3 * M_PI / 2;
}

// Verifică daca putem prelua un pachet
bool Tema2::CheckPackageCollision() {
    // Dacă deja avem un pachet, nu putem prelua altul
    if (packageAttached)
        return false;

    // Distanta dintre drona si pachet
    float distance = glm::distance(
        glm::vec2(translateX, translateZ),
        glm::vec2(currentPackage.position.x, currentPackage.position.z)
    );

    return distance < (currentPackage.size + 0.5f);
}

// Verifică dacă am ajuns la punctul de livrare
bool Tema2::CheckDeliveryPointReached() {
    // Dacă nu avem un pachet, nu putem livra
    if (!packageAttached)
        return false;

    // Distanta dintre drona si punctul de livrare
    float distance = glm::distance(
        glm::vec2(translateX, translateZ),
        glm::vec2(deliveryPoint.position.x, deliveryPoint.position.z)
    );

    return distance < deliveryPoint.radius;
}

// Actualizează logica și randarea scenei pentru fiecare frame
void Tema2::Update(float deltaTimeSeconds) {
    totalTime += deltaTimeSeconds; // Actualizează timpul total de rulare

    
    // Actualizare poziție cameră folosind angularStepOY (rotația dronei)
    glm::vec3 dronePosition(translateX, translateY, translateZ);
    camera->UpdatePosition(dronePosition, angularStepOY);

    // Setează parametrii de randare
    glLineWidth(3);
    glPointSize(5);

    // Actualizează rotația elicei (animație continuă)
    propellerRotation += deltaTimeSeconds * 2.0f;

    // Renderizare scenă principală
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    Render(); // Apelul principal pentru desenarea scenei

    // Randăm norii
    for (const auto& cloud : clouds) {
        glm::mat4 cloudMatrix = glm::mat4(1);
        cloudMatrix *= transform3D::Translate(cloud.position.x, cloud.position.y, cloud.position.z);
        RenderMesh(meshes["cloud"], shaders["VertexColor"], cloudMatrix);
    }


	// Actualizarea sagetii de directie
    UpdateDirectionArrow();

    // Verifică coliziunea cu pachetul
    if (CheckPackageCollision()) {
        currentPackage.isCollected = true; // Marchează pachetul ca ridicat
        packageAttached = true; // Atașează pachetul
        SpawnNewDeliveryPoint(); // Generează un nou punct de livrare
        std::cout << "You picked up the package!" << std::endl;
    }

    // Verifică dacă pachetul a fost livrat
    if (CheckDeliveryPointReached()) {
        score++; // Crește scorul
        packageAttached = false; // Detasează pachetul
        SpawnNewPackage(); // Generează un nou pachet
        std::cout << "Package delivered! Score: " << score << std::endl;
    }

    // Actualizăm poziția norilor
    for (auto& cloud : clouds) {
        cloud.position.x += cloud.speed * deltaTimeSeconds;
        if (cloud.position.x > 20) {
            cloud.position.x = -20; // Reîntoarce norii la marginea scenei
        }
    }


    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);
    

    // Cameră ortografică pentru minimap
    camera->UpdatePositionForTopView(dronePosition);

    // Proiecție ortografică pentru minimap
    float zoom = 30.0f; // Ajustează zoom-ul pentru a include toate obiectele
    glm::mat4 minimapProjection = glm::ortho(-zoom, zoom, -zoom, zoom, 0.1f, 100.0f);

    glm::mat4 minimapView = glm::lookAt(
        camera->position,                  // Poziția camerei minimap-ului
        camera->position + camera->forward, // Punctul spre care privește
        camera->up                         // Vectorul „up” al camerei
    );

    // Setează grosimea liniilor pentru elementele minimapei
    glLineWidth(3.0f);

    Render();

    // Rendează elementele cu proiecția ortografică
    glm::mat4 terrainMatrix = glm::mat4(1);
    terrainMatrix *= transform3D::Scale(4.0f, 1.0f, 4.0f);
    RenderMesh(meshes["terrain"], shaders["VertexColor"], terrainMatrix);


	// Daca pachetul nu este ridicat, afiseaza markerul pentru el
    if (!currentPackage.isCollected) {
        glm::mat4 packageMarker = glm::mat4(1);
        packageMarker *= transform3D::Translate(currentPackage.position.x, 0.1f, currentPackage.position.z);
        packageMarker *= transform3D::Scale(1.0f, 1.0f, 1.0f);
        RenderMesh(meshes["pickup-marker"], shaders["VertexColor"], packageMarker);
    }

	// Daca pachetul este atasat, afiseaza punctul de livrare
    if (packageAttached) {
        glm::mat4 deliveryMarker = glm::mat4(1);
        deliveryMarker *= transform3D::Translate(deliveryPoint.position.x, 0.1f, deliveryPoint.position.z);
        deliveryMarker *= transform3D::Scale(1.2f, 1.0f, 1.2f);
        RenderMesh(meshes["delivery-marker"], shaders["VertexColor"], deliveryMarker);
    }

}

void Tema2::FrameEnd()
{
}

void Tema2::OnInputUpdate(float deltaTime, int mods) {
    static bool wasMovingBackwards = false;  // Pentru a ține minte ultima stare
	bool isMovingBackwards = window->KeyHold(GLFW_KEY_S); // Verifică dacă se mișcă înapoi

    // Dacă s-a schimbat starea (de la înainte la înapoi sau invers)
    if (isMovingBackwards != wasMovingBackwards) {
        if (window->KeyHold(GLFW_KEY_W) || window->KeyHold(GLFW_KEY_S)) {
			lateralDirection = -1.0f;  // Inversăm direcția lui A si D
        }
		arrowRotation *= -1.0f;  // Inversăm rotația sagetii să fie în direcția corectă
    }
    wasMovingBackwards = isMovingBackwards;

	// Calculăm direcția de mers în funcție de unghiul de rotație pe OY
    glm::vec3 forward = glm::normalize(glm::vec3(
        sin(angularStepOY),
        0,
        -cos(angularStepOY)
    ));

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

    float speed = 2.0f * deltaTime;
    glm::vec3 proposedPosition = glm::vec3(translateX, translateY, translateZ);

    // Setăm starea de mers înapoi pentru cameră
    camera->SetMovingBackwards(isMovingBackwards, window->KeyHold(GLFW_KEY_W));

    if (window->KeyHold(GLFW_KEY_W)) {
        proposedPosition += forward * speed;
		if (lateralDirection < 0.0f) {
			lateralDirection *= -1.0f;  // Inversăm direcția lui A si D
		}
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        proposedPosition -= forward * speed;
        if (lateralDirection > 0.0f) {
			lateralDirection *= -1.0f;  // Inversăm direcția lui A si D
        }
    }
    if (window->KeyHold(GLFW_KEY_A)) {
		// Daca se misca inapoi, inversam directia (in oglinda) in care se misca daca apasam A
        if (lateralDirection > 0.0f) {
			proposedPosition -= right * speed;
		}
		else {
			proposedPosition += right * speed;
		}
    }
    if (window->KeyHold(GLFW_KEY_D)) {
		// Daca se misca inapoi, inversam directia (in oglinda) in care se misca daca apasam D
        if (lateralDirection > 0.0f) {
			proposedPosition += right * speed;
		}
		else {
			proposedPosition -= right * speed;
		}
    }
    if (window->KeyHold(GLFW_KEY_E)) {
        proposedPosition.y += speed;
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
        proposedPosition.y -= speed;
    }

	// Ne asigurăm că drona nu merge sub nivelul solului
    if (proposedPosition.y < 0.5f) {
        proposedPosition.y = 0.5f;
    }

    // Verifică coliziunea cu obiecte din scenă
    if (!IsColliding(proposedPosition)) {
        translateX = proposedPosition.x;
        translateY = proposedPosition.y;
        translateZ = proposedPosition.z;
    } else {
        // Dacă există coliziune, aplică logica de "bounceback"
        glm::vec3 direction = glm::normalize(glm::vec3(translateX, translateY, translateZ) - proposedPosition);
        translateX += direction.x * 0.5f; // Retrage drona pe axa X
        translateY += direction.y * 0.5f; // Retrage drona pe axa Y
        translateZ += direction.z * 0.5f; // Retrage drona pe axa Z
    }

}

void Tema2::OnKeyPress(int key, int mods)
{
    // Schimbă modul de desenare (GL_FILL, GL_LINE, GL_POINT) cu tasta SPACE
    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }

    // Controlează mișcarea viewport-ului minimap
    if (key == GLFW_KEY_I)
    {
        miniViewportArea.y += 10;
    }
    if (key == GLFW_KEY_J)
    {
        miniViewportArea.x -= 10;
    }
    if (key == GLFW_KEY_K)
    {
        miniViewportArea.y -= 10;
    }
    if (key == GLFW_KEY_L)
    {
        miniViewportArea.x += 10;
    }
    if (key == GLFW_KEY_U)
    {
        miniViewportArea.width += 10;
        miniViewportArea.height += 10;
    }
    if (key == GLFW_KEY_O)
    {
        miniViewportArea.width -= 10;
        miniViewportArea.height -= 10;
    }

    if (key == GLFW_KEY_P) {
        // Comută între vedere first-person și third-person
        firstPersonView = !firstPersonView;
        camera->SetFirstPerson(firstPersonView);
    }
}

void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}

// Actualizează rotația dronei pe OY
void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    angularStepOY += deltaX * mouseSensitivity; // Schimbă unghiul de rotație pe OY
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}