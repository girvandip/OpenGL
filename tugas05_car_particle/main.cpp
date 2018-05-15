#define M_PI 3.14159265359
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ImageLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include "Shader.h"
#include <iostream>
#include <algorithm>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void DrawCar(Shader&);
void DrawTriangle(Shader&);
void DrawWindow(Shader&);
void DrawLight(Shader&);
void DrawBrake(Shader&);
void DrawKnalpot(Shader&);
void DrawLampu(Shader& ourShader, glm::vec3 position);
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

// CPU representation of a particle
struct Particle {
	glm::vec3 pos, speed;
	unsigned char r,g,b,a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

const int MaxParticles = 1000;
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle = 0;

const int RainMaxParticles = 1000;
Particle RainContainer[RainMaxParticles];
int LastUsedRain = 0;

const int SplashMaxParticles = 1000;
Particle SplashContainer[SplashMaxParticles];
int LastUsedSplash = 0;

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int FindUnusedParticle(){

	for(int i=LastUsedParticle; i<MaxParticles; i++){
		if (ParticlesContainer[i].life < 0){
			LastUsedParticle = i;
			return i;
		}
	}

	for(int i=0; i<LastUsedParticle; i++){
		if (ParticlesContainer[i].life < 0){
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int FindUnusedRain(){

	for(int i=LastUsedRain; i < RainMaxParticles; i++){
		if (RainContainer[i].life < 0){
			LastUsedRain = i;
			return i;
		}
	}

	for(int i=0; i<LastUsedRain; i++){
		if (RainContainer[i].life < 0){
			LastUsedRain = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int FindUnusedSplash(){

	for(int i=LastUsedSplash; i < SplashMaxParticles; i++){
		if (SplashContainer[i].life < 0){
			LastUsedSplash = i;
			return i;
		}
	}

	for(int i=0; i < LastUsedSplash; i++){
		if (SplashContainer[i].life < 0){
			LastUsedSplash = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void SortParticles(Particle ParticleContainer[], int MaxParticle){
	std::sort(&ParticleContainer[0], &ParticleContainer[MaxParticle]);
}

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
	i = 2; // indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_vbo[i]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_ix), sphere_ix, GL_STATIC_DRAW);
	glEnableVertexAttribArray(i);
	glVertexAttribPointer(i, 4, GL_UNSIGNED_INT, GL_FALSE, 0, 0);
	i = 1; // normal
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
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, //Segitiga belakang, z-
	 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
	
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, //Segitiga depan, z+
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

	 -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f, //kotak bawah y-
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //kotak kiri x+
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,

	 -0.5f, -0.5f, -0.5f,  -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, //kotak miring x-, y+
	 0.5f,  0.5f, -0.5f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	 -0.5f, -0.5f,  0.5f,  -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	 -0.5f, -0.5f,  0.5f,  -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f
};

float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f
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
	Shader prog("2.2.basic_lighting.vs", "2.2.basic_lighting.fs");
	Shader lampShader("2.2.lamp.vs", "2.2.lamp.fs");

	// particle program
	Shader particleProg("Particle.vertexshader", "Particle.fragmentshader");

	// vertex
	GLuint CameraRight_worldspace_ID  = glGetUniformLocation(particleProg.ID, "CameraRight_worldspace");
	GLuint CameraUp_worldspace_ID  = glGetUniformLocation(particleProg.ID, "CameraUp_worldspace");
	GLuint ViewProjMatrixID = glGetUniformLocation(particleProg.ID, "VP");

	// fragment shader
	GLuint TextureID  = glGetUniformLocation(particleProg.ID, "myTextureSampler");
	
	// Get a handle for our buffers
	GLuint squareVerticesID = glGetAttribLocation(particleProg.ID, "squareVertices");
	GLuint xyzsID = glGetAttribLocation(particleProg.ID, "xyzs");
	GLuint colorID = glGetAttribLocation(particleProg.ID, "color");   

	static GLfloat* ParticlePosition = new GLfloat[MaxParticles * 4];
	static GLubyte* ParticleColor = new GLubyte[MaxParticles * 4];

	for(int i=0; i<MaxParticles; i++){
		ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
	}

	static GLfloat* RainPosition = new GLfloat[MaxParticles * 4];
	static GLubyte* RainColor         = new GLubyte[MaxParticles * 4];

	for(int i=0; i<RainMaxParticles; i++){
		RainContainer[i].life = -1.0f;
		RainContainer[i].cameradistance = -1.0f;
	}

	static GLfloat* SplashPosition = new GLfloat[MaxParticles * 4];
	static GLubyte* SplashColor         = new GLubyte[MaxParticles * 4];

	for(int i=0; i<SplashMaxParticles; i++){
		SplashContainer[i].life = -1.0f;
		SplashContainer[i].cameradistance = -1.0f;
	}
	
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


	ImageLoader texture1("metal.jpg");
	ImageLoader texture2("white.jpg");
	ImageLoader texture3("gold.jpg");
	ImageLoader texture4("pink.jpg");
	ImageLoader texture5("awesomeface.jpg");
	ImageLoader textureBlack("black.png");
	ImageLoader particleTexture("smoke.bmp");


	prog.use();
	prog.setInt("texture1", 0);
	prog.setInt("texture2", 0);
	prog.setInt("texture3", 0);
	prog.setInt("texture4", 0);
	prog.setInt("texture5", 0);
	particleProg.setInt("myTextureSampler", 0); //masih gatau
	lampShader.setInt("texture2", 0);

	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	static const GLfloat g_vertex_buffer_data[] = { 
		 -0.1f, -0.1f, -0.1f,
		  0.1f, -0.1f, 0.1f,
		 -0.1f,  0.1f, 0.1f,
		  0.1f,  0.1f, 0.1f,
	};
	GLuint billboard_vertex_buffer;
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	GLuint particles_position_buffer;
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	GLuint particles_color_buffer;
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);


	unsigned int modelLoc = glGetUniformLocation(prog.ID, "model");
	unsigned int viewLoc = glGetUniformLocation(prog.ID, "view");
	unsigned int projectionLoc = glGetUniformLocation(prog.ID, "projection");

	bool naik = true;
	bool naik2 = true;
	float arrOfLightPos[3] = {-5, 4, 0};

	float angle = 0;
	double lastTime = glfwGetTime();
	double lastTimeFPS = glfwGetTime();
	int nbFrames = 0;

	while (!glfwWindowShouldClose(window))
	{
		
		processInput(window);
		glClearColor(0.7f, 0.7f, 0.7f, 0.7f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Count time
		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;

		// FPS
		nbFrames++;
		if (currentTime - lastTimeFPS >= 1.0) {
			printf("FPS : %f (%f ms/frame)\n", double(nbFrames), 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTimeFPS += 1.0; 
		}

		if(naik) {
			if(arrOfLightPos[0] >= 5) {
				arrOfLightPos[0] -= 0.001;
				naik = false;
				if(naik2) {
					if(arrOfLightPos[2] >= 5) {
						arrOfLightPos[2] -= 0.001;
						naik2 = false;
					} else {
						arrOfLightPos[2] += 0.002;
					}
				} else {
					if(arrOfLightPos[2] <= -5) {
						arrOfLightPos[2] += 0.001;
						naik2 = true;
					} else {
						arrOfLightPos[2] -= 0.002;
					}
				}
			} else {
				arrOfLightPos[0] += 0.001;
				if(naik2) {
					if(arrOfLightPos[2] >= 5) {
						arrOfLightPos[2] -= 0.001;
						naik2 = false;
					} else {
						arrOfLightPos[2] += 0.002;
					}
				} else {
					if(arrOfLightPos[2] <= -5) {
						arrOfLightPos[2] += 0.001;
						naik2 = true;
					} else {
						arrOfLightPos[2] -= 0.002;
					}
				}
			}
		} else {
			if(arrOfLightPos[0] <= -5) {
				arrOfLightPos[0] += 0.001;
				naik = true;
				if(naik2) {
					if(arrOfLightPos[2] >= 5) {
						arrOfLightPos[2] -= 0.001;
						naik2 = false;
					} else {
						arrOfLightPos[2] += 0.002;
					}
				} else {
					if(arrOfLightPos[2] <= -5) {
						arrOfLightPos[2] += 0.001;
						naik2 = false;
					} else {
						arrOfLightPos[2] -= 0.002;
					}
				}
			} else {
				arrOfLightPos[0] -= 0.001;
				if(naik2) {
					if(arrOfLightPos[2] >= 5) {
						arrOfLightPos[2] -= 0.001;
						naik2 = false;
					} else {
						arrOfLightPos[2] += 0.002;
					}
				} else {
					if(arrOfLightPos[2] <= -5) {
						arrOfLightPos[2] += 0.001;
						naik2 = false;
					} else {
						arrOfLightPos[2] -= 0.002;
					}
				}
			}
		}

		float radius = 11.0f;
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		
		glm::mat4 projection = glm::mat4(1.0f);;
		view = glm::lookAt(cameraFront * radius, glm::vec3(0.0, 0.0, 0.0), cameraUp);
		// view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); 

		// pass them to the shaders (3 different ways)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		// TODO: lampu belum ke render
		
		//DRAW SMOKE
		int newparticles = (int)(delta*10000.0);
		if (newparticles > (int)(0.016f*10000.0))
			newparticles = (int)(0.016f*10000.0);
		
		for(int i=0; i<newparticles; i++){
			int particleIndex = FindUnusedParticle();
			ParticlesContainer[particleIndex].life = 0.5f; // This particle will live 5 seconds.
			ParticlesContainer[particleIndex].pos = glm::vec3(1.2f,-1.5f, 3.55f);

			float spread = 1.5f;
			glm::vec3 maindir = glm::vec3(0.0f, 0.0f, 8.0f);
			// Very bad way to generate a random direction; 
			// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
			// combined with some user-controlled parameters (main direction, spread, etc)
			glm::vec3 randomdir = glm::vec3(
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f
			);
			
			ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;


			// // Very bad way to generate a random color
			ParticlesContainer[particleIndex].r = 150;
			ParticlesContainer[particleIndex].g = 150;
			ParticlesContainer[particleIndex].b = 150;
			ParticlesContainer[particleIndex].a = 70;

			ParticlesContainer[particleIndex].size = (rand()%1000)/2000.0f + 0.1f;
			
		}

		// Simulate all particles
		int ParticlesCount = 0;
		for(int i=0; i<MaxParticles; i++){

			Particle& p = ParticlesContainer[i]; // shortcut

			if(p.life > 0.0f){

				// Decrease life
				p.life -= delta;
				if (p.life > 0.0f){

					// Simulate simple physics : gravity only, no collisions
					p.speed += glm::vec3(0.0f, 40.0f, -10.0f) * (float)delta * 0.5f;
					p.pos += p.speed * (float)delta;
					p.cameradistance = glm::length2( p.pos - cameraPos );
					//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

					// Fill the GPU buffer
					ParticlePosition[4*ParticlesCount+0] = p.pos.x;
					ParticlePosition[4*ParticlesCount+2] = p.pos.z;
					ParticlePosition[4*ParticlesCount+1] = p.pos.y;
												   
					ParticlePosition[4*ParticlesCount+3] = p.size;
												   
					ParticleColor[4*ParticlesCount+0] = p.r;
					ParticleColor[4*ParticlesCount+1] = p.g;
					ParticleColor[4*ParticlesCount+2] = p.b;
					ParticleColor[4*ParticlesCount+3] = p.a;

				}else{
					// Particles that just died will be put at the end of the buffer in SortParticles();
					p.cameradistance = -1.0f;
				}

				ParticlesCount++;

			}
		}

		SortParticles(ParticlesContainer, MaxParticles); // for testing

		// Update the buffers that OpenGL uses for rendering.
		// There are much more sophisticated means to stream data from the CPU to the GPU, 
		// but this is outside the scope of this tutorial.
		// http://www.opengl.org/wiki/Buffer_Object_Streaming
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, ParticlePosition);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, ParticleColor);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		particleProg.use();

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		particleTexture.use();
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// Same as the billboards tutorial
		glUniform3f(CameraRight_worldspace_ID, view[0][0], view[1][0], view[2][0]);
		glUniform3f(CameraUp_worldspace_ID   , view[0][1], view[1][1], view[2][1]);

		glm::mat4 ViewProjectionMatrix = projection * view;

		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

        
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(squareVerticesID);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
			squareVerticesID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		
		// 2nd attribute buffer : positions of particles' centers
		glEnableVertexAttribArray(xyzsID);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glVertexAttribPointer(
			xyzsID,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : particles' colors
		glEnableVertexAttribArray(colorID);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glVertexAttribPointer(
			colorID,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// These functions are specific to glDrawArrays*Instanced*.
		// The first parameter is the attribute buffer we're talking about.
		// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
		// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
		glVertexAttribDivisorARB(squareVerticesID, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisorARB(xyzsID, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisorARB(colorID, 1); // color : one per quad                                  -> 1

		// Draw the particules !
		// This draws many times a small triangle_strip (which looks like a quad).
		// This is equivalent to :
		// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
		// but faster.
		glDrawArraysInstancedARB(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

		glDisableVertexAttribArray(squareVerticesID);
		glDisableVertexAttribArray(xyzsID);
		glDisableVertexAttribArray(colorID);

		//RAIN
		int newRain = (int)(delta*10000.0);
		if (newRain > (int)(0.016f*10000.0))
			newRain = (int)(0.016f*10000.0);
		
		for(int i=0; i<newRain; i++){
			int rainIndex = FindUnusedRain();
			RainContainer[rainIndex].life = 2.0f; // This particle will live 5 seconds.
			RainContainer[rainIndex].pos = glm::vec3(
				rand() % (3 - (-3) + 1) + (-3), 
				6.0f,
				rand() % (4 - (-6) + 1) + (-6)
			);

			float spread = 1.5f;
			glm::vec3 maindir = glm::vec3(0.0f, -8.0f, 0.0f);
			// Very bad way to generate a random direction; 
			// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
			// combined with some user-controlled parameters (main direction, spread, etc)
			glm::vec3 randomdir = glm::vec3(
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f
			);
			
			RainContainer[rainIndex].speed = maindir + randomdir*spread;


			// // Very bad way to generate a random color
			RainContainer[rainIndex].r = 0;
			RainContainer[rainIndex].g = 0;
			RainContainer[rainIndex].b = 255;
			RainContainer[rainIndex].a = 70;

			RainContainer[rainIndex].size = 0.3f; // (rand()%1000)/2000.0f + 0.1f;
			
		}



		// Simulate all particles
		int RainCount = 0;
		for(int i=0; i<RainMaxParticles; i++){

			Particle& p = RainContainer[i]; // shortcut

			if(p.life > 0.0f){

				// Decrease life
				p.life -= delta;
				if (p.life > 0.0f){

					// Simulate simple physics : gravity only, no collisions
					p.speed += glm::vec3(0.0f, 0.0f, 0.0f) * (float)delta * 0.5f;
					p.pos += p.speed * (float)delta;
					p.cameradistance = glm::length2( p.pos - cameraPos );
					//RainContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

					// Fill the GPU buffer
					RainPosition[4*RainCount+0] = p.pos.x;
					RainPosition[4*RainCount+1] = p.pos.y;
					RainPosition[4*RainCount+2] = p.pos.z;
												   
					RainPosition[4*RainCount+3] = p.size;
												   
					RainColor[4*RainCount+0] = p.r;
					RainColor[4*RainCount+1] = p.g;
					RainColor[4*RainCount+2] = p.b;
					RainColor[4*RainCount+3] = p.a;

					//collision
					if(p.pos.y <= -1.8f) {
						// printf("masuk sini \n");
						int splashParticleIndex = FindUnusedSplash();
						SplashContainer[splashParticleIndex].life = 0.05f;
						SplashContainer[splashParticleIndex].pos = glm::vec3(p.pos.x, p.pos.y, p.pos.z);
						// SplashContainer[splashParticleIndex].speed = glm::vec3(0.0f, 0.0f, 0.0f);
						// Random color
						SplashContainer[splashParticleIndex].r = 0.0f;
						SplashContainer[splashParticleIndex].g = 0.0f;
						SplashContainer[splashParticleIndex].b = 150.0f;
						SplashContainer[splashParticleIndex].a = 150.0f;
						SplashContainer[splashParticleIndex].size = 0.5f;
						
						// kill the raindrops
						RainContainer[i].life = -0.1f;
					}

				} else{
					// Particles that just died will be put at the end of the buffer in SortParticles();
					p.cameradistance = -1.0f;
				}

				RainCount++;

			}
		}

		SortParticles(RainContainer, RainMaxParticles); // for testing

		// Update the buffers that OpenGL uses for rendering.
		// There are much more sophisticated means to stream data from the CPU to the GPU, 
		// but this is outside the scope of this tutorial.
		// http://www.opengl.org/wiki/Buffer_Object_Streaming
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, RainMaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, RainCount * sizeof(GLfloat) * 4, RainPosition);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, RainMaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, RainCount * sizeof(GLubyte) * 4, RainColor);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		particleProg.use();

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		particleTexture.use();
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// Same as the billboards tutorial
		glUniform3f(CameraRight_worldspace_ID, view[0][0], view[1][0], view[2][0]);
		glUniform3f(CameraUp_worldspace_ID   , view[0][1], view[1][1], view[2][1]);

		ViewProjectionMatrix = projection * view;

		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

        
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(squareVerticesID);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
			squareVerticesID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		
		// 2nd attribute buffer : positions of particles' centers
		glEnableVertexAttribArray(xyzsID);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glVertexAttribPointer(
			xyzsID,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : particles' colors
		glEnableVertexAttribArray(colorID);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glVertexAttribPointer(
			colorID,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// These functions are specific to glDrawArrays*Instanced*.
		// The first parameter is the attribute buffer we're talking about.
		// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
		// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
		glVertexAttribDivisorARB(squareVerticesID, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisorARB(xyzsID, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisorARB(colorID, 1); // color : one per quad                                  -> 1

		// Draw the particules !
		// This draws many times a small triangle_strip (which looks like a quad).
		// This is equivalent to :
		// for(i in RainCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
		// but faster.
		glDrawArraysInstancedARB(GL_TRIANGLE_STRIP, 0, 4, RainCount);

		glDisableVertexAttribArray(squareVerticesID);
		glDisableVertexAttribArray(xyzsID);
		glDisableVertexAttribArray(colorID);

		//SPLASH
		// Simulate all particles
		int splashParticlesCount = 0;
		for (int i = 0; i < SplashMaxParticles; i++) {
			Particle& p = SplashContainer[i];
			if (p.life > 0.0f) {
				// Decrease life
				p.life -= delta;
				if (p.life > 0.0f) {
					p.cameradistance = glm::length2(p.pos - cameraPos);
					// Fill the GPU buffer
					SplashPosition[4 * splashParticlesCount + 0] = p.pos.x;
					SplashPosition[4 * splashParticlesCount + 1] = p.pos.y;
					SplashPosition[4 * splashParticlesCount + 2] = p.pos.z;
					SplashPosition[4 * splashParticlesCount + 3] = p.size;
					SplashColor[4 * splashParticlesCount + 0] = p.r;
					SplashColor[4 * splashParticlesCount + 1] = p.g;
					SplashColor[4 * splashParticlesCount + 2] = p.b;
					SplashColor[4 * splashParticlesCount + 3] = p.a;
				}
				else {
					// Particles that just died will be put at the end of the buffer in SortParticles()
					p.cameradistance = -1.0f;
				}
				splashParticlesCount++;
			}
		}
		SortParticles(SplashContainer, SplashMaxParticles);

		// Update the buffers that OpenGL uses for rendering.
		// There are much more sophisticated means to stream data from the CPU to the GPU, 
		// but this is outside the scope of this tutorial.
		// http://www.opengl.org/wiki/Buffer_Object_Streaming
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, SplashMaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, splashParticlesCount * sizeof(GLfloat) * 4, SplashPosition);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, SplashMaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, splashParticlesCount * sizeof(GLubyte) * 4, SplashColor);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		particleProg.use();

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		particleTexture.use();
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// Same as the billboards tutorial
		glUniform3f(CameraRight_worldspace_ID, view[0][0], view[1][0], view[2][0]);
		glUniform3f(CameraUp_worldspace_ID   , view[0][1], view[1][1], view[2][1]);

		ViewProjectionMatrix = projection * view;

		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

        
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(squareVerticesID);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
			squareVerticesID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		
		// 2nd attribute buffer : positions of particles' centers
		glEnableVertexAttribArray(xyzsID);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glVertexAttribPointer(
			xyzsID,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : particles' colors
		glEnableVertexAttribArray(colorID);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glVertexAttribPointer(
			colorID,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// These functions are specific to glDrawArrays*Instanced*.
		// The first parameter is the attribute buffer we're talking about.
		// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
		// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
		glVertexAttribDivisorARB(squareVerticesID, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisorARB(xyzsID, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisorARB(colorID, 1); // color : one per quad                                  -> 1

		// Draw the particules !
		// This draws many times a small triangle_strip (which looks like a quad).
		// This is equivalent to :
		// for(i in RainCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
		// but faster.
		glDrawArraysInstancedARB(GL_TRIANGLE_STRIP, 0, 4, RainCount);

		glDisableVertexAttribArray(squareVerticesID);
		glDisableVertexAttribArray(xyzsID);
		glDisableVertexAttribArray(colorID);

		//LIGHT AND CAR

		glm::vec3 lightPos(arrOfLightPos[0], arrOfLightPos[1], arrOfLightPos[2]);
		//bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		texture3.use();
		glActiveTexture(GL_TEXTURE0);
		texture4.use();
		glActiveTexture(GL_TEXTURE1);
		texture3.use();
		
		// be sure to activate shader when setting uniforms/drawing objects
        prog.use();
        prog.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        prog.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        prog.setVec3("lightPos", lightPos);
        prog.setVec3("viewPos", glm::vec3(0.0, 0.0, 0.0));

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

		//Render Knalpot
		glActiveTexture(GL_TEXTURE0);
		textureBlack.use();
		prog.use();
		DrawKnalpot(prog);

		//Render Lampu
		lampShader.use();
		texture2.use();
        lampShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        lampShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lampShader.setVec3("lightPos", lightPos);
        lampShader.setVec3("viewPos", glm::vec3(0.0, 0.0, 0.0));
		DrawLampu(lampShader, lightPos);
		glBindVertexArray(lightVAO);
		// ending
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

void DrawKnalpot(Shader& ourShader) {
	glm::vec3 cubePositions[] = {
		glm::vec3(1.2f,  -1.5f, 3.4f),
	};
	glm::vec3 cubeScales[] = {
		glm::vec3(0.35f,  0.25f, 0.3f),
	};
	glEnable(GL_DEPTH);
	// calculate the model matrix for each object and pass it to shader before drawing
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, cubePositions[0]);
	model = glm::scale(model, cubeScales[0]);
	ourShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void DrawLampu(Shader& ourShader, glm::vec3 position) {
	glEnable(GL_DEPTH);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(1.0f,  1.0f, 1.0f));
	ourShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
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