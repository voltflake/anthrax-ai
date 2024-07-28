 #include "anthraxAI/camera.h"

 void Camera::setdirections()
 { 
    Direction = glm::normalize(Position - Target); 
    
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    Right = glm::normalize(glm::cross(WorldUp, Direction)); 
    Up = glm::cross(Direction, Right);
}

#ifdef AAI_LINUX
void Camera::checkmovement(xcb_keysym_t k, float delta)
{
    const float cameraSpeed = 15.0f * delta;	
    if (k == W_KEY) {
        Position += cameraSpeed * Front;
    }
    if (k == S_KEY) {
        Position -= cameraSpeed * Front;
    }
    if (k == A_KEY) {
        Position -= glm::normalize(glm::cross(Front, Up)) * cameraSpeed;
    }
    if (k == D_KEY) {
        Position += glm::normalize(glm::cross(Front, Up)) * cameraSpeed;
    }
}
#else
void Camera::checkmovement(float delta)
{
    const float cameraSpeed = 1.0f * delta;
	if (GetAsyncKeyState(W_KEY) < 0) {
        Position += cameraSpeed * Front;
    }
	if (GetAsyncKeyState(S_KEY) < 0) {
        Position -= cameraSpeed * Front;
    }
	if (GetAsyncKeyState(A_KEY) < 0) {
        Position -= glm::normalize(glm::cross(Front, Up)) * cameraSpeed;
    }
	if (GetAsyncKeyState(D_KEY) < 0) {
        Position += glm::normalize(glm::cross(Front, Up)) * cameraSpeed;
    }
}
#endif
void Camera::checkdirection(Positions mousepos)
{
    Positions mousemove = mousepos;

    float rotateSpeed = 0.0007f;

    if (mousemove.x || mousemove.y) {
        float yaw = rotateSpeed * mousemove.x;
        float pitch = rotateSpeed * mousemove.y;

        Rotation = glm::rotate(glm::mat4(1.0), -yaw, glm::vec3(0.f, 1.f, 0.f));
        Rotation = glm::rotate(Rotation, -pitch, Right);
    }


    Direction = glm::normalize(glm::mat3(Rotation) * Direction);
    Up = glm::normalize(glm::mat3(Rotation) * Up);
    Right = glm::normalize(glm::cross(Direction, Up));
    Front = -Direction;
    // static float lastx = mousepos.x;
    // static float lasty = mousepos.y;
  
    // float xoffset = mousepos.x - lastx;
    // float yoffset = lasty - mousepos.y; 
    // lastx = mousepos.x;
    // lasty = mousepos.y;

    // float sensitivity = 0.05f;
    // xoffset *= sensitivity;
    // yoffset *= sensitivity;

    // yaw   += xoffset;
    // pitch += yoffset;

    // if(pitch > 89.0f)
    //     pitch = 89.0f;
    // if(pitch < -89.0f)
    //     pitch = -89.0f;

    // glm::vec3 direction;
    // direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    // direction.y = sin(glm::radians(pitch));
    // direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    // Front = glm::normalize(direction);
    // Right = glm::normalize(glm::cross(Front, WorldUp));
    // Up    = glm::normalize(glm::cross(Right, Front));
}