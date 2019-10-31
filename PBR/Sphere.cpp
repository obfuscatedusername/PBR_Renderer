#include "Sphere.h"

Sphere::Sphere(glm::vec3 pos, std::string n) {
	//init vars
	Model = glm::mat4(1.0f);
	s_Pos = pos;
	s_Name = n;
	s_Rotate = false;
	//init GL ting
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	Model = glm::mat4(1.0);
	Model = glm::translate(Model, s_Pos);
	//Make the sphere
	GenSphere(16);
}

void Sphere::GenSphere(size_t segs) {
	//generate the sphere coordinates, UVs, and Normals
	for (size_t i = 0; i <= segs; i++) {
		for (size_t j = 0; j <= segs; j++) {
			float xSeg = (float)j / (float)segs;
			float ySeg = (float)i / (float)segs;
			float xPos = std::cos(xSeg * 2.0f * PI) * std::sin(ySeg * PI);
			float yPos = std::cos(ySeg * PI);
			float zPos = std::sin(xSeg * 2.0f * PI) * std::sin(ySeg * PI);
			s_Positions.push_back(glm::vec3(xPos, yPos, zPos));
			s_UV.push_back(glm::vec2(xSeg, ySeg));
			s_Normals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}

	//generate the sphere indices
	bool oddRow = false;
	for (int i = 0; i < segs; i++) {
		if (!oddRow) {
			for (int j = 0; j <= segs; j++) {
				s_Indices.push_back(i		* (segs + 1) + j);
				s_Indices.push_back((i + 1) * (segs + 1) + j);
			}
		}
		else {
			for (int j = segs; j >= 0; j--) {
				s_Indices.push_back((i + 1)	* (segs + 1) + j);
				s_Indices.push_back(i		* (segs + 1) + j);
			}
		}
		oddRow = !oddRow;
	}
	s_IndexCount = s_Indices.size();

	//Add all sphere data to data vector
	for (size_t i = 0; i < s_Positions.size(); i++) {
		//add position data
		s_Data.push_back(s_Positions[i].x);
		s_Data.push_back(s_Positions[i].y);
		s_Data.push_back(s_Positions[i].z);

		//check there are UVs, if so add them next
		if (s_UV.size() > 0) {
			s_Data.push_back(s_UV[i].x);
			s_Data.push_back(s_UV[i].y);
		}

		//check there are normals, if so add them next
		if (s_Normals.size() > 0) {
			s_Data.push_back(s_Normals[i].x);
			s_Data.push_back(s_Normals[i].y);
			s_Data.push_back(s_Normals[i].z);
		}
	}

}

void Sphere::Render(glm::mat4 v, glm::mat4 p, glm::vec3 cp) {
	if (VAO == 0) { //Check to see if it's running for the first time, to save the frames
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, s_Data.size() * sizeof(float), &s_Data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, s_Indices.size() * sizeof(unsigned int), &s_Indices[0], GL_STATIC_DRAW);
		//generate the stride for data: 3 position floats + 2 uv floats + 3 normals floats * sizeof(float)
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLE_STRIP, s_IndexCount, GL_UNSIGNED_INT, 0);
}

void Sphere::Render() {
	//Bind the sphere's texture to the texture slots
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, s_Material.albedo);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, s_Material.normal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, s_Material.metallic);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, s_Material.roughness);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, s_Material.ao);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, s_Data.size() * sizeof(float), &s_Data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, s_Indices.size() * sizeof(unsigned int), &s_Indices[0], GL_STATIC_DRAW);
	//generate the stride for data: 3 position floats + 2 uv floats + 3 normals floats * sizeof(float)
	float stride = (3 + 2 + 3) * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLE_STRIP, s_IndexCount, GL_UNSIGNED_INT, 0);
}

unsigned int Sphere::LoadTexture(char const* p) {
	std::cout << "Loading: " << p;
	unsigned int tex;
	glGenTextures(1, &tex);
	int w, h, nComp;
	unsigned char* d = stbi_load(p, &w, &h, &nComp, 0);
	if (d) {
		GLenum format;
		if (nComp == 1)
			format = GL_RED;
		else if (nComp == 3)
			format = GL_RGB;
		else if (nComp == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, d);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(d);
	}
	else {
		std::cerr << "Texture failed to load from: " << p << std::endl;
		stbi_image_free(d);
	}
	std::cout << " | Loaded!" << std::endl;
	return tex;
}

void Sphere::SetTexture(int k, const char* p) {
	switch (k) {
	case 0: s_Material.albedo		= LoadTexture(p); break;
	case 1: s_Material.normal		= LoadTexture(p); break;
	case 2: s_Material.metallic		= LoadTexture(p); break;
	case 3: s_Material.roughness	= LoadTexture(p); break;
	case 4: s_Material.ao			= LoadTexture(p); break;
	}
}

void Sphere::SetRotate(bool b) {
	s_Rotate = b;
}

void Sphere::SetModel(glm::mat4 m) {
	Model = m;
}

//Delete all the things
void Sphere::CleanUp() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}