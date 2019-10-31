// PBR.cpp : This file contains the 'main' function. Program execution begins and ends there.

//OpenGL, GLAD, glm, ImGui includes 
#include <GLAD/include/glad/glad.h>
#include <glfw3.3/include/GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

//user generated class includes
#include "Shader.h"
#include "stb_image.h"
#include "Sphere.h"
#include "Camera.h"
#include "Light.h"

//Standard c++ includes
#include <iostream>
#include <vector>

// forward method declarations
void framebuffer_size_callback(GLFWwindow* win, int width, int height);
bool init();
void input(GLFWwindow* win);
void CalculateDT();
void GenSpheres();
void uiSetup();


glm::vec3 cols[10] = {	glm::vec3(1.0, 0.0, 0.0), 
						glm::vec3(1.0, 1.0, 0.0), 
						glm::vec3(1.0, 0.0, 1.0), 
						glm::vec3(1.0, 1.0, 1.0), 
						glm::vec3(0.0, 1.0, 1.0),
						glm::vec3(0.0, 1.0, 0.0),
						glm::vec3(0.0, 0.0, 1.0),
						glm::vec3(0.5, 0.5, 0.5),
						glm::vec3(0.0, 0.0, 0.0),
						glm::vec3(1.0, 0.5, 0.5)};
//Global vars !DANGER!
GLFWwindow* window;
Camera m_Camera;
static int s_Width = 1280, s_Height = 720;
static int CamChoice = 0;
static bool rotateCam = false, camMenGenerated = false, rotSphere = false, oscilateLights = false;
static float lightMultiplier = 5.0f;
float dt, lf;
glm::mat4 Projection;
glm::vec3 sphereCameOffset = glm::vec3(0.0f, 0.0f, -3.0f);
std::vector<Sphere> m_Spheres;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
const char* camChoices = "Main Camera\0Sphere 0\0Sphere 1\0Sphere 2\0Sphere 3\0Sphere 4\0Sphere 5\0Sphere 6\0Sphere 7\0Sphere 8\0Sphere 9";
Light lights[4];
Shader m_Shader;
glm::mat4 Model;


//Method used to init all the things, e.g. OpenGL context, window, glfw, glad, etc.
bool init() {
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(s_Width, s_Height, "PBR Renderer", NULL, NULL);
	if (window == NULL) {
		std::cerr << "Failed to create a GLFW Window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialise GLAD" << std::endl;
		return false;
	}

	//init delta time vars
	dt = lf = 0.0f;

	//Generate the spheres
	GenSpheres();

	//Setup IMGUI tings
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	
	m_Shader.Make("pbr_Vertex.vs", "pbr_Frag.fs");

	lights[0].SetLightPos(glm::vec3(-5.0f, 2.0f, 5.0f));
	lights[1].SetLightPos(glm::vec3(5.0f, 2.0f, 5.0f));
	lights[2].SetLightPos(glm::vec3(-5.0f, -2.0f, 5.0f));
	lights[3].SetLightPos(glm::vec3(5.0f, -2.0f, 5.0f));

	return true;
}

