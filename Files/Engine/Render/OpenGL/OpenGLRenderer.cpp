/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

#include "OpenGLRenderer.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"

namespace Engine
{
	namespace Render
	{
		const char* vertexShader = R"(#version 150 core

in vec3 position;
in vec2 uv;
in vec3 normal;
in vec4 colour;

out vec2 v_uv;
out vec3 v_normal;
out vec4 v_vert_colour;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(position, 1.0f);
	v_uv = uv;
	v_normal = normal;
	v_vert_colour = colour;
})";

		const char* fragmentShader = R"(#version 150 core

out vec4 colour;

in vec2 v_uv;
in vec3 v_normal;
in vec4 v_vert_colour;

uniform sampler2D u_texture;

void main()
{
	colour = v_vert_colour;
})";

		OpenGL::Shader* mainShader;


		void OpenGLRenderer::Init(GLFWwindow* window)
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
			glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

			Logging::Log("Initializing OpenGL", "info");

			/* Make the window's context current */
			glfwMakeContextCurrent(window);

			/* Initialize glad */
			gladLoadGL(glfwGetProcAddress);

			const unsigned char* c = glGetString(GL_RENDERER);
			const unsigned char* v = glGetString(GL_VERSION);

			Logging::Log("GPU: " + std::string((char*)c), "info");

			Logging::Log("OpenGL Version: " + std::string((char*)v), "info");

			mainShader = new OpenGL::Shader();

			mainShader->CompileShader(vertexShader, fragmentShader);

			mainShader->Bind();

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDisable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}

		void OpenGLRenderer::Render(GLFWwindow* window)
		{
			glViewport(0, 0, Viewport.x, Viewport.y);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


			mainShader->Bind();

			glfwSwapBuffers(window);

		}
	}
}