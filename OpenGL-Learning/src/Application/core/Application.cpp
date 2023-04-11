#include "Application.h"

ApplicationNamespace::Application::Application()
{
	m_ShaderBasic = new Shader("res/shader/shader_basic.vert", "res/shader/shader_basic.frag");
	m_MatProjectionVertex = glm::ortho(0.0f, (float)conf.WIN_WIDTH, 0.0f, (float)conf.WIN_HEIGHT, -1.0f, 1.0f);

	unsigned int index[] = {
		0, 1, 2, 2, 3, 0
	};

	m_IndexBuffer = std::make_unique<IndexBuffer>(index, 6);
	m_VertexBuffer = std::make_unique<VertexBuffer>(4, sizeof(ApplicationNamespace::Primitive::Vertex));

	ApplicationNamespace::Primitive::Vertex vert[] = {
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, conf.WIN_HEIGHT, 0.f),
		glm::vec3(conf.WIN_WIDTH, conf.WIN_HEIGHT, 0.f),
		glm::vec3(conf.WIN_WIDTH, 0.f, 0.f)
	};

	m_VertexBuffer->Bind();
	m_VertexBuffer->AddVertexData(vert, sizeof(ApplicationNamespace::Primitive::Vertex) * 4);

	m_VertexBufferLayout = std::make_unique<VertexBufferLayout>();
	m_VertexBufferLayout->Push<float>(3);	// Position

	m_VertexArray = std::make_unique<VertexArray>();
	m_VertexArray->AddBuffer(*m_VertexBuffer, *m_VertexBufferLayout);

	m_Camera.Position = { 0.f, 1.5f, -3.f };
	m_Camera.Front = { 0.f, 0.f, 1.f };

	m_ShaderBasic->Bind();
	m_ShaderBasic->SetUniformMat4f("u_Projection", m_MatProjectionVertex);
	m_ShaderBasic->SetUniform3f("u_CameraOrigin", m_Camera.Position.x, m_Camera.Position.y, m_Camera.Position.z);
	m_ShaderBasic->SetUniform2f("u_Resolution", (float)conf.WIN_WIDTH, (float)conf.WIN_HEIGHT);

	m_CameraSpeed = 0.05f;
}

void ApplicationNamespace::Application::ProcessMouse(GLFWwindow* window, float deltaTime)
{
	if (m_FirstMouseInit) {
		m_LastX = s_MouseX;
		m_LastY = s_MouseY;
		m_FirstMouseInit = false;
	}

	static bool keyPressed1 = false;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !keyPressed1)
	{
		keyPressed1 = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_IsInViewMode = true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && keyPressed1) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		keyPressed1 = false;
		m_IsInViewMode = false;
	}

	double xoffset = s_MouseX - m_LastX;
	double yoffset = m_LastY - s_MouseY;

	yoffset *= deltaTime * 20.f;
	xoffset *= deltaTime * 20.f;

	m_LastX = s_MouseX;
	m_LastY = s_MouseY;

	const double sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	if (keyPressed1) {
		m_Camera.yaw += (float)xoffset;
		m_Camera.pitch += (float)yoffset;

		if (m_Camera.pitch > 89.99f)
			m_Camera.pitch = 89.99f;
		if (m_Camera.pitch < -89.99f)
			m_Camera.pitch = -89.99f;

		glm::vec3 direction;

		direction.x = cos(glm::radians(m_Camera.yaw)) * cos(glm::radians(m_Camera.pitch));
		direction.y = sin(glm::radians(m_Camera.pitch));
		direction.z = sin(glm::radians(m_Camera.yaw)) * cos(glm::radians(m_Camera.pitch));

		m_Camera.Front = glm::normalize(direction);
	}
}

void ApplicationNamespace::Application::OnMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	ApplicationNamespace::Application::s_MouseX = float(xpos);
	ApplicationNamespace::Application::s_MouseY = float(ypos);
}

void ApplicationNamespace::Application::OnScrollCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (ypos < 0.f) m_CameraSpeed *= 0.9f;
	else if (ypos > 0.f) m_CameraSpeed *= 1.1f;
}

ApplicationNamespace::Application::~Application()
{
	delete m_ShaderBasic;
}

void ApplicationNamespace::Application::OnRender()
{
	m_ShaderBasic->Bind();
	m_VertexArray->Bind();
	m_IndexBuffer->Bind();
	GLCall(BatchRenderer::Draw(*m_VertexArray, *m_IndexBuffer, *m_ShaderBasic, GL_TRIANGLES));
}

void ApplicationNamespace::Application::OnImGuiRender(GLFWwindow* window)
{
	if(m_GuiEnabled) GUI(window);
}

void ApplicationNamespace::Application::OnInput(GLFWwindow* window, float deltaTime)
{
	static bool keyPressedC = false;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !keyPressedC)
	{
		keyPressedC = true;
		m_GuiEnabled = !m_GuiEnabled;
	}
	else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && keyPressedC) {
		keyPressedC = false;
	}

	glfwGetCursorPos(window, &s_MouseX, &s_MouseY);
	glfwSetScrollCallback(window, OnScrollCallback);
	ProcessMouse(window, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_Camera.Position += m_CameraSpeed * m_Camera.Front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_Camera.Position -= m_CameraSpeed * m_Camera.Front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_Camera.Position -= glm::normalize(glm::cross(m_Camera.Front, m_Camera.Up)) * m_CameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_Camera.Position += glm::normalize(glm::cross(m_Camera.Front, m_Camera.Up)) * m_CameraSpeed;
}

void ApplicationNamespace::Application::OnUpdate()
{
	m_MatView = glm::lookAt(m_Camera.Position, m_Camera.Position + m_Camera.Front, m_Camera.Up);
	m_MatProjectionRay = glm::perspectiveFov(glm::radians(m_FOV), (float)conf.WIN_WIDTH, (float)conf.WIN_HEIGHT, 0.1f, 100.f);

	m_ApplicationTime = (float)glfwGetTime();
	m_ShaderBasic->Bind();
	m_ShaderBasic->SetUniform1f("u_Time", m_ApplicationTime);
	m_ShaderBasic->SetUniform3f("u_CameraOrigin", m_Camera.Position.x, m_Camera.Position.y, m_Camera.Position.z);
	m_ShaderBasic->SetUniform2f("u_Mouse", (float)s_MouseX, (float)s_MouseY);

	m_ShaderBasic->SetUniformMat4f("u_InverseView", glm::inverse(m_MatView));
	m_ShaderBasic->SetUniformMat4f("u_InverseProjection", glm::inverse(m_MatProjectionRay));
}

void ApplicationNamespace::Application::GUI(GLFWwindow* window)
{
	ImGui::SetNextWindowPos({ 0.f, 0.f });
	ImGui::SetNextWindowSize({ (const float)conf.WIN_WIDTH / 4.f, (const float)conf.WIN_HEIGHT });
	
	ImGui::Begin("Scene");
	ImGui::Text("Press C to hide GUI");
	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.7f, 0.35f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.9f, 0.55f, 0.0f, 1.0f));

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Sampletext");
	}

	if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Sampletext");
	}

	if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Sampletext");
	}

	ImGui::PopStyleColor(2);

	ImGui::End();
}
