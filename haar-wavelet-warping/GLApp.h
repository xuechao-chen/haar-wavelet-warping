#pragma once
#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include "ShaderProgram.h"
#include "ShaderStorageBuffer.h"
#include "VertexBuffer.h"
#include "Texture.h"

class GLApp
{
	unsigned int m_Width, m_Height;
	std::string m_Name;
	GLFWwindow* m_pWindow = nullptr;

public:
	GLApp(unsigned int width, unsigned int height, const std::string& name) : m_Width(width), m_Height(height), m_Name(name) {}

	~GLApp() 
	{ 
		glfwTerminate();
	}

	void run()
	{
		init();
		while (!glfwWindowShouldClose(m_pWindow))
		{
			processInput(m_pWindow);
			render();
		}
	}

protected:
	virtual void onInit() {}

	virtual void onRender() {}

private:
	void init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_pWindow = glfwCreateWindow(m_Width, m_Height, "LearnOpenGL", NULL, NULL);
		if (m_pWindow == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(m_pWindow);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}

		onInit();
	}

	void render()
	{
		onRender();
		glfwSwapBuffers(m_pWindow);
		glfwPollEvents();
	}

	void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}
};

#define LAUNCH_APPLICATION(width, height, app)                                             \
		std::shared_ptr<GLApp> pApp = std::make_shared<app>(width, height, "Application"); \
		pApp->run();