#define M_PI 3.14159265359
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ImageLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void DrawCar(Shader&);
void DrawTriangle(Shader&);
void DrawWindow(Shader&);
void DrawLight(Shader&);
void DrawBrake(Shader&);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//---------------------------------------------------------------------------
const int na = 36;        // vertex grid size
const int nb = 18;
const int na3 = na * 3;     // line in grid size
const int nn = nb * na3;    // whole grid size
GLfloat sphere_pos[nn]; // vertex
GLfloat sphere_nor[nn]; // normal
						//GLfloat sphere_col[nn];   // color
GLint  sphere_ix[na*(nb - 1) * 6];    // indices
GLuint sphere_vbo[4] = { 0,0,0,0 };
GLuint sphere_vao[4] = { 0,0,0,0 };

void sphere_init()
{
	// generate the sphere data
	GLfloat x, y, z, a, b, da, db, r = 3.5;
	int ia, ib, ix, iy;
	da = 2.0*M_PI / GLfloat(na);
	db = M_PI / GLfloat(nb - 1);
	// [Generate sphere point data]
	// spherical angles a,b covering whole sphere surface
	for (ix = 0, b = -0.5*M_PI, ib = 0; ib<nb; ib++, b += db)
		for (a = 0.0, ia = 0; ia<na; ia++, a += da, ix += 3)
		{
			// unit sphere
			x = cos(b)*cos(a);
			y = cos(b)*sin(a);
			z = sin(b);
			sphere_pos[ix + 0] = x * r;
			sphere_pos[ix + 1] = y * r;
			sphere_pos[ix + 2] = z * r;
			sphere_nor[ix + 0] = x;
			sphere_nor[ix + 1] = y;
			sphere_nor[ix + 2] = z;
		}
	// [Generate GL_TRIANGLE indices]
	for (ix = 0, iy = 0, ib = 1; ib<nb; ib++)
	{
		for (ia = 1; ia<na; ia++, iy++)
		{
			// first half of QUAD
			sphere_ix[ix] = iy;      ix++;
			sphere_ix[ix] = iy + 1;    ix++;
			sphere_ix[ix] = iy + na;   ix++;
			// second half of QUAD
			sphere_ix[ix] = iy + na;   ix++;
			sphere_ix[ix] = iy + 1;    ix++;
			sphere_ix[ix] = iy + na + 1; ix++;
		}
		// first half of QUAD
		sphere_ix[ix] = iy;       ix++;
		sphere_ix[ix] = iy + 1 - na;  ix++;
		sphere_ix[ix] = iy + na;    ix++;
		// second half of QUAD
		sphere_ix[ix] = iy + na;    ix++;
		sphere_ix[ix] = iy - na + 1;  ix++;
		sphere_ix[ix] = iy + 1;     ix++;
		iy++;
	}
	// [VAO/VBO stuff]
	GLuint i;
	glGenVertexArrays(4, sphere_vao);
	glGenBuffers(4, sphere_vbo);
	glBindVertexArray(sphere_vao[0]);
	i = 0; // vertex
	glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo[i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_pos), sphere_pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(i);
	glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 0, 0);
	i = 1; // indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_vbo[i]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_ix), sphere_ix, GL_STATIC_DRAW);
	glEnableVertexAttribArray(i);
	glVertexAttribPointer(i, 4, GL_UNSIGNED_INT, GL_FALSE, 0, 0);
	i = 2; // normal
	glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo[i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_nor), sphere_nor, GL_STATIC_DRAW);
	glEnableVertexAttribArray(i);
	glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}
