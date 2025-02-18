#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <string.h>

#include <cglm/cglm.h>

#define STB_IMAGE_IMPLEMENTATION
#include "./vendor/stb_image.h"

#include "utils.h"

#define ENABLE_LOGS
#include "debug.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define VIEWPORT_WIDTH WINDOW_WIDTH
#define VIEWPORT_HEIGHT WINDOW_HEIGHT

#define SHADER_ERROR_LOG_SIZE 512

#define t1 0
#define t2 1

#define TRIANGLE_COUNT 2

bool create_shader(unsigned int* shader_obj, GLenum shader_type, const char* path);

void check_program_linking(unsigned int shader_program);

void init(GLFWwindow** window);
void process_input(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void clean_up();


float vertices[TRIANGLE_COUNT][9] = {
    {
    -1.0f,-0.0f, 0.0f,
     0.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
    },
    {
     0.0f,-1.0f, 0.0f,
     0.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
    }
};  

// Now create the same 2 triangles using two different VAOs and VBOs for their data: solution.

int main(void)
{
    GLFWwindow* window;
    init(&window);

    unsigned int main_programs[TRIANGLE_COUNT];
    unsigned int vert_shader;
    unsigned int frag_shaders[TRIANGLE_COUNT];

    // vertex buffer object
    unsigned int VBOS[TRIANGLE_COUNT];
    // vertex array object (holds VBO configuration)
    unsigned int VAOS[TRIANGLE_COUNT];
    
    // 1. create buffers & VAO
    glGenBuffers(TRIANGLE_COUNT, VBOS);
    glGenVertexArrays(TRIANGLE_COUNT, VAOS);
    
    // 3. copy our data to buffers for OpenGL to use
    for (int i = 0; i < TRIANGLE_COUNT; i++)
    {
        // 2. bind VAO -> VAO then stores last bound GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER and vertex attribute configuration !! Cerefull VAO also stores unBindCalls !!
        glBindVertexArray(VAOS[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBOS[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[i]), vertices[i], GL_STATIC_DRAW);
        gl_check_error();

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        gl_check_error();

        // unbind VAO - úplně to to nemusí být, ale je to více clear
        glBindVertexArray(0);
    }

    #pragma region shader program creation
    my_assert(create_shader(&vert_shader, GL_VERTEX_SHADER, "./shaders/vertex.vert"), "failed to create VETEXE SHADER");
    my_assert(create_shader(frag_shaders+t1, GL_FRAGMENT_SHADER, "./shaders/t1.frag"), "failed to create FRAGMENT SHADER");
    my_assert(create_shader(frag_shaders+t2, GL_FRAGMENT_SHADER, "./shaders/t2.frag"), "failed to create FRAGMENT SHADER");
    
    
    main_programs[t1] = glCreateProgram();
    main_programs[t2] = glCreateProgram();

    // Attach shader stages
    glAttachShader(main_programs[t1], vert_shader);
    glAttachShader(main_programs[t2], vert_shader);
    gl_check_error();


    glAttachShader(main_programs[t1], frag_shaders[t1]);
    gl_check_error();
    glAttachShader(main_programs[t2], frag_shaders[t2]);
    gl_check_error();

    // link shader stages
    glLinkProgram(main_programs[t1]);
    check_program_linking(main_programs[t1]);

    glLinkProgram(main_programs[t2]);
    check_program_linking(main_programs[t2]);

    // remove now uneneccesary shaders
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shaders[t1]);
    glDeleteShader(frag_shaders[t2]);
    #pragma endregion

    // set clear color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
    while(!glfwWindowShouldClose(window))
    {
        process_input(window);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(main_programs[t1]);
        glBindVertexArray(VAOS[t1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
            
        
        glUseProgram(main_programs[t2]);
        glBindVertexArray(VAOS[t2]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}

/**
 * creates, compiles and checks for shader errors
 */
bool create_shader(unsigned int* shader_obj, GLenum shader_type, const char* path)
{
    FILE* f_shader_src = fopen(path, "r");
    if (f_shader_src == NULL)
    {
        perror("failed to open file: ");
        return false;
    }
    
    fseek(f_shader_src, 0, SEEK_END);
    long size = ftell(f_shader_src);
    fseek(f_shader_src, 0, SEEK_SET);
    
    char* shader_src = malloc(size + 1);
    fread(shader_src, 1, size, f_shader_src);
    shader_src[size] = '\0';

    *shader_obj = glCreateShader(shader_type);
    
    glShaderSource(*shader_obj, 1, (const GLchar * const*) &shader_src, NULL);
    gl_check_error();
    
    glCompileShader(*shader_obj);
    gl_check_error();
    
    int success;
    char info_log[SHADER_ERROR_LOG_SIZE];
    glGetShaderiv(*shader_obj, GL_COMPILE_STATUS, &success);
    gl_check_error();
    
    if(!success)
    {
        glGetShaderInfoLog(*shader_obj, SHADER_ERROR_LOG_SIZE, NULL, info_log);
        my_log(ERRMSG("failed to compile SHADER:VERTEX")"\nerror messages:\n%s\n", info_log);
        return false;
    }

    return true;

}


// Error checking
void check_program_linking(unsigned int shader_program)
{
    int success;
    char info_log[SHADER_ERROR_LOG_SIZE];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    gl_check_error();
    
    if(!success)
    {
        glGetShaderInfoLog(shader_program, SHADER_ERROR_LOG_SIZE, NULL, info_log);
        my_log(ERRMSG("failed to link SHADERS in program")"\nerror messages:\n%s\n", info_log);
    }

}

// Callbacks
void init(GLFWwindow** window)
{
    glfwInit();
    atexit(clean_up);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);

    my_assert(*window, "failed to creat window");
    glfwMakeContextCurrent(*window);
    
    my_assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "failed to initialize GLAD");

    glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
}

void process_input(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    
    // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    
    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    // {
    // }

    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    // {
    // }

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void clean_up()
{
    glfwTerminate();
    my_log("cleaned up\n");
}