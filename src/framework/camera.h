#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 4.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 60.0f;

// Custom camera handler class
class Camera {
public:
	// Camera attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	// Eular angles
	float yaw;
	float pitch;

	// Camera options
	float movementSpeed;
	float mouseSensitivity;
	float zoom; // Should i change this to FOV? Makes more sense

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, 0.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		//updateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		//updateCameraVectors();
	}

	// Returns the view matrix calculated using Eular angles and the LookAt matrix
	glm::mat4 getViewMatrix() {
		return glm::lookAt(position, position + front, up);
	}

	// Processes input received from any keyboard-like input system.
	// Accepts input parameter in the form of camera defined ENUM
	void processKeyboard(Camera_Movement direction, float delta) {
		float velocity = movementSpeed * delta;
		if(direction == FORWARD)
			position += front * velocity;
		if(direction == BACKWARD)
			position -= front * velocity;
		if(direction == LEFT)
			position -= right * velocity;
		if(direction == RIGHT)
			position += right * velocity;
		if(direction == UP)
			//position += up * velocity;
			position.y += velocity;
		if(direction == DOWN)
			//position -= up * velocity;
			position.y -= velocity;
	}

	// Processes input received from a mouse input system. Excepts the offset value in both the x and y direction.
	void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true) {
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;

		yaw += xOffset;
		pitch += yOffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if(constrainPitch) {
			if(pitch > 89.0f)
				pitch = 89.0f;
			if(pitch < -89.0f)
				pitch = -89.0f;
		}

		// Update front, right, and up vectors using the updated Euler angles
		updateCameraVectors();
	}

	//Processes input received from a mouse scroll-wheel event. ONly requires input on vertical wheel axis
	void processMouseScroll(float yOffset) {
		if(zoom >= 1.0f && zoom <= 45.0f)
			zoom -= yOffset;
		if(zoom <= 1.0f)
			zoom = 1.0f;
		if(zoom >= 45.0f)
			zoom = 45.0f;
	}
private:
	// Calculates the front vector from the camera's (updated) euler angles
	void updateCameraVectors() {
		// Calculates the new front vector
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		this->front = glm::normalize(front);

		// Also re-calculate the right and up vector
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(right, this->front));
	}
};

#endif