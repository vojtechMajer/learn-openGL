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

bool create_shader(unsigned int* shader_obj, GLenum shader_type, const char* path);

void check_program_linking(unsigned int shader_program);

void init(GLFWwindow** window);
void process_input(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void clean_up();


// cube
float vertices[] = {
    -0.5f, -0.5f, 0,
     0.5f, -0.5f, 0,
     0.5f,  0.5f, 0
};



int main(void)
{
    GLFWwindow* window;
    init(&window);

    unsigned int main_program;
    unsigned int vert_shader;
    unsigned int frag_shader;

    // vertex buffer object
    unsigned int VBO;
    // element buffer object (index buffer)
    unsigned int EBO;
    // vertex array object (holds VBO configuration)
    unsigned int VAO;
    
    // 1. create buffers & VAO
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    // 2. bind VAO -> VAO then stores last bound GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER and vertex attribute configuration !! Cerefull VAO also stores unBindCalls !!
    glBindVertexArray(VAO);
    
    // 3. copy our data to buffers for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    gl_check_error();

    // EBO
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // gl_check_error();

    // then set our vertex attributes pointers
    // 0 - corresponds to location qualifier in vertex shader layout (location = 0) in vec3 aPos;
    // 3, GL_FLOAT TAKE 3 times float size from vertex of total size 6*float
    // offset skip position attribute (3 floats x,y,z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // unbind VAO
    glBindVertexArray(0);

    #pragma region texture

    unsigned int textures[2];
    glGenTextures(2, textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("resources/textures/wall.jpg", &width, &height, &nrChannels, 0); 
    my_assert(data, "failed to load texture");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    unsigned char *data2 = stbi_load("resources/textures/awesomeface.png", &width, &height, &nrChannels, 0); 
    my_assert(data2, "failed to load texture");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data2);

    #pragma endregion

    #pragma region shader program creation
    my_assert(create_shader(&vert_shader, GL_VERTEX_SHADER, "./shaders/vertex.vert"), "failed to create VETEXE SHADER");
    my_assert(create_shader(&frag_shader, GL_FRAGMENT_SHADER, "./shaders/fragment.frag"), "failed to create FRAGMENT SHADER");

    main_program = glCreateProgram();
    
    // Attach shader stages
    glAttachShader(main_program, vert_shader);
    gl_check_error();

    glAttachShader(main_program, frag_shader);
    gl_check_error();

    // link shader stages
    glLinkProgram(main_program);
    check_program_linking(main_program);

    // remove now uneneccesary shaders
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    #pragma endregion


    // select active program(shaders) for rendering
    glUseProgram(main_program);

    // set uniforms
    // set texture units to texture samplers
    glUniform1i(glGetUniformLocation(main_program, "texture1"), 0);
    glUniform1i(glGetUniformLocation(main_program, "texture2"), 1);

    // select active Vertex array object (VBO + VBO configuration) for rendering
    glBindVertexArray(VAO);

    // set clear color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
    while(!glfwWindowShouldClose(window))
    {
        process_input(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // glDrawArrays(GL_TRIANGLES, 0, 36);
    
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
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
    // vec3 cam_right = {0,0,0};

    // if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    //     glfwSetWindowShouldClose(window, true);

    // const float camera_speed = 0.05f; // adjust accordingly
    
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
    my_log("cleaned up\n");
    glfwTerminate();
}