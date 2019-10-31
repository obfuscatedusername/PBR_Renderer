#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include "Shader.h"
#include <vector>

const float PI = 3.141592653589f;

struct mat {
	unsigned int albedo, normal, metallic, roughness, ao;
};


class Sphere {
private:
	unsigned int VAO, VBO, EBO, s_IndexCount;
	std::string s_Name;
	glm::vec3 s_Pos; 
	glm::mat4 Model;
	std::vector<glm::vec3> s_Positions, s_Normals;
	std::vector<glm::vec2> s_UV;
	std::vector<unsigned int> s_Indices;
	std::vector<float> s_Data;
	unsigned int LoadTexture(char const *p);
	bool s_Rotate;
public:
	mat s_Material;
	Sphere(glm::vec3 pos, std::string n);
	glm::vec3 GetSpherePos() { return s_Pos; }
	std::string GetName() { return s_Name; }
	bool GetRotate() { return s_Rotate; }
	void SetRotate(bool b);
	void SetTexture(int k, const char* p);
	glm::mat4 GetModel() { return Model; }
	void SetModel(glm::mat4 m);
	void GenSphere(size_t segs);
	void Render(glm::mat4 v, glm::mat4 p, glm::vec3 cp);
	void Render();
	void CleanUp();
};