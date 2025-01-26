#include "clpch.h"
#include "OGLRenderer.h"
/*
namespace Carnival {
	static uint32_t rendererID{};

	std::string vertexSrc = R"(
		#version 450

		layout(location = 0) in vec3 a_Position;
		out vec3 v_Position;

		void main(){
			v_Position = a_Position;
			gl_Position = vec4(a_Position, 1.0f);
		}
	)";

	std::string fragSrc = R"(
		#version 450

		layout(location = 0) out vec4 color;
		in vec3 v_Position;

		void main(){
			color = vec4(v_Position * 0.5 +0.5, 1.0);
		}
	)";

	static void DoShaders()
	{
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		auto src = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &src, 0);
		glCompileShader(vertexShader);
		
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		src = fragSrc.c_str();
		glShaderSource(fragShader, 1, &src, 0);
		glCompileShader(fragShader);

		rendererID = glCreateProgram();
		glAttachShader(rendererID, vertexShader);
		glAttachShader(rendererID, fragShader);
		glLinkProgram(rendererID);

		glDetachShader(rendererID, vertexShader);
		glDetachShader(rendererID, fragShader);
	}

	static void bindProgram() { glUseProgram(rendererID); }
	static void unbindProgram() { glUseProgram(0); }

	static void clean()
	{
		glDeleteProgram(rendererID);
	}

	OpenGLRenderer::OpenGLRenderer(GLFWwindow* window, bool VSync) : m_WindowHandle{ window }, m_VSync(VSync)
	{
		CL_CORE_ASSERT(window, "Window Is Null, Context Initialization Failed.");
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CL_CORE_ASSERT(status, "Failed to initialize glad!");
		setSwapInterval(m_VSync);

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);
		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		glm::mat3 vertices = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		std::vector<uint16_t> indices = { 0,1,2 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

		DoShaders();
		bindProgram();
	}
	OpenGLRenderer::~OpenGLRenderer()
	{
		clean();
	}


	void OpenGLRenderer::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	void OpenGLRenderer::loadModel(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{

	}
	void OpenGLRenderer::swapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
	void OpenGLRenderer::drawFrame()
	{
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);
	}

	void OpenGLRenderer::setSwapInterval(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}
	void OpenGLRenderer::framebufferResizeCallback()
	{
	}


}
*/