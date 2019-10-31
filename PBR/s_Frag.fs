#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in vec3 s_Color;

void main(){

	FragColor = vec4(s_Color, 1.0);
}