void sphere_exit()
{
	glDeleteVertexArrays(4, sphere_vao);
	glDeleteBuffers(4, sphere_vbo);
}
void sphere_draw(Shader& ourShader)
{
	glm::vec3 spherePositions[] = {
		glm::vec3(1.6f, -1.25f, 1.8f),
		glm::vec3(-1.6f,-1.25f, -2.1f),
		glm::vec3(1.6f, -1.25f, -2.1f),
		glm::vec3(-1.6f,-1.25f, 1.8f),
	};
	glm::vec3 sphereScales = glm::vec3(0.185f, 0.185f, 0.1f);

	for (int i = 0; i < 4; i++) {
		float sphereRotation[] = {
			glm::radians(90.0f),
			glm::radians(-90.0f)
		};
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, spherePositions[i]);
		model = glm::rotate(model, sphereRotation[i%2], glm::vec3(0.0f, 1.0f, 0.0f));
		if(i == 0 || i == 2) {
			model = glm::rotate(model, (float)glfwGetTime()*5, glm::vec3(0.0f, 0.0f, -1.0f));
		} else {
			model = glm::rotate(model, (float)glfwGetTime()*5, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		model = glm::scale(model, sphereScales);
		ourShader.setMat4("model", model);

		glBindVertexArray(sphere_vao[0]);
		//  glDrawArrays(GL_POINTS,0,sizeof(sphere_pos)/sizeof(GLfloat));                   // POINTS ... no indices for debug
		glDrawElements(GL_TRIANGLES, sizeof(sphere_ix) / sizeof(GLuint), GL_UNSIGNED_INT, 0);    // indices (choose just one line not both !!!)
		glBindVertexArray(0);
	}
}


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

float vertices_2[] = {
	 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

	 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

	 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f
};

float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

int main() {
	GLFWwindow* window;

	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Car Viewer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback); 

	if (glewInit() != GLEW_OK)
		std::cout << "Error GLEW Init" << std::endl;
	glEnable(GL_DEPTH_TEST);
	sphere_init();
	Shader prog("car.vert", "car.frag");
	Shader prog2("car.vert", "car.frag");
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	ImageLoader texture1("metal.jpg");
	ImageLoader texture2("white.jpg");
	ImageLoader texture3("gold.jpg");
	ImageLoader texture4("pink.jpg");
	ImageLoader texture5("awesomeface.jpg");

	prog.use();
	//prog2.use();
	prog.setInt("texture1", 0);
	prog.setInt("texture2", 0);
	prog.setInt("texture3", 0);
	prog.setInt("texture4", 0);
	prog.setInt("texture5", 0);

	unsigned int modelLoc = glGetUniformLocation(prog.ID, "model");
	unsigned int viewLoc = glGetUniformLocation(prog.ID, "view");
	unsigned int projectionLoc = glGetUniformLocation(prog.ID, "projection");

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//bind textures on corresponding texture units
		// glActiveTexture(GL_TEXTURE0);
		// texture3.use();
		glActiveTexture(GL_TEXTURE0);
		texture4.use();
		// glActiveTexture(GL_TEXTURE1);
		// texture3.use();
		prog.use();

		float radius = 11.0f;
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		
		glm::mat4 projection = glm::mat4(1.0f);;
		view = glm::lookAt(cameraFront * radius, glm::vec3(0.0, 0.0, 0.0), cameraUp);
		// view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	
		//zoom
		projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);  

		// pass them to the shaders (3 different ways)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// render car
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		DrawCar(prog);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_STATIC_DRAW);
		DrawTriangle(prog);
		
		//Render wheel
		glActiveTexture(GL_TEXTURE0);
		texture1.use();
		prog.use();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		sphere_draw(prog);

		//Render window
		glActiveTexture(GL_TEXTURE0);
		texture2.use();
		prog.use();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		DrawWindow(prog);

		//Render Light
		glActiveTexture(GL_TEXTURE0);
		texture3.use();
		prog.use();
		DrawLight(prog);

		//Render Brake
		glActiveTexture(GL_TEXTURE0);
		texture5.use();
		prog.use();
		DrawBrake(prog);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
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
void DrawTriangle(Shader& ourShader) {

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, -0.35f, -2.8f),	
	};
	glm::vec3 cubeScales[] = {
		glm::vec3(2.25f,  0.75f, 3.5f),
	};
	glEnable(GL_DEPTH);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, cubePositions[0]);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, cubeScales[0]);
	ourShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void DrawCar(Shader& ourShader) {

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.3f,  0.8f),
		glm::vec3(0.0f,  -1.2f, 2.8f),
		glm::vec3(0.0f,  -1.2f, -0.2f),
		glm::vec3(0.0f, -1.22f, -3.25f),
		glm::vec3(0.0f,  -1.0f, -2.0f),
		glm::vec3(0.0f,  -1.0f, 1.8f),
	};
	glm::vec3 cubeScales[] = {
		glm::vec3(3.5f,  2.0f,  5.0f),
		glm::vec3(3.5f,  1.0f, 1.0f),
		glm::vec3(3.5f,  1.0f, 3.0f),
		glm::vec3(3.5f,  1.0f, 1.25f),
		glm::vec3(2.5f,  1.0f, 1.25f),
		glm::vec3(2.5f,  1.0f, 1.35f),
	};
	glEnable(GL_DEPTH);
	for (unsigned int i = 0; i < 6; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		model = glm::scale(model, cubeScales[i]);
		ourShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void DrawWindow(Shader& ourShader) {

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.6f, -1.8f),
		glm::vec3(0.0f, 0.6f, 3.3f),
		glm::vec3(1.8f, 0.6f, 0.8f),
		glm::vec3(-1.8f, 0.6f, 0.8f),
		glm::vec3(1.2f, -1.0f, -3.9f),
		glm::vec3(-1.2f, -1.0f, -3.9f),
	};
	glm::vec3 cubeScales[] = {
		glm::vec3(2.8f,  1.0f, 0.1f),
		glm::vec3(2.8f,  1.0f, 0.1f),
		glm::vec3(0.1f,  1.0f, 4.0f),	
		glm::vec3(0.1f,  1.0f, 4.0f),
		glm::vec3(0.35f,  0.35f, 0.1f),
		glm::vec3(0.35f,  0.35f, 0.1f),
	};
	glEnable(GL_DEPTH);
	for (unsigned int i = 0; i < 6; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		model = glm::scale(model, cubeScales[i]);
		ourShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void DrawLight(Shader& ourShader) {

	glm::vec3 cubePositions[] = {
		glm::vec3(1.2f, -0.7f, 3.3f),
		glm::vec3(-1.2f, -0.7f, 3.3f),
	};
	glm::vec3 cubeScales[] = {
		glm::vec3(0.35f,  0.25f, 0.1f),
		glm::vec3(0.35f,  0.25f, 0.1f),
	};
	glEnable(GL_DEPTH);
	for (unsigned int i = 0; i < 2; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		model = glm::scale(model, cubeScales[i]);
		ourShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void DrawBrake(Shader& ourShader) {

	glm::vec3 cubePositions[] = {
		glm::vec3(1.2f, -1.0f, 3.3f),
		glm::vec3(-1.2f, -1.0f, 3.3f),
	};
	glm::vec3 cubeScales[] = {
		glm::vec3(0.35f,  0.35f, 0.1f),
		glm::vec3(0.35f,  0.35f, 0.1f),
	};
	glEnable(GL_DEPTH);
	for (unsigned int i = 0; i < 2; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, cubeScales[i]);
		ourShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void processInput(GLFWwindow *window)
{
    float cameraSpeed = 0.05f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  if(fov >= 1.0f && fov <= 45.0f)
  	fov -= yoffset;
  if(fov <= 1.0f)
  	fov = 1.0f;
  if(fov >= 45.0f)
  	fov = 45.0f;
}