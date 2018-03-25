#include "Engine.hpp"

unique_ptr<Engine> Engine::pointerInstance = nullptr;

unique_ptr<Engine> &Engine::instance() { return pointerInstance; }

GLfloat Engine::bgColor[] = {0.0f, 0.25f, 0.0f};

Engine::Engine(int width, int height, string title, function<void()> func) {
  this->width = width;
  this->height = height;
  this->title = title;
  this->initOpenGL(func);
}

Engine::~Engine() {}

void Engine::init(int width, int length, string title, function<void()> func) {
  pointerInstance = make_unique<Engine>(width, length, title, func);
}

void Engine::setKeyCallback(GLFWkeyfun callback) {
  keyCallback = callback;
  glfwSetKeyCallback(window, this->keyCallback);
}

void Engine::setErrorCallback(GLFWerrorfun callback) {
  errorCallback = callback;
  glfwSetErrorCallback(this->errorCallback);
}

void Engine::initOpenGL(function<void()> func) {
  // Initialize GLFW, and if it fails to initialize for any reason, print it out
  // to STDERR.
  if (!glfwInit()) {
    fprintf(stderr, "Failed initialize GLFW.");
    exit(EXIT_FAILURE);
  }

  // Set up OpenGL options.
  // Use OpenGL verion 4.1
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  // GLFW_OPENGL_FORWARD_COMPAT specifies whether the OpenGL context should be
  // forward-compatible, i.e. one where all functionality deprecated in the
  // requested version of OpenGL is removed.
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  // Indicate we only want the newest core profile, rather than using backwards
  // compatible and deprecated features.
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Make the window resize-able.
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

  // Create a window to put our stuff in.
  window = glfwCreateWindow(height, width, title.c_str(), NULL, NULL);

  // If the window fails to be created, print out the error, clean up GLFW and
  // exit the program.
  if (!window) {
    fprintf(stderr, "Failed to create GLFW window.");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // Use the window as the current context (everything that's drawn will be
  // place in this window).
  glfwMakeContextCurrent(window);

  printf("OpenGL version supported by this platform (%s): \n",
         glGetString(GL_VERSION));

  // Makes sure all extensions will be exposed in GLEW and initialize GLEW.
  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    cout << "Error GLEW Init" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  func();
}

void Engine::loadVerticeBuffer(float vertices[], function<void()> func) {
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(&vertices), vertices, GL_STATIC_DRAW);
  func();
}

void Engine::loop(function<void()> func) {
  while (!glfwWindowShouldClose(window)) {
    glClearBufferfv(GL_COLOR, 0, bgColor);

    func();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Engine::destroy() {
  // Program clean up when the window gets closed.
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  glDeleteProgram(program);
  glfwTerminate();
}

void Engine::loadShader(const string vertexPath, const string fragmentPath,
                        function<void()> func) {
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }

  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  int success;
  char infoLog[512];

  // Compile vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX:COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT:COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  // shader program
  program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  // Print linking error
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  glUseProgram(program);

  func();
}

void Engine::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}

void Engine::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Engine::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}