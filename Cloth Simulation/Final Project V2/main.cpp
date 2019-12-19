#define GLM_FORCE_RADIANS
#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#define GLFW_DLL
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <io.h>
#include <string>
#include <sstream> 

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "ShadersV2.h"
#include "texture.h"
#include "CustomCameraKeyboard.h"
#include "CallBack.hpp"
#include "Cloth.h"

#define DEBUG

const std::string TITLE_NAME = "Final Project Cloth Simulation";

//Call backs for GLFW
void framebufferSizeCallback(GLFWwindow* window, int w, int h);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void keyCallback(GLFWwindow* window);
void error_callback(int error, const char* description);

void drawGUI();

float eyex, eyey, eyez;	// current user position

//Shaders Stuff
glm::vec4 clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
glm::vec4 light = glm::vec4(0.0, 0.0, 0.3, 1.0f);

bool use= false;

int main(int argc, char** argv) {
	GLFWwindow* window;
	const char* glsl_version = "#version 330";

	//Context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		fprintf(stderr, "can't initialize GLFW\n");
		glfwTerminate();
		return -1;
	}

	window = glfwCreateWindow(WIDTH, HEIGHT, TITLE_NAME.c_str(), NULL, NULL);
	if (!window) {
		fprintf(stderr, "Failed to Create Window\n");
		glfwTerminate();
		return -2;
	}

	//Setting Callbacks 
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);

	GLenum error = glewInit();
	if (error != GLEW_OK) {
		printf("Error starting GLEW: %s\n", glewGetErrorString(error));
		return -3;
	}
#ifdef DEBUG
	//Debug Message
	if (glDebugMessageCallback != NULL) {
		glDebugMessageCallback((GLDEBUGPROC)openGlDebugCallback, NULL);
	}
	glEnable(GL_DEBUG_OUTPUT);
#endif // DEBUG

	std::cout << "GLEW Version:   " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "GPU Vendor:     " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "GPU Model:      " << glGetString(GL_RENDERER) << std::endl << std::endl;

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glViewport(0, 0, WIDTH, HEIGHT);

	//Setting up ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	projection = glm::perspective(glm::radians(70.0f), (float)WIDTH / (float)HEIGHT, 0.01f, 100.0f);

	Cloth mesh = Cloth(10,10,10,0.1);

	eyex = 0.0;
	eyez = 0.0;
	eyey = 3.0;
	light = glm::vec4(eyex, eyey, eyez, 1.0f);

	glfwSwapInterval(1);
	lastTime = glfwGetTime();

	glm::vec3 eye(5.0, 10.0, -3.0);
	glm::vec3 look(5.0, 0.0, 5.0);
	glm::vec3 up(0.0, 1.0, 0.0);

	glm::mat4 view = glm::lookAt(eye, look, up);

	while (!glfwWindowShouldClose(window)) {
		//Frame stuff
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		glfwPollEvents();

		drawGUI();

		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		mesh.renderGeometry(projection, view);
		if (use) {
			mesh.updateGeometry(deltaTime);

		}
		keyCallback(window);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

}

void drawGUI() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Shader Controls");
		ImGui::Checkbox("Start/Pause Sim", &use);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
}