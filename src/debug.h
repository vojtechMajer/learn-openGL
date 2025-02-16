// comment symbols: ●, -, ⤷, ✓, ❌

#ifndef __MY_DEBUG_H__
#define __MY_DEBUG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glext.h>

#define MAX_OPENGL_ERROR_MESSAGE 30

GLenum gl_check_error_(const char *file,const char* func, int line);



#pragma region colors
// standard ansi colors for terminals with ANSI color support
#define ANSI_COLOR_RED     "\033[31m"
#define ANSI_COLOR_GREEN   "\033[32m"
#define ANSI_COLOR_YELLOW  "\033[33m"
#define ANSI_COLOR_BLUE    "\033[34m"
#define ANSI_COLOR_MAGENTA "\033[35m"
#define ANSI_COLOR_CYAN    "\033[36m"
#define ANSI_COLOR_RESET   "\033[0m"

#define STARTBOLD   "\033[1m"

// predefind text colors
#define ERRMSG(MSG)     STARTBOLD ANSI_COLOR_RED    MSG     ANSI_COLOR_RESET 
#define SCCSMSG(MSG)    STARTBOLD ANSI_COLOR_GREEN  MSG     ANSI_COLOR_RESET
#define INFOMSG(MSG)              ANSI_COLOR_BLUE   MSG     ANSI_COLOR_RESET
#define PATHMSG(MSG)    STARTBOLD ANSI_COLOR_GREEN  MSG     ANSI_COLOR_RESET
#define WARRMSG(MSG)    STARTBOLD ANSI_COLOR_YELLOW MSG     ANSI_COLOR_RESET

#define TXTMSGB(MSG)    STARTBOLD                   MSG     ANSI_COLOR_RESET
#define TXTMSG(MSG)                                 MSG     ANSI_COLOR_RESET

#pragma endregion






#pragma region asserts
// returns string value of macro e.g
/*
    #define my_macro 69
    str(my_macro)
    output  => "my_macro"
*/
#define str(s) #s

// general assert for everything
#define my_assert(EX, errmsg) \
    if(!(EX)) { \
        fprintf(stderr, PATHMSG("%s:%d:%s(): ") ERRMSG("asserted ( ") str(EX) ERRMSG(" )\nError: %s\n") ,__FILE__, __LINE__, __func__, errmsg); \
        exit(EXIT_FAILURE); \
    }

// Define ENABLE_LOGS before including this header to enable logging
#ifdef ENABLE_LOGS

#define gl_check_error() gl_check_error_(__FILE__, __func__, __LINE__);


// this is log for logging stuff... what else did you really expect
#define my_log(...) \
    fprintf(stdout, __VA_ARGS__);

// logs if condition is met
#define my_log_if(EX, ...) \
    if(EX) { \
    fprintf(stdout, __VA_ARGS__); \
    }

// Debug disabled
#else
#define my_log(...)
#define my_log_if(EX, ...)
#define gl_check_error()
#endif

#pragma endregion



#endif // __MY_DEBUG_H__