/*
 * Handles the parsing of a single .vm file, and encapsulates access to the
 * input code. It reads VM commands, parses them and provides convenient access
 * to these components. In addition, it removes all white space and comments.
 */
#include "parse.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fd;
char *command;

/* Opens the input file/stream and gets ready to parse it. */
void parseConstructor(char *file)
{
    printf("TC_DEBUG: opening input file -> \'%s\'\n", file);
    
    fd = fopen(file, "r");

    if(!fd) {
        fprintf(stderr, "Unable to open file \'%s\'", file);
    }
    
    command = (char *)malloc(257);

    if(command == NULL) {
        fprintf(stderr, "Unable to create command buffer");
    }
}

/* Are there more commands in the input? */
int hasMoreCommands()
{
    if(fd == NULL) {
        return 0;
    }

    char c = fgetc(fd);

    while(1) {
        if(c == '/') {
            c = fgetc(fd);

            if(c == '/') {
                while((c != '\n') && !feof(fd)) {
                    c = fgetc(fd);
                }
            }
            else {
                ungetc(c, fd);
                ungetc('/', fd);

                break;
            }
        }
        else if(!isspace(c)) {
            ungetc(c, fd);

            break;
        }
        else if(feof(fd)) {
            break;
        }

        c = fgetc(fd);
    }

    return !feof(fd);
}

/*
 * Reads the next command from the input and makes it the current
 * command. Should be called only if hasMoreCommands() is true. Initialily there
 * is no current command.
 */
void advance()
{
    if(hasMoreCommands() && command) {
        char c = fgetc(fd);
        int i = 0;
        int full = 0;
        int maybe_comment = 0;
        int leading = 1;
        
        while((c != '\n') && !feof(fd)) {
            if(leading) {
                if((c == '\t') || (c == '\v') || (c == '\f') || (c == ' ')) {
                    c = fgetc(fd);
                    
                    continue;
                }
                else {
                    leading = 0;
                }
            }

            if(i > 256) {
                full = 1;
            }

            if(!full) {
                if(c == '/') {
                    ++maybe_comment;
                }

                if(maybe_comment == 2) {
                    ungetc('/', fd);
                    ungetc('/', fd);
                    command[i - 1] = '\0';

                    break;
                }
                else if(maybe_comment < 2) {
                    command[i] = c;
                    ++i;
                    c = fgetc(fd);
                }
            }
        }

        command[i] = '\0';

        if(command[i - 1] == '\r') {
            command[i - 1] == '\0';
        }

        while(isspace(command[strlen(command) - 1])) {
            command[strlen(command) - 1] = '\0';
        }

        if(!feof(fd)) {
            c = ungetc(c, fd);
        }
    }
}

/*
 * Returns the type of the current VM command. C_ARITHMETIC is returned for all
 * the arithmetic commands.
 */
VM_TYPE commandType()
{
    if(strncmp(command, "push", strlen("push")) == 0) {
        return C_PUSH;
    }
    else if(strncmp(command, "pop", strlen("pop")) == 0) {
        return C_POP;
    }
    else if(strncmp(command, "label", strlen("label")) == 0) {
        return C_LABEL;
    }
    else if(strncmp(command, "goto", strlen("goto")) == 0) {
        return C_GOTO;
    }
    else if(strncmp(command, "if-goto", strlen("if-goto")) == 0) {
        return C_IF;
    }
    else if(strncmp(command, "function", strlen("function")) == 0) {
        return C_FUNCTION;
    }
    else if(strncmp(command, "call", strlen("call")) == 0) {
        return C_CALL;
    }
    else if(strncmp(command, "return", strlen("return")) == 0) {
        return C_RETURN;
    }
    else {
        return C_ARITHMETIC;
    }
}

/*
 * Returns the first argument of the current command. In the case of
 * C_ARITHMETIC the command itself(add, sub, etc.) is returned. Should not get
 * called if the current commands is C_RETURN.
 */
char *arg1()
{
    if(commandType() == C_RETURN) {
        return NULL;
    }

    char *result = NULL;
    
    if(commandType() == C_ARITHMETIC) {        
        int len = 2;

        if(!isspace(*(command + 2))) {
            len = 3;
        }

        result = (char *)malloc(len + 1);

        if(result == NULL) {
            fprintf(stderr, "Unable to allocate arg1");

            return NULL;
        }

        result[0] = *command;
        result[1] = *(command + 1);

        if(len == 3) {
            result[2] = *(command + 2);
            result[3] = '\0';
        }
        else {
            result[2] = '\0';
        }

        return result;
    }

    char *s = command;

    while(!isspace(*s) && (*s != '\0')) {
        s += 1;
    }

    while(isspace(*s) && (*s != '\0')) {
        s += 1;
    }

    if(s != '\0') {
        char *e = s + 1;

        while(!isspace(*e) && (*e != '\0')) {
            e += 1;
        }

        result = (char *)malloc(e - s + 1);
        strncpy(result, s, e - s);
        result[e - s] = '\0';

        return result;
    }

    return NULL;
}

/*
 * Returns the second argument of the current command. Should be called only if
 * the current command is C_PUSH, C_POP, C_FUNCTION, or C_CALL.
 */
int arg2()
{
    if((commandType() != C_PUSH)
       && (commandType() != C_POP)
       && (commandType() != C_FUNCTION)
       && (commandType() != C_CALL)) {
        return -1;
    }

    char *s = command + strlen(command) - 1;
    int ctr = 0;

    while(!isspace(*s) && (ctr < strlen(command))) {
        ++ctr;
        --s;
    }

    return atoi(s + 1);
}
