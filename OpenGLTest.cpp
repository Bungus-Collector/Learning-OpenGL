#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

/*
* Code is basically just copied from this tutorial series with my own comments:
* https://www.youtube.com/watch?v=hYZNN0MTLuc&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-&index=3
* The videos have been helpful so far but a few adjustments have been made since the tutorial
* guy uses glad and we're using glew.
*/

//Copy and pasted vertex and fragment shader source in gl shading language
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)glsl";
//FragColor is what determines the triangle colour
const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
)glsl";

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
    GLFWwindow* window = glfwCreateWindow(800, 800, "Window", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -2;
    }
    glfwMakeContextCurrent(window);

    //Start up glew, must be done after making the current context or things break
    GLenum err = glewInit();

    //Vertices of the triangle
    GLfloat vertices[]{
        -0.5f, -0.5f, 0.0f, //bottom left
        0.5f, -0.5f, 0.0f, //bottom right
        0.0f, 0.5f, 0.0f //top
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
    glViewport(0, 0, 800, 800);

    //Changing the background colour
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window); //Important to swap buffers after a change so the window actually updates

    //Main render loop
    while (!glfwWindowShouldClose(window)) {
        //Refreshing background colour
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
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
