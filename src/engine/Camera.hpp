#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

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

        float MovementSpeed;
        float CameraSensitivity;
        float Zoom;

        Camera();
        glm::mat4 GetViewMatrix();
        void ProcessKeyboard(Camera_Movement dir, float deltaTime);
        void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch);
        void ProcessMouseScroll(float yoffset);
        void MoveForward();
        void MoveBack();
        void MoveLeft();
        void MoveRight();
        void MoveUp();
        void MoveDown();
    
    private:
        void updateCameraVectors();
};

#endif