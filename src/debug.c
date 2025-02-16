#include "debug.h"


GLenum gl_check_error_(const char *file,const char* func, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        char error[MAX_OPENGL_ERROR_MESSAGE];
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  strcpy(error, "INVALID_ENUM"); break;
            case GL_INVALID_VALUE:                 strcpy(error, "INVALID_VALUE"); break;
            case GL_INVALID_OPERATION:             strcpy(error, "INVALID_OPERATION"); break;
            case GL_STACK_OVERFLOW:                strcpy(error, "STACK_OVERFLOW"); break;
            case GL_STACK_UNDERFLOW:               strcpy(error, "STACK_UNDERFLOW"); break;
            case GL_OUT_OF_MEMORY:                 strcpy(error, "OUT_OF_MEMORY"); break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: strcpy(error, "INVALID_FRAMEBUFFER_OPERATION"); break;
            default :                              strcpy(error, "NO ERROR FOUND"); 
        }
        printf("%s:%d:%s(): " ERRMSG("%s\n"),file, line, func, error);
    }
    return errorCode;
}