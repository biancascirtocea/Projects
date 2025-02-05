#pragma once
#include "utils/glm_utils.h"

namespace implemented {
    class DroneCamera {
    public:
        DroneCamera() {
            position = glm::vec3(0, 2, 5);
            forward = glm::vec3(0, 0, -1);
            up = glm::vec3(0, 1, 0);
            right = glm::vec3(1, 0, 0);
			distanceToTarget = 1.5f;  // Adăugat pentru a ține cont de distanța față de dronă
			firstPerson = true;       // Adăugat pentru a ține cont de perspectiva camerei
			currentRotation = 0.0f;   // Adăugat pentru a ține cont de rotație
			movingBackwards = false;  // Adăugat pentru a ține cont de mișcarea înapoi
            lastMovingState = false;  // Adăugat pentru a menține ultima stare
        }

		// Setează dacă camera este în first person sau third person
        void SetFirstPerson(bool isFirstPerson) {
            firstPerson = isFirstPerson;
        }

		// Setează dacă camera se mișcă înapoi sau înainte
        void SetMovingBackwards(bool isMovingBackwards, bool isMovingForward) {
            // Actualizăm movingBackwards doar când mergem înapoi sau când începem să mergem înainte
            if (isMovingBackwards) {
                movingBackwards = true;
                lastMovingState = true;
            }
            else if (isMovingForward) {
                movingBackwards = false;
                lastMovingState = false;
            }
            else {
                // Când nu ne mișcăm, păstrăm ultima stare
                movingBackwards = lastMovingState;
            }
        }

		// Actualizează poziția camerei pentru vizualizarea de sus
        void UpdatePositionForTopView(const glm::vec3& centerPosition) {
            // Plasează camera deasupra centrului scenei
            position = centerPosition + glm::vec3(0, 9, 0); // Înălțimea deasupra dronei

            // Camera privește direct în jos
            forward = glm::vec3(0, -1, 0);
            right = glm::vec3(1, 0, 0); // Paralel cu axa X
            up = glm::vec3(0, 0, -1);   // Paralel cu axa Z
        }

		// Actualizează poziția camerei pentru vizualizarea din față
        void UpdatePosition(const glm::vec3& dronePosition, float droneRotationY) {
            if (firstPerson) {
				// Camera este în spatele dronei
                position = dronePosition + glm::vec3(0, 0.15f, 0);

                float rotationDiff = droneRotationY - currentRotation;
                currentRotation += rotationDiff * 0.1f;

                float actualRotation = currentRotation;

				// Dacă dronele se mișcă înapoi, camera trebuie să se rotească cu 180 de grade
                if (movingBackwards) {
                    actualRotation += glm::pi<float>();
                }

                forward = glm::vec3(
                    sin(actualRotation),
                    0,
                    -cos(actualRotation)
                );
            }
            else {
                float smoothedRotation = currentRotation + (droneRotationY - currentRotation) * 0.1f;
                currentRotation = smoothedRotation;

                float actualRotation = smoothedRotation;
				// Dacă dronele se mișcă înapoi, camera trebuie să se rotească cu 180 de grade
                if (movingBackwards) {
                    actualRotation += glm::pi<float>();
                }

                forward = glm::vec3(
                    sin(actualRotation),
                    -0.2f,
                    -cos(actualRotation)
                );

                position = dronePosition - forward * distanceToTarget * 1.0f + glm::vec3(0, 0.6f, 0);
            }

            right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
            up = glm::normalize(glm::cross(right, forward));
        }

		// Returnează matricea de vizualizare
        glm::mat4 GetViewMatrix() {
            return glm::lookAt(position, position + forward, up);
        }

    public:
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
        float distanceToTarget;
        bool firstPerson;

    private:
        float currentRotation;
        bool movingBackwards;
        bool lastMovingState;  // Pentru a păstra ultima stare de mișcare
    };
}