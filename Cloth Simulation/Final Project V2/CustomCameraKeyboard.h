#pragma once

#ifndef CUSTOM_CAMERA_KEYBOARD_H
#define CUSTOM_CAMERA_KEYBOARD_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;

class CameraKeyBoard {
	public:
		//Camera Attribs
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;

		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		
		// Euler Angles
		float Yaw;
		float Pitch;

		// Camera options
		float MovementSpeed;
		float MouseSensitivity;

		//Constructor
		CameraKeyBoard(glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f), //Initial Position for camera if not declared
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = YAW,
			float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, 1.0f)),
			MovementSpeed(SPEED),
			MouseSensitivity(SENSITIVITY) {

			Position = position;
			WorldUp = up;
			Yaw = yaw;
			Pitch = pitch;
			updateCameraVectors();
		}

		//Returns View Matrix
		glm::mat4 GetViewMatrix() {
			return glm::lookAt(this->Position,this->Position + this->Front, this->Up);
		}

		//Keyboard Movement
		void processKeyboardEntry(Camera_Movement direction, float deltaTime) {
			float velocity = MovementSpeed * deltaTime;
			if (direction == FORWARD)
				Position += Front * velocity;
			if (direction == BACKWARD)
				Position -= Front * velocity;
			if (direction == LEFT)
				Position -= Right * velocity;
			if (direction == RIGHT)
				Position += Right * velocity;
		}

		//Mouse Movement
		void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true){
			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (Pitch > 89.0f)
					Pitch = 89.0f;
				if (Pitch < -89.0f)
					Pitch = -89.0f;
			}

			// Update Front, Right and Up Vectors
			updateCameraVectors();
		}

		glm::mat4 getViewMatrix() {
			return viewMatrix;
		}

		glm::mat4 getProjectionMatrix() {
			return projectionMatrix;
		}
	private:
		// Calculates the front vector from the Camera's (updated) Euler Angles
		void updateCameraVectors()
		{
			// Calculate the new Front vector
			glm::vec3 front;
			front.x = cos(glm::radians(Yaw))* cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch));
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(front);
			// Also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
};

#endif
