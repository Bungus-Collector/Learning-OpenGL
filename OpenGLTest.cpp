#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_transform.hpp"

//Window dimensions
#define HEIGHT 800
#define WIDTH 800

//MVP is the passed model-view-position matrix for moving verticies around
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 MSpos;
    uniform mat4 MVP;

    void main() {
        vec4 v = vec4(MSpos,1); 
        gl_Position = MVP * v;
    }
)glsl";

//FragColor is what determines the triangle colour
const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0f, 0.8f, 0.0f, 1.0f);
    }
)glsl";

//Function for closing the window when the escape key is pressed
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

//Error callback function
static void glfwError(int id, const char* description)
{
    std::cout << description << std::endl;
}

int main()
{
    //Setting error callback function
    glfwSetErrorCallback(&glfwError);

    //Starts up glfw
    if (!glfwInit()) {
        return -1;
    }

    //Tell GLFW what version of OpenGL we're using
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //Core profile means only modern functions are available
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Creating the window and creating the current context
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "the triangle", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -2;
    }
    glfwMakeContextCurrent(window);

    //Start up glew, must be done after making the current context or things break
    GLenum err = glewInit();

    //Vertices of the triangle
    GLfloat vertices[]{
        -0.05f, -0.05f, 0.0f, //bottom left
        0.05f, -0.05f, 0.0f, //bottom right
        0.0f, 0.05f, 0.0f //top
    };

    //Setting up the shader program with the glsl code above
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //Now that the shaders have been linked their objects can be deleted
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //Vertex array object and vertex buffer object:
    //VBO stores vertex data
    //VAO stores pointers to VBOs and tells OpenGL how to interpret them (good for switching between multiple VBOs);
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); //1 represents the number of objects stored by the VBO

    //Binding is sort of like making this a global variable that will be modified whenever functions are called on that particular buffer (I think)
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //Storing vertex data in the VBO (which is being modified with GL_ARRAY_BUFFER because we bound it earlier)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //I'm not sure what this does but it's important
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Defining viewport
    glViewport(0, 0, WIDTH, HEIGHT);

    //Changing the background colour
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window); //Important to swap buffers after a change so the window actually updates

    //Values for modifying triangle position
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float theta = 0.1f;

    //Main render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        //Refreshing background colour
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Transformation matrix
        glm::mat4x4 MVP = glm::mat4x4(
            glm::vec4(cos(theta), 0.0f, -sin(theta), 0.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
            glm::vec4(sin(theta), 0.0f, cos(theta), 0.0f),
            glm::vec4(0.0f + offsetX, 0.0f + offsetY, 0.0f, 1.0f)
        );
        
        //Arrow inputs
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            offsetX += 0.001f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            offsetX -= 0.001f;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            offsetY += 0.001f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            offsetY -= 0.001f;
        }

        //Updating angle
        theta += 0.01f;

        //Pass transformation matrix to shader program
        GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        //Draw the triangle
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //Swap front and back buffer
        glfwSwapBuffers(window);

        //Handles events
        glfwPollEvents();
    }

    //Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    //Kill the window
    glfwDestroyWindow(window);

    //End glfw
    glfwTerminate();
    return 1;
}
