#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// setting
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// vertex shader
const char *vertexShaderSource = R"glsl(
#version 410 core
in vec2 position;
void main()
{
 gl_Position = vec4(position, 0.0, 1.0);
}
)glsl";

// fragment shader
const char *fragmentShaderSource = R"glsl(
#version 410 core
out vec4 FragColor;
void main()
{
 FragColor = vec4(1.0f, 0.5f, 0.5f, 1.0f);
}
)glsl";

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "hello", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewInit();
	// compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// combining shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// NDC
	float vertices[] = {
		-0.5f,-0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	// init VAO dan VBO
	unsigned int VBO, VAO;
	// generate VAO dan VBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind VAO
	glBindVertexArray(VAO); // sudah nyatu dengan window
							// bind VBO to VAO 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// copy vertices to buffer memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// parse data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window)) {
		// load program
		glUseProgram(shaderProgram);
		// bind VAO tiap kali menggambar
		glBindVertexArray(VAO);

		// gambar segitiga
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
