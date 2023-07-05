#pragma once



#define ANSI_COLOR_BOLD	   "\x1b[1m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
#define ANSI_COLOR_GRAY    "\x1b[90m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_CLS "\x1b[2J"
#define ANSI_CUR_HOME "\033[0;0H"

#define ANSI_CLEAR_LINE "\x1b[2K"
#define ANSI_CUR_BEGIN "\033[100D"

#define ANSI_REDRAW_LINE ANSI_CLEAR_LINE ANSI_CUR_BEGIN

#define ANSI_CUR(x,y) "\x1b[" #x ";" #y "H"

