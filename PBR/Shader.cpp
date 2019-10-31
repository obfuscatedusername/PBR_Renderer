#include "Shader.h"

Shader::Shader() {}

void Shader::Make(const GLchar* vPath, const GLchar* fPath) {
	std::string vCode, fCode;
	std::ifstream vShaderFile, fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vShaderFile.open(vPath);
		fShaderFile.open(fPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vCode = vShaderStream.str();
		fCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cerr << "ERROR: Shaders not successfully read! " << std::endl;
	}
	const char* vShaderCode = vCode.c_str();
	const char* fShaderCode = fCode.c_str();
	unsigned int v, f;
	int success;
	char infoLog[512];
	v = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v, 1, &vShaderCode, NULL);
	glCompileShader(v);

	glGetShaderiv(v, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(v, 512, NULL, infoLog);
		std::cerr << "ERROR: vertshader compilation failure\n" << infoLog << std::endl;
	}

	f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f, 1, &fShaderCode, NULL);
	glCompileShader(f);

	glGetShaderiv(f, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(f, 512, NULL, infoLog);
		std::cerr << "ERROR: fragshader compilation failure\n" << infoLog << std::endl;
	}
	ID = glCreateProgram();
	glAttachShader(ID, v);
	glAttachShader(ID, f);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(ID, 512, NULL, infoLog);
		std::cerr << "ERROR: shaderProgram compilation failure\n" << infoLog << std::endl;
	}

	glDeleteShader(v);
	glDeleteShader(f);
}

void Shader::Use() {
	glUseProgram(ID);
}

void Shader::SetBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::SetInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::SetFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetMat4(const std::string &name, glm::mat4 &value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::SetVec3(const std::string &name, glm::vec3 &value) const 
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::SetVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}