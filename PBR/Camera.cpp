#include "Camera.h"


Camera::Camera() {
	c_Pos		= glm::vec3(0.0f, 0.0f, 10.0f);
	c_Target	= glm::vec3(0.0f, 0.0f, 0.0f);
	c_Dir		= glm::normalize(c_Pos - c_Target);
	up			= glm::vec3(0.0f, 1.0f, 0.0f);
	c_Right		= glm::normalize(glm::cross(up, c_Dir));
	c_Up		= glm::cross(c_Dir, c_Right);
	c_Front		= glm::vec3(0.0f, 0.0f, -1.0f);
	c_View		= glm::mat4(1.0f);
	c_View		= glm::lookAt(c_Pos, c_Pos + c_Front, c_Up);
}


void Camera::UpdatePos(glm::vec3 po) {
	c_Pos = po;
	Update();
	//std::cout << po.x << " " << po.y << " " << po.z << std::endl;
}

void Camera::UpdateLookAt(glm::vec3 la) {
	c_Target = la;
	Update();
	//std::cout << la.x << " " << la.y << " " << la.z << std::endl;
}

void Camera::ResetCamPosition() {
	c_Pos = glm::vec3(0.0f, 0.0f, 10.0f);
	c_Target = glm::vec3(0.0f, 0.0f, 0.0f); 
	Update(); 
}

void Camera::Update() {
	c_View = glm::lookAt(c_Pos, c_Target, c_Up);
}