//Simple method to generate (currently 10) spheres, also used to load the PBR textures into memory and
//attach to each of the spheres, saving into the material struct.
void GenSpheres() {
	int c = 0;
	float stride = -5.0f;
	for (int i = 0; i < 10; i++) {
		glm::vec3 sPos;
		if (c == 5)
			stride = -5.0f;

		if (c < 5) {
			sPos = glm::vec3(stride, 2.0, 0.0);
			stride += 2.5f;
			c++;
		}
		else {
			sPos = glm::vec3(stride, -2.0, 0.0);
			stride += 2.5f;
			c++;
		}
		std::string sn = "Sphere " + std::to_string(i);
		m_Spheres.push_back(Sphere(sPos, sn));
	}

	//Load all the textures (currently doubling up)
	{
		m_Spheres[0].SetTexture(0, "textures/ground#23/ground23_col.jpg");
		m_Spheres[0].SetTexture(1, "textures/ground#23/ground23_nrm.jpg");
		m_Spheres[0].SetTexture(2, "textures/ground#23/ground23_disp.jpg");
		m_Spheres[0].SetTexture(3, "textures/ground#23/ground23_rgh.jpg");
		m_Spheres[0].SetTexture(4, "textures/ground#23/ground23_AO.jpg");

		m_Spheres[1].SetTexture(0, "textures/Rock#22/Rock22_col.jpg");
		m_Spheres[1].SetTexture(1, "textures/Rock#22/Rock22_nrm.jpg");
		m_Spheres[1].SetTexture(2, "textures/Rock#22/Rock22_disp.jpg");
		m_Spheres[1].SetTexture(3, "textures/Rock#22/Rock22_rgh.jpg");
		m_Spheres[1].SetTexture(4, "textures/Rock#22/Rock22_AO.jpg");

		m_Spheres[2].SetTexture(0, "textures/ground#31/Ground31_col.jpg");
		m_Spheres[2].SetTexture(1, "textures/ground#31/Ground31_nrm.jpg");
		m_Spheres[2].SetTexture(2, "textures/ground#31/Ground31_disp.jpg");
		m_Spheres[2].SetTexture(3, "textures/ground#31/Ground31_rgh.jpg");
		m_Spheres[2].SetTexture(4, "textures/ground#31/Ground31_AO.jpg");

		m_Spheres[3].SetTexture(0, "textures/metal#03/Metal03_col.jpg");
		m_Spheres[3].SetTexture(1, "textures/metal#03/Metal03_nrm.jpg");
		m_Spheres[3].SetTexture(2, "textures/metal#03/Metal03_disp.jpg");
		m_Spheres[3].SetTexture(3, "textures/metal#03/Metal03_rgh.jpg");
		m_Spheres[3].SetTexture(4, "textures/metal#03/Metal03_met.jpg");

		m_Spheres[4].SetTexture(0, "textures/rocks#02/Rocks02_col.jpg");
		m_Spheres[4].SetTexture(1, "textures/rocks#02/Rocks02_nrm.jpg");
		m_Spheres[4].SetTexture(2, "textures/rocks#02/Rocks02_disp.jpg");
		m_Spheres[4].SetTexture(3, "textures/rocks#02/Rocks02_rgh.jpg");
		m_Spheres[4].SetTexture(4, "textures/rocks#02/Rocks02_AO.jpg");

		m_Spheres[5].SetTexture(0, "textures/pavingstones42/PavingStones42_col.jpg");
		m_Spheres[5].SetTexture(1, "textures/pavingstones42/PavingStones42_nrm.jpg");
		m_Spheres[5].SetTexture(2, "textures/pavingstones42/PavingStones42_disp.jpg");
		m_Spheres[5].SetTexture(3, "textures/pavingstones42/PavingStones42_rgh.jpg");
		m_Spheres[5].SetTexture(4, "textures/pavingstones42/PavingStones42_AO.jpg");

		m_Spheres[6].SetTexture(0, "textures/Rock19/Rock19_col.jpg");
		m_Spheres[6].SetTexture(1, "textures/Rock19/Rock19_nrm.jpg");
		m_Spheres[6].SetTexture(2, "textures/Rock19/Rock19_disp.jpg");
		m_Spheres[6].SetTexture(3, "textures/Rock19/Rock19_rgh.jpg");
		m_Spheres[6].SetTexture(4, "textures/Rock19/Rock19_AO.jpg");

		m_Spheres[7].SetTexture(0, "textures/Bricks23/Bricks23_col.jpg");
		m_Spheres[7].SetTexture(1, "textures/Bricks23/Bricks23_nrm.jpg");
		m_Spheres[7].SetTexture(2, "textures/Bricks23/Bricks23_disp.jpg");
		m_Spheres[7].SetTexture(3, "textures/Bricks23/Bricks23_rgh.jpg");
		m_Spheres[7].SetTexture(4, "textures/Bricks23/Bricks23_AO.jpg");

		m_Spheres[8].SetTexture(0, "textures/Metal18/Metal18_col.jpg");
		m_Spheres[8].SetTexture(1, "textures/Metal18/Metal18_nrm.jpg");
		m_Spheres[8].SetTexture(2, "textures/Metal18/Metal18_disp.jpg");
		m_Spheres[8].SetTexture(3, "textures/Metal18/Metal18_rgh.jpg");
		m_Spheres[8].SetTexture(4, "textures/Metal18/Metal18_met.jpg");

		m_Spheres[9].SetTexture(0, "textures/Fabric20/Fabric20_col.jpg");
		m_Spheres[9].SetTexture(1, "textures/Fabric20/Fabric20_nrm.jpg");
		m_Spheres[9].SetTexture(2, "textures/Fabric20/Fabric20_disp.jpg");
		m_Spheres[9].SetTexture(3, "textures/Fabric20/Fabric20_rgh.jpg");
		m_Spheres[9].SetTexture(4, "textures/Fabric20/Fabric20_mask.jpg");
	}
}

