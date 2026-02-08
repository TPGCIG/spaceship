#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h> // Replaces the old framework
#include <algorithm>
#include <glad/glad.h> // Replaces glload
#include <iostream>
#include <vector>

GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile) {
  GLuint shader = glCreateShader(eShaderType);
  const char *strFileData = strShaderFile.c_str();
  glShaderSource(shader, 1, &strFileData, NULL);

  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar *strInfoLog = new GLchar[infoLogLength + 1];
    glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

    const char *strShaderType = NULL;
    switch (eShaderType) {
    case GL_VERTEX_SHADER:
      strShaderType = "vertex";
      break;
    case GL_GEOMETRY_SHADER:
      strShaderType = "geometry";
      break;
    case GL_FRAGMENT_SHADER:
      strShaderType = "fragment";
      break;
    }

    fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType,
            strInfoLog);
    delete[] strInfoLog;
  }

  return shader;
}

GLuint CreateProgram(const std::vector<GLuint> &shaderList) {
  GLuint program = glCreateProgram();

  for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
    glAttachShader(program, shaderList[iLoop]);

  glLinkProgram(program);

  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar *strInfoLog = new GLchar[infoLogLength + 1];
    glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
    fprintf(stderr, "Linker failure: %s\n", strInfoLog);
    delete[] strInfoLog;
  }

  for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
    glDetachShader(program, shaderList[iLoop]);

  return program;
}

GLuint theProgram;

const std::string strVertexShader("#version 330\n"
                                  "layout(location = 0) in vec4 position;\n"
                                  "void main()\n"
                                  "{\n"
                                  "   gl_Position = position;\n"
                                  "}\n");

const std::string
    strFragmentShader("#version 330\n"
                      "out vec4 outputColor;\n"
                      "void main()\n"
                      "{\n"
                      "   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
                      "}\n");

void InitializeProgram() {
  std::vector<GLuint> shaderList;

  shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertexShader));
  shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

  theProgram = CreateProgram(shaderList);

  std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

const float vertexPositions[] = {
    1.75f, 1.75f, 0.0f,   1.0f,   1.75f, -1.75f,
    0.0f,  1.0f,  -1.75f, -1.75f, 0.0f,  1.0f,
};

GLuint positionBufferObject;
GLuint vao;

void InitializeVertexBuffer() {
  glGenBuffers(1, &positionBufferObject);

  glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Called after the window and OpenGL are initialized. Called exactly once,
// before the main loop.
void init() {
  InitializeProgram();
  InitializeVertexBuffer();

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
}

// Called to update the display.
// You should call glutSwapBuffers after all of your rendering to display what
// you rendered. If you need continuous updates of the screen, call
// glutPostRedisplay() at the end of the function.
void display(GLFWwindow *window) {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(theProgram);

  glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(0);
  glUseProgram(0);

  glfwSwapBuffers(window); // Replaced glutSwapBuffers
}

void reshape(GLFWwindow *window, int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

int main() {
  // 1. Initialize GLFW
  if (!glfwInit())
    return -1;

  // 2. Create a window
  GLFWwindow *window = glfwCreateWindow(500, 500, "Tutorial 01", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, reshape);

  // 3. Initialize GLAD (The Loader)
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // 4. Call the Tutorial's init functions
  init();

  // 5. Main Loop
  while (!glfwWindowShouldClose(window)) {
    display(window);
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  }

  glfwTerminate();
  return 0;
}
