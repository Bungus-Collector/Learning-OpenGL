/*
Assignment 2, created by:
Gabriel Lagacé, 40236989
Peter Howe 40237623
Chistopher Puran 40006107
*/

#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm.hpp"
#include "mat4x4.hpp"
#include "ext/matrix_transform.hpp"

//Window dimensions
#define HEIGHT 800
#define WIDTH 800

#define TRIANGLE_HEIGHT 0.1f
#define TRIANGLE_WIDTH 0.1f

#define TRANSFORM_MOD 0.01f
#define ANGLE_MOD 0.05f
#define SCALE_MOD 0.05f

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
    out vec4 FragColour;
    uniform vec4 vertexColour;

    void main()
    {
        FragColour = vertexColour;
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

    //Start up glew
    GLenum err = glewInit();

    //Vertices of the (equilateral) triangle
    GLfloat vertices[]{
        (TRIANGLE_WIDTH / 2),   -(TRIANGLE_HEIGHT / (2*sqrt(3))),   0.0f, //bottom left
        -(TRIANGLE_WIDTH / 2),  -(TRIANGLE_HEIGHT / (2*sqrt(3))),   0.0f, //bottom right
        0.0f,                   (TRIANGLE_HEIGHT / (sqrt(3))),      0.0f //top
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
    //VAO stores pointers to VBO data and tells OpenGL how to interpret them
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); //1 represents the number of objects stored by the VBO

    //Binding is sort of like making this a global variable that will be modified whenever functions are called on that particular buffer (I think)
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //Storing vertex data in the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Defining viewport
    glViewport(0, 0, WIDTH, HEIGHT);

    //Changing the background colour
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window); //Important to swap buffers after a change so the window actually updates

    //Values for transformations
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float theta = 0.0f;
    float scale = 1.0f;

    //Colours!!!
    float colourMod = 1.0f;
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;

    //Main render loop
    while (!glfwWindowShouldClose(window)) {
        //Processing input (just for the escape key, all the other inputs are just handled in this loop)
        processInput(window);

        //Sets refresh rate to 60 fps
        glfwSwapInterval(1);

        //Refreshing background colour
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Transformation matrix
        glm::mat4x4 rotation_matrix = glm::mat4x4(
            glm::vec4(cos(theta),   -sin(theta),    0.0f, 0.0f),
            glm::vec4(sin(theta),   cos(theta),     0.0f, 0.0f),
            glm::vec4(0.0f,         0.0f,           1.0f, 0.0f),
            glm::vec4(0.0f,         0.0f,           0.0f, 1.0f)
        );

        glm::mat4x4 scaling_matrix = glm::mat4x4(
            glm::vec4(scale,    0.0f,   0.0f, 0.0f),
            glm::vec4(0.0f,     scale,  0.0f, 0.0f),
            glm::vec4(0.0f,     0.0f,   1.0f, 0.0f),
            glm::vec4(0.0f,     0.0f,   0.0f, 1.0f)
        );

        glm::mat4x4 translation_matrix = glm::mat4x4(
            glm::vec4(1.0f,     0.0f,       0.0f, 0.0f),
            glm::vec4(0.0f,     1.0f,       0.0f, 0.0f),
            glm::vec4(0.0f,     0.0f,       1.0f, 0.0f),
            glm::vec4(offsetX,  offsetY,    0.0f, 1.0f)
        );

        // Combine matrices to make final MVP
        glm::mat4x4 MVP = translation_matrix * rotation_matrix * scaling_matrix;

        //Inputs
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            if ((TRIANGLE_WIDTH / 2) * scale + offsetX + TRANSFORM_MOD < 1) {
                offsetX += TRANSFORM_MOD;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            if (-(TRIANGLE_WIDTH / 2) * scale + offsetX - TRANSFORM_MOD > -1) {
                offsetX -= TRANSFORM_MOD;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            if ((TRIANGLE_HEIGHT / sqrt(3)) * scale + offsetY + TRANSFORM_MOD < 1) {
                offsetY += TRANSFORM_MOD;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            if (-(TRIANGLE_HEIGHT / (2*sqrt(3))) * scale + offsetY - TRANSFORM_MOD > -1) {
                offsetY -= TRANSFORM_MOD;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            theta += ANGLE_MOD;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            theta -= ANGLE_MOD;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && scale + SCALE_MOD < 20) {
            scale += SCALE_MOD;
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && scale - SCALE_MOD > 0.01) {
            scale -= SCALE_MOD;
        }

        //Passing stuff to shaders
        GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
        GLint ColourID = glGetUniformLocation(shaderProgram, "vertexColour");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniform4f(ColourID, r, g, b, 1.0f);

        //Cycling colours
        r = sin(colourMod) / 2 + 0.5;
        g = cos(colourMod) / 2 + 0.5;
        b = -cos(colourMod) / 2 + 0.5;
        colourMod += 0.05f;

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
