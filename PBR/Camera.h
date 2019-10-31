#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Camera {
	glm::mat4 c_View;
	glm::vec3 c_Pos, c_Target, c_Dir, c_Up, c_Right, up, c_Front;

public:
	Camera();
	void Update();
	void UpdatePos(glm::vec3 po);
	void ResetCamPosition();
	void UpdateLookAt(glm::vec3 la);
	glm::vec3 GetPos() { return c_Pos; }
	glm::mat4 GetView() { return c_View; }
};