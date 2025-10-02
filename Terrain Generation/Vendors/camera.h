#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// camera values
const float YAW{ -90.0f };
const float PITCH{ 0.0f };
const float SPEED{ 20.0f };
const float SENSITIVITY{ 0.1f };
const float FOV = 45.0f;

class Camera
{
public:
	// camera attributes
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 Right;
	glm::vec3 Worldup; //the initial up vector

	// euler angles
	float Yaw = YAW;
	float Pitch = PITCH;

	// camera options
	float movement_speed = SPEED;
	float mouse_sensitivity = SENSITIVITY;
	float zoom = FOV;

	// constructor with vectors
	Camera(glm::vec3 position, glm::vec3 up, glm::vec3 front, float yaw, float pitch)
	{
		Position = position;
		Worldup = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// constructor with scalars
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	{
		Position = glm::vec3(posX, posY, posZ);
		Worldup = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;

		updateCameraVectors();
	}

	// return the view matrix
	glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// process input from keyboard, accepting inputs as enums
	void processKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = deltaTime * movement_speed;

		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	// process mouse movement inputs
	void processMouseMovement(float xoffset, float yoffset, GLboolean constraintPitch = true)
	{
		xoffset *= mouse_sensitivity;
		yoffset *= mouse_sensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that if the pitch is out of bounds, the screen doesnt get flipped
		if (constraintPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// update the front, right, up vectors using new euler angles
		updateCameraVectors();
	}

	void processMouseScroll(float yoffset)
	{
		zoom -= (float)yoffset;
		if (zoom < 1.0f)
			zoom = 1.0f;
		if (zoom > 45.0f)
			zoom = 45.0f;
	}

private:

	//calculates front, right and up vectors
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
		front.y = sin(glm::radians(Pitch));
		front.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
		Front = glm::normalize(front);

		Right = glm::normalize(glm::cross(Front, Worldup));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

#endif