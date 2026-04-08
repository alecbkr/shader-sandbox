#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class AppTimer;
class InputState;

enum Camera_Movement {
    CAM_FORWARD,
    CAM_BACKWARD,
    CAM_RIGHT,
    CAM_LEFT
};

class Camera {
    public:
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        float Yaw;
        float Pitch;

        bool moveFast = false;
        float MovementSpeed;
        float CameraSensitivity;
        float Zoom;

        Camera(AppTimer* _timerPtr, InputState* _inputPtr);
        glm::mat4 GetViewMatrix();
        void ProcessKeyboard(Camera_Movement dir, float deltaTime);
        void ProcessMouseMovement();
        void ProcessMouseScroll(float yoffset);
        void MoveForward();
        void MoveBack();
        void MoveLeft();
        void MoveRight();
        void MoveUp();
        void MoveDown();
        void MoveFast();
        void reset();
    
    private:
        AppTimer* timerPtr = nullptr;
        InputState* inputPtr = nullptr;
        void updateCameraVectors();
};

#endif