#pragma once

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <algorithm>

#include "config.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "../Primitive.h"
#include "OpenGL_util/core/Renderer.h"
#include "Camera.h"

#include "OpenGL_util/core/VertexBuffer.h"
#include "OpenGL_util/core/VertexArray.h"
#include "OpenGL_util/core/IndexBuffer.h"
#include "OpenGL_util/core/VertexBufferLayout.h"
#include "OpenGL_util/core/Shader.h"

namespace ApplicationNamespace {
	class Application {
	private:
		// Render
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<VertexBufferLayout> m_VertexBufferLayout;
		std::unique_ptr<VertexArray> m_VertexArray;

		glm::mat4 m_MatProjectionVertex;

		glm::mat4 m_MatProjectionRay;
		glm::mat4 m_MatView;

		Shader* m_ShaderBasic;
		bool m_GuiEnabled = true;

		// Attributes
		Camera m_Camera;

		float m_FOV = 60.f;

		float m_ApplicationTime = 0.f;

		// Input
		bool m_IsInViewMode;
		inline static float m_CameraSpeed;

		double m_LastX = 400, m_LastY = 300;
		bool m_FirstMouseInit = true;

		inline static double s_MouseX = 0;
		inline static double s_MouseY = 0;

		void ProcessMouse(GLFWwindow* window, float deltaTime);
		static void OnMouseCallback(GLFWwindow* window, double xpos, double ypos);
		static void OnScrollCallback(GLFWwindow* window, double xpos, double ypos);

	public:
		explicit Application();
		~Application();

		void OnRender();
		void OnImGuiRender(GLFWwindow* window);
		void OnInput(GLFWwindow* window, float deltaTime);
		void OnUpdate();

		void GUI(GLFWwindow* window);
	};
}