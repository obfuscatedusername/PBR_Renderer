#include "Light.h"

Light::Light() {
	l_Pos = glm::vec3(0.0f, 0.0f, 10.0f);
	l_Color = glm::vec3(300.0);//(23.47, 21.31, 20.79);
}

void Light::SetLightPos(glm::vec3 p) {
	l_Pos = p;
}