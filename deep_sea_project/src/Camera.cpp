#include "Camera.h"

Camera::Camera(
    glm::vec3 position,
    glm::vec3 up,
    float yaw,
    float pitch
)
{
    Position = position;

    WorldUp = up;

    Yaw = yaw;
    Pitch = pitch;

    Front = glm::vec3(0.0f, 1.0f, 0.0f);

    MovementSpeed = 15.0f;
    MouseSensitivity = 0.1f;

    Radius = 0.3f;

    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(
        Position,
        Position + Front,
        Up
    );
}

glm::vec3 Camera::GetNextPosition(
    bool forward,
    bool backward,
    bool left,
    bool right,
    float deltaTime
)
{
    float velocity =
        MovementSpeed * deltaTime;

    glm::vec3 nextPos = Position;

    glm::vec3 flatFront =
        glm::normalize(
            glm::vec3(
                Front.x,
                Front.y,
                0.0f
            )
        );

    glm::vec3 flatRight =
        glm::normalize(
            glm::vec3(
                Right.x,
                Right.y,
                0.0f
            )
        );

    if (forward)
        nextPos += flatFront * velocity;

    if (backward)
        nextPos -= flatFront * velocity;

    if (left)
        nextPos -= flatRight * velocity;

    if (right)
        nextPos += flatRight * velocity;

    return nextPos;
}

void Camera::SetPosition(
    const glm::vec3& pos
)
{
    Position = pos;
}

void Camera::ProcessMouseMovement(
    float xoffset,
    float yoffset
)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (Pitch > 89.0f)
        Pitch = 89.0f;

    if (Pitch < -89.0f)
        Pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;

    front.x =
        cos(glm::radians(Yaw)) *
        cos(glm::radians(Pitch));

    front.y =
        sin(glm::radians(Yaw)) *
        cos(glm::radians(Pitch));

    front.z =
        sin(glm::radians(Pitch));

    Front = glm::normalize(front);

    Right = glm::normalize(
        glm::cross(
            Front,
            WorldUp
        )
    );

    Up = glm::normalize(
        glm::cross(
            Right,
            Front
        )
    );
}