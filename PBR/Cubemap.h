#pragma once
#include <GLAD/include/glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <iostream>

class Cubemap {
private:
	unsigned int captureFBO, captureRBO, envCubemap, hdrTex, VAO, VBO;
	glm::mat4 c_CapProjection;
	glm::mat4 c_CapViews[6] = {
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	};
public:
	Cubemap();
	void LoadCubemap(const char* path);
	void MakeCubeMap();
	void Render();
	glm::mat4 GetCMCapProj() { return c_CapProjection; }
	glm::mat4 GetCMCapViews(size_t ind){ return c_CapViews[ind]; }
};