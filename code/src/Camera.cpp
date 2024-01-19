#include <Camera.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 5.0f, -1.0f)), MovementSpeed(SPEED),
          MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 5.0f, -1.0f)), MovementSpeed(SPEED),
          MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

// Processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::ProcessKeyboard(Camera_Mode mode, Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;

    switch (mode) {
        case HUMAN_WALKING:
            switch (direction) {
                case FORWARD:
                    Position += glm::vec3(glm::cos(glm::radians(Yaw)), 5, glm::sin(glm::radians(Yaw))) *
                                velocity; //Y is not affected, Y is looking up
                    break;
                case BACKWARD:
                    Position -= glm::vec3(glm::cos(glm::radians(Yaw)), 5, glm::sin(glm::radians(Yaw))) *
                                velocity; //Y is not affected, Y is looking up
                    break;
                case LEFT:
                    Position -= Right * velocity;
                    break;
                case RIGHT:
                    Position += Right * velocity;
                    break;
            }
            break;
        case FLYING:
            switch (direction) {
                case FORWARD:
                    Position += Front * velocity;
                    break;
                case BACKWARD:
                    Position -= Front * velocity;
                    break;
                case LEFT:
                    Position -= Right * velocity;
                    break;
                case RIGHT:
                    Position += Right * velocity;
                    break;
            }
            break;
    }
    updateCameraVectors();
}

// Processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                                  GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires
// input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
    if (Zoom >= 1.0f && Zoom <= 45.0f)
        Zoom -= yoffset;
    if (Zoom <= 1.0f)
        Zoom = 1.0f;
    if (Zoom >= 45.0f)
        Zoom = 45.0f;
}

void Camera::ToggleFlightMode() {
    mode = (mode == HUMAN_WALKING) ? FLYING : HUMAN_WALKING;
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = static_cast<float>(cos(glm::radians(static_cast<double>(Yaw))) *
                                 cos(glm::radians(static_cast<double>(Pitch))));
    front.y = static_cast<float>(sin(glm::radians(static_cast<double>(Pitch))));
    front.z = static_cast<float>(sin(glm::radians(static_cast<double>(Yaw))) *
                                 cos(glm::radians(static_cast<double>(Pitch))));
    Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(
            Front, WorldUp)); // Normalize the vectors, because their length gets
    // closer to 0 the more you look up or down which
    // results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}
