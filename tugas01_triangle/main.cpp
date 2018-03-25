#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// This is the callback we'll be registering with GLFW for errors.
// It'll just print out the error to the STDERR stream.
void error_callback(int error, const char *description) {
  fputs(description, stderr);
}

// This is the callback we'll bse registering with GLFW for keyboard handling.
// The only thing we're doing here is setting up the window to close when we
// press ESC
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

int main(void) {
  GLFWwindow *window;

  glfwSetKeyCallback(window, key_callback);
  glfwSetErrorCallback(error_callback);

  // Initialize the library
  if (!glfwInit()) {
    return -1;
  }

  // Create a windowed mode window and its OpenGL context
  window =
      glfwCreateWindow(640, 480, "OpenGL | Triangle (No Shader)", NULL, NULL);

  if (!window) {
    glfwTerminate();
    return -1;
  }

  // Make the window's context current
  glfwMakeContextCurrent(window);

  float vertices[] = {
      0,    0.5,  0.0, // top corner
      -0.5, -0.5, 0.0, // bottom left corner
      0.5,  -0.5, 0.0  // bottom right corner
  };

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Render OpenGL here

    glEnableClientState(GL_VERTEX_ARRAY); // tell OpenGL that you're using a
                                          // vertex array for fixed-function
                                          // attribute
    glVertexPointer(3, GL_FLOAT, 0,
                    vertices);             // point to the vertices to be used
    glDrawArrays(GL_TRIANGLES, 0, 3);      // draw the vertixes
    glDisableClientState(GL_VERTEX_ARRAY); // tell OpenGL that you're finished
                                           // using the vertex arrayattribute

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}