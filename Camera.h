#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <vector>



// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    ROLLL,
    ROLLR
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.001f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::quat Rotation; 
    glm::mat4x4 Transform; 
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    float Roll;
    // camera options
    float MovementSpeed;
    float RollSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        RollSpeed = MovementSpeed/2;
        Right = glm::vec3(1, 0, 0);
        Up = glm::vec3(0, 1, 0);
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        RollSpeed = MovementSpeed/2;
        Right = glm::vec3(1, 0, 0);
        Up = glm::vec3(0, 1, 0);
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        glm::mat4x4 model(1.0f);
        model = glm::translate(model, Position);
        model *= glm::mat4_cast(-Rotation);
        //model = glm:: translate(model, Position);
        /*model *= glm::inverse(glm::mat4_cast(Rotation));*/

        return Transform;
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == ROLLL)
        {
            setRoll(RollSpeed * deltaTime);

        }
        if (direction == ROLLR)
        {

            setRoll(-RollSpeed * deltaTime);
        }
        if (Position.y < 0.5)
            Position.y = 0.5;
        updateCameraVectors();
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        if (glm::abs(xoffset) > 0.25)
            xoffset *= MouseSensitivity;
        else
            xoffset = 0;
        if (glm::abs(yoffset) > 0.25)
            yoffset *= MouseSensitivity;
        else
            yoffset = 0;
        //Yaw works at 0 and 180 degrees, is flipped at 90 and 270, is wrong everywhere else
        setYaw(-xoffset);// *glm::cos(glm::radians(Roll)) + yoffset * (1 - glm::cos(glm::radians(Roll)));
        //Pitch appears to be working correctly
        setPitch(yoffset);// *glm::cos(glm::radians(Roll)) + xoffset * (1 - glm::cos(glm::radians(Roll)));


        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }
    void setRoll(float amount)
    {
        glm::quat rotation = glm::angleAxis(amount, Front);
        Right = glm::normalize(rotation * Right);
        Up = glm::normalize(rotation * Up);
        updateViewMatrix();
    }

    void setYaw(float amount)
    {
        glm::quat rotation = glm::angleAxis(amount, Up);
        Front = glm::normalize(rotation * Front);
        Right = glm::normalize(rotation * Right);
    }

    void setPitch(float amount)
    {
        glm::quat rotation = glm::angleAxis(amount, Right);
        Front = glm::normalize(rotation * Front);
        //Up = glm::normalize(rotation * Up);
        if (glm::dot(Up, glm::cross(Right, Front)) < 0)
        {
            Up *= -1;
        }
        updateViewMatrix();
    }




    void updateViewMatrix()
    {
        Transform = glm::lookAt(Position, Position + Front, Up);
        //Transform = glm::lookAt(Position, Front, Up);
    }


    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        Transform = glm::lookAt(Position, Position + Front, Up);
    }
};
#endif