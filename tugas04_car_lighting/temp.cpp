#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <vector>

// setting
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

// vertex shader
const char *vertexShaderSource = R"glsl(
#version 130
in vec2 position;
void main()
{
 gl_Position = vec4(position, 0.0, 1.0);
}
)glsl";

// fragment shader
const char *fragmentShaderSource = R"glsl(
#version 130
out vec4 FragColor;
void main()
{
 FragColor = vec4(0.0f, 1.0f, 0.5f, 1.0f);
}
)glsl";

// wheel shader
const char *fragmentWheelShaderSource = R"glsl(
#version 130
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

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Check Vertex Shader
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(vertexShader, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}


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
    unsigned int modelLoc = glGetUniformLocation(prog.ID, "model");
    unsigned int viewLoc = glGetUniformLocation(prog.ID, "view");
    unsigned int projectionLoc = glGetUniformLocation(prog.ID, "projection");
	while (!glfwWindowShouldClose(window)) { 
        processInput(window);
		glClear(GL_COLOR_BUFFER_BIT);
        float radius = 11.0f;
		
        glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		
		glm::mat4 projection = glm::mat4(1.0f);;
		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
		view = glm::lookAt(cameraFront * radius, glm::vec3(0.0, 0.0, 0.0), cameraUp);
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		// retrieve the matrix uniform locations

		// pass them to the shaders (3 different ways)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

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

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = -sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