// Method for setting up ImGui, add all the bits and bobs we want to the 'debug' menu
void uiSetup() {
	static float f = 0.0f;
	ImGui::Begin("PBR Renderer Menu");  // Create a window
	ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Separator(); //separating line
	//Generate a Camera dropdown menu to select different camera options
	if (ImGui::Combo("Camera Choice", &CamChoice, camChoices)) {
		rotSphere = false;
		switch (CamChoice){
		case 0: 
			m_Camera.ResetCamPosition();
			break;
		case 1: 
			m_Camera.UpdatePos(m_Spheres[CamChoice-1].GetSpherePos() - sphereCameOffset); 
			m_Camera.UpdateLookAt(m_Spheres[CamChoice - 1].GetSpherePos()); 
			break;
		case 2:
			m_Camera.UpdatePos(m_Spheres[CamChoice - 1].GetSpherePos() - sphereCameOffset);
			m_Camera.UpdateLookAt(m_Spheres[CamChoice - 1].GetSpherePos());
			break;
		case 3:
			m_Camera.UpdatePos(m_Spheres[CamChoice - 1].GetSpherePos() - sphereCameOffset);
			m_Camera.UpdateLookAt(m_Spheres[CamChoice - 1].GetSpherePos());
			break;
		case 4:
			m_Camera.UpdatePos(m_Spheres[CamChoice - 1].GetSpherePos() - sphereCameOffset);
			m_Camera.UpdateLookAt(m_Spheres[CamChoice - 1].GetSpherePos());
			break;
		case 5:
			m_Camera.UpdatePos(m_Spheres[CamChoice - 1].GetSpherePos() - sphereCameOffset);
			m_Camera.UpdateLookAt(m_Spheres[CamChoice - 1].GetSpherePos());
			break;
		case 6:
			m_Camera.UpdatePos(m_Spheres[CamChoice - 1].GetSpherePos() - sphereCameOffset);
			m_Camera.UpdateLookAt(m_Spheres[CamChoice - 1].GetSpherePos());
			break;
		case 7:
			m_Camera.UpdatePos(m_Spheres[CamChoice - 1].GetSpherePos() - sphereCameOffset);
			m_Camera.UpdateLookAt(m_Spheres[CamChoice - 1].GetSpherePos());
			break;
		case 8:
			m_Camera.UpdatePos(m_Spheres[CamChoice - 1].GetSpherePos() - sphereCameOffset);
			m_Camera.UpdateLookAt(m_Spheres[CamChoice - 1].GetSpherePos());
			break;
		case 9:
			m_Camera.UpdatePos(m_Spheres[CamChoice - 1].GetSpherePos() - sphereCameOffset);
			m_Camera.UpdateLookAt(m_Spheres[CamChoice - 1].GetSpherePos());
			break;
		case 10:
			m_Camera.UpdatePos(m_Spheres[CamChoice - 1].GetSpherePos() - sphereCameOffset);
			m_Camera.UpdateLookAt(m_Spheres[CamChoice - 1].GetSpherePos());
			break;
		}
	}
	ImGui::Checkbox("Rotate Sphere", &rotSphere);

	ImGui::Separator(); //separating line
	ImGui::DragFloat("Light Intensity", &lightMultiplier, 0.1, 5.0, 15.0);
	ImGui::Checkbox("Oscillate Lighting", &oscilateLights);
	ImGui::Separator(); //separating line
	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
	ImGui::Separator(); //separating line
	
	//End the drawing of the menu
	ImGui::End();
}

