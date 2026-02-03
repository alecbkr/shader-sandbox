#include "Camera.hpp"

#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "engine/AppTimer.hpp"

// DEFAULT CAMERA VALUES
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// CAMERA MOVEMENT DEFINITIONS

// Camera( float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
//  MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)

Camera::Camera() {


    Position = glm::vec3(0.0f, 2.0f, 2.0f);
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Yaw = YAW;
    Pitch = PITCH;

    MovementSpeed = SPEED;
    CameraSensitivity = SENSITIVITY;
    Zoom = ZOOM;

    updateCameraVectors();
}



glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

// void Camera::MoveForward() {
//     float velocity = MovementSpeed * AppTimer::getDt();
//     Position += Front * velocity;
// }
// void Camera::MoveBack() {
//     float velocity = MovementSpeed * AppTimer::getDt();
//     Position -= Front * velocity;
// }
// void Camera::MoveLeft() {
//     float velocity = MovementSpeed * AppTimer::getDt();
//     Position -= Right * velocity;
// }
// void Camera::MoveRight() {
//     float velocity = MovementSpeed * AppTimer::getDt();
//     Position += Right * velocity;
// }
// void Camera::MoveUp() {
//     float velocity = MovementSpeed * AppTimer::getDt();
//     Position += WorldUp * velocity;
// }
// void Camera::MoveDown() {
//     float velocity = MovementSpeed * AppTimer::getDt();
//     Position -= WorldUp * velocity;
// }

void Camera::ProcessKeyboard(Camera_Movement dir, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (dir == CAM_FORWARD)
        Position += Front * velocity;
    if (dir == CAM_BACKWARD)
        Position -= Front * velocity;
    if (dir == CAM_LEFT)
        Position -= Right * velocity;
    if (dir == CAM_RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= CameraSensitivity;
    yoffset *= CameraSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}

