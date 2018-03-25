#ifndef ENGINE_H
#define ENGINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

using namespace std;

class Engine {
public:
  Engine(int, int, string, function<void()> = []() -> void {});
  ~Engine();
  static void init(int = 800, int = 600, string = "OpenGL",
                   function<void()> = []() -> void {});
  static unique_ptr<Engine> &instance();
  static GLfloat bgColor[];
  void setKeyCallback(GLFWkeyfun);
  void setErrorCallback(GLFWerrorfun);
  void destroy();
  void loop(function<void()>);
  void loadShader(const string, const string,
                  function<void()> = []() -> void {});
  void loadVerticeBuffer(float[], function<void()> = []() -> void {});

  GLuint program;
  GLuint vao; // vertex array object
  GLuint vbo; // vertex buffer object
  GLFWwindow *window;

private:
  static unique_ptr<Engine> pointerInstance;

  void initOpenGL(function<void()> = []() -> void {});

  int height = 600;
  int width = 800;
  string title = "OpenGL";

  unsigned int vertex;
  unsigned int fragment;

  void setBool(const std::string &, bool) const;
  void setFloat(const std::string &, float) const;
  void setInt(const std::string &, int) const;

  GLFWerrorfun errorCallback = nullptr;
  GLFWkeyfun keyCallback = nullptr;
};

#endif