void RotateCamRoundLookAt() {
	float r = -15.0f;
	float camX = sin(glfwGetTime()) * r;
	float camZ = cos(glfwGetTime()) * r;
	m_Camera.UpdatePos(glm::vec3(camX, 0.0f, camZ));
}

//main entry point for the program
int main()
{
	//init things for use in the simulation
	if (!init()) {
		std::cerr << "ERROR: Could not init()" << std::endl;
		return 0;
	}
	glEnable(GL_DEPTH_TEST);
	Projection = glm::mat4(1.0f);
	Projection = glm::perspective(glm::radians(45.0f), (float)s_Width / (float)s_Height, 0.1f, 100.0f);
	
	//use the shader, setup the correct ints for the texture maps
	m_Shader.Use();
	m_Shader.SetInt("albedoMap", 0);
	m_Shader.SetInt("normalMap", 1);
	m_Shader.SetInt("metallicMap", 2);
	m_Shader.SetInt("roughnessMap", 3);
	m_Shader.SetInt("aoMap", 4);

	/// Main program loop, do all the rendering and send data to shaders, etc
	while (!glfwWindowShouldClose(window)) {
		CalculateDT();
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //render as wireframe only
		
		//Use the shader, send it the specific mat data
		m_Shader.Use();
		glm::mat4 v = m_Camera.GetView();
		m_Shader.SetMat4("Projection", Projection); //Send Projection mat to the shader
		m_Shader.SetMat4("View", v); //Send the View mat to the shader
		//Get the camera position and send it to the shader
		glm::vec3 cp = m_Camera.GetPos();
		m_Shader.SetVec3("camPos", cp);

		///		Iterate over the sphere array, set Model matrix send it to the shader, then render it
		for (size_t i = 0; i < m_Spheres.size(); i++) {
			//Create/set the Model mat for each sphere
			Model = m_Spheres[i].GetModel();
			//Model = glm::translate(Model, m_Spheres[i].GetSpherePos());
			if (rotSphere && i == CamChoice - 1) {
				Model = glm::rotate(Model, glm::radians(50.0f) * dt, glm::vec3(0.0, 1.0, 0.0));
			}
			m_Shader.SetMat4("Model", Model);
			m_Spheres[i].SetModel(Model);
			//Finally, call Render for each sphere
			m_Spheres[i].Render();
		}
		///	Setuo all the ligts, send the information to the shader
		for (size_t i = 0; i < 4; i++) {
			glm::vec3 np = lights[i].GetLightPos();
			if (oscilateLights) {
				np = lights[i].GetLightPos() + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			}
			glm::vec3 c = lights[i].GetLightColor();
			c *= lightMultiplier;
			m_Shader.SetVec3("l_Positions[" + std::to_string(i) + "]", np);
			m_Shader.SetVec3("l_Colors[" + std::to_string(i) + "]", c);
		}

		//Setup and Render the ImGui things
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		uiSetup();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//Deal with the menu things 
		input(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//When we've broken out of the loop, do some cleaning up
	for (size_t i = 0; i < m_Spheres.size();i++)
		m_Spheres[i].CleanUp();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

//Handle input into the program (mainly for escape closing the program)
void input(GLFWwindow* win) {
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(win, true);
	}

}

//Callback method to deal with resizing the window
void framebuffer_size_callback(GLFWwindow* win, int width, int height) {
	glViewport(0, 0, width, height);
}

//simple method of calculating delta time for use in rotations and such
void CalculateDT() {
	float cf = glfwGetTime();
	dt = cf - lf;
	lf = cf;
}
