#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include <vector>

class Light {
	glm::vec3 l_Pos, l_Color;
public:
	Light();
	glm::vec3 GetLightPos() { return l_Pos; }
	void SetLightPos(glm::vec3 p);
	glm::vec3 GetLightColor() { return l_Color; }
};