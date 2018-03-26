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
 FragColor = vec4(0.0f, 1.0f, 0.5f, 1.0f);
}
)glsl";

// wheel shader
const char *fragmentWheelShaderSource = R"glsl(
#version 410 core
out vec4 FragColor;
void main()
{
 FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
)glsl";

void rotateVertices(float* vertices, int numberOfVertices, float centerX, float centerY, float angle) {
	float angleSin = sin(angle);
	float angleCos = cos(angle);

	for (int i = 0; i < numberOfVertices*3; i += 3) {
		vertices[i] -= centerX;
		vertices[i+1] -= centerY;
		// rotate point
		float xnew = vertices[i] * angleCos - vertices[i+1] * angleSin;
		float ynew = vertices[i] * angleSin + vertices[i+1] * angleCos;

		vertices[i] = xnew + centerX;
		vertices[i+1] = ynew + centerY;
	}

}

int main() {
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "hello", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();
	// compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// compile fragment shader
	GLuint wheelShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(wheelShader, 1, &fragmentWheelShaderSource, NULL);
	glCompileShader(wheelShader);

	// combining shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// wheel shader program
	GLuint wheelShaderProgram = glCreateProgram();
	glAttachShader(wheelShaderProgram, vertexShader);
	glAttachShader(wheelShaderProgram, wheelShader);
	glLinkProgram(wheelShaderProgram);

	// NDC
	float vertices[] = {
		0.1f, 0.25f, 0.0f,
		-0.1f, 0.25f, 0.0f,
		-0.25f, 0.0f, 0.0f,
		-0.25f, 0.0f, 0.0f,
		0.1f, 0.25f, 0.0f,
		0.25f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		-0.5f, -0.25f, 0.0f,
		-0.5f, -0.25f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.5f, -0.25f, 0.0f
	};

	float wheelRight[] = {
		0.2f, -0.2f, 0.0f,
		0.3f, -0.3f, 0.0f,
		0.2f, -0.3f, 0.0f,
		0.2f, -0.2f, 0.0f,
		0.3f, -0.2f, 0.0f,
		0.3f, -0.3f, 0.0f
	};

	float leftWheel[] = {
		-0.2f, -0.2f, 0.0f,
		-0.3f, -0.3f, 0.0f,
		-0.2f, -0.3f, 0.0f,
		-0.2f, -0.2f, 0.0f,
		-0.3f, -0.2f, 0.0f,
		-0.3f, -0.3f, 0.0f
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

	while (!glfwWindowShouldClose(window)) { 
		glClear(GL_COLOR_BUFFER_BIT);

		// copy vertices to buffer memory
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// parse data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// load program
		glUseProgram(shaderProgram);
		// bind VAO tiap kali menggambar
		glBindVertexArray(VAO);

		// gambar segitiga
		glDrawArrays(GL_TRIANGLES, 0, 12);

		// draw right wheel
		glBufferData(GL_ARRAY_BUFFER, sizeof(wheelRight), wheelRight, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glUseProgram(wheelShaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// draw left wheel
		glBufferData(GL_ARRAY_BUFFER, sizeof(leftWheel), leftWheel, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glUseProgram(wheelShaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		glfwSwapBuffers(window);
		glfwPollEvents();

		rotateVertices(wheelRight, 6, 0.25, -0.25, 0.1);
		rotateVertices(leftWheel, 6, -0.25, -0.25, 0.1);
	}

	glfwTerminate();
	return 0;
}
