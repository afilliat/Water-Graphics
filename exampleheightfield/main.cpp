/*

  BASIC GLFW + GLXW WINDOW AND OPENGL SETUP 
  ------------------------------------------
  See https://gist.github.com/roxlu/6698180 for the latest version of the example.

*/


#include <iostream>
#include <vector>

#if defined(__linux)
#  include <GLXW/glxw.h>
#endif

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#define ROXLU_USE_OPENGL
#define ROXLU_USE_MATH
#include <tinylib.h>

#include "HeightField.h"
HeightField height_field;

void cursor_callback(GLFWwindow* win, double x, double y);
void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
void error_callback(int err, const char* desc);
void resize_callback(GLFWwindow* window, int width, int height);

int main() {

  glfwSetErrorCallback(error_callback);

  if(!glfwInit()) {
    printf("error: cannot setup glfw.\n");
    return false;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow* win = NULL;
  int w = 1280;
  int h = 720;

  win = glfwCreateWindow(w, h, "HeightField", NULL, NULL);
  if(!win) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetFramebufferSizeCallback(win, resize_callback);
  glfwSetKeyCallback(win, key_callback);
  glfwSetCursorPosCallback(win, cursor_callback);
  glfwMakeContextCurrent(win);
  glfwSwapInterval(1);

#if defined(__linux)  
  if(glxwInit() != 0) {
    printf("error: cannot initialize glxw.\n");
    ::exit(EXIT_FAILURE);
  }

#endif

  // ----------------------------------------------------------------
  // THIS IS WHERE YOU START CALLING OPENGL FUNCTIONS, NOT EARLIER!!
  // ----------------------------------------------------------------

  if(!height_field.setup()) {
    printf("Canot setup the height field.\n");
    ::exit(EXIT_FAILURE);
  }


  while(!glfwWindowShouldClose(win)) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    height_field.diffuse();
    height_field.calculateNormals();
    height_field.drawTexture(height_field.tex_u0, 0.0, 0.0, 320.0, 240.0);
    height_field.drawTexture(height_field.tex_v0, 320.0, 0.0, 320.0, 240.0);
    height_field.render();

    glfwSwapBuffers(win);
    glfwPollEvents();
  }


  glfwTerminate();

  return EXIT_SUCCESS;
}

void error_callback(int err, const char* desc) {
  printf("glfw error: %s (%d)\n", desc, err);
}

void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods) {
  
  if(action != GLFW_PRESS) {
    return;
  }


  switch(key) {
    case GLFW_KEY_1: {
      break;
    }
    case GLFW_KEY_SPACE: {
      // water.testFlowFieldForce();
#if USE_WATER
      water.addDrop(64, 64);
#endif
      break;
    }
    case GLFW_KEY_LEFT: {
      break;
    }
    case GLFW_KEY_RIGHT: {
      break;
    }
    case GLFW_KEY_ESCAPE: {
      glfwSetWindowShouldClose(win, GL_TRUE);
      break;
    }
  };
  
}

void cursor_callback(GLFWwindow* win, double x, double y) {
#if USE_WATER  
  int b = glfwGetMouseButton(win, 0);
  if(b) {
    water.addDrop(  (y/720.0) * FIELD_N, <(x/1280.0) * FIELD_N ) ;
  }
#endif
}

void resize_callback(GLFWwindow* window, int width, int height) {
  
}
