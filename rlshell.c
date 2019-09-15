/*
 * Copyright (C) 2017 Fernando Vanyo Garcia
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *	Fernando Vano Garcia <fernando@fervagar.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>
#include <dirent.h>
#include <sys/types.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "preprocessor.h"
#include "commands.h"

#define PROMPT                          "(rlshell)> "

typedef struct command_t_ command_t;
typedef int (*cmdfunc_t)(CMD_ARGS);
struct command_t_ {
        char *cname;                    // Command name //
        cmdfunc_t callback;             // Callback function //
        char *doc;                      // Documentation //
};

// Current Working Directory
static char shell_cwd[PATH_MAX];

// Shell commands
static command_t commands[] = {
#include "cmd_spec.h"
};

// Embed the callbacks implementation //
#include "commands.c"

// ---------------------- //
//  --- Shell logic ---   //
// ---------------------- //

/**
 *  Search command structure (in 'commands[]' array) by command name.
 *  NOTE: Search cost is O(n) -- TODO: improve
 */
static command_t *search_command_by_name(char *name) {
        command_t *cmd_ptr;
        int i;

        if(!name) return (command_t *)NULL;

        for(i = 0; ((cmd_ptr = &commands[i])->cname); i++) {
                if(!strcmp(cmd_ptr->cname, name)) break;
        }

        if(!(cmd_ptr->cname)) {
                // Command not found in the array
                return (command_t *)NULL;
        }

        return cmd_ptr;
}

/**
 *  This function gets a previously validated command and process it. So, at this point, we
 *  assume that 'cmd' is a valid input for the shell (although it can be an undefined command).
 */
static int process_command(int argc, char *argv[]) {
        command_t *cmd_ptr;
        long len;
        char *cmd; 

        assert((argc > 0) && (argv));

        cmd = argv[0];
        len = strlen(cmd);
        assert(len > 0);

        // Search command
        if(!(cmd_ptr = search_command_by_name(cmd))) {
                PRINTERROR("[DEBUG] Command not found! [%s]\n", cmd);
                return -1;
        }

        // Execute the command callback
        cmd_ptr->callback(argc, argv);
        return STATE_OK;
}

/**
 *  Check if the input is a valid command. 
 *  The return value is treated as current STATE if >= 0 ; "bad command" otherwise
 */
static int parse_cmd(char *cmd) {
        char *token, *cmd_copy, **argv;
        long i, argc;
        int retval;

        // TODO: Add tokens to a dynamic data structure (e.g., list) as they are being processed...
        // -----
        cmd_copy = strdup(cmd);
        if (!cmd_copy) goto parse_cmd_mem_error;
        for(argc = 0, token = strtok(cmd_copy, " "); token; token = strtok(NULL, " "), argc++);
        free(cmd_copy);
        // -----

        argv = malloc(argc * sizeof(*argv));
        if (!argv) goto parse_cmd_mem_error;

        cmd_copy = strdup(cmd);
        if (!cmd_copy) goto parse_cmd_mem_error;
        for(i = 0, token = strtok(cmd_copy, " "); token; token = strtok(NULL, " "), i++) {
                argv[i] = token;
                if(!argv[i]) {
                        goto parse_cmd_mem_error;
                }
        }
        retval = process_command(argc, argv);
        free(cmd_copy);
        free(argv);

        return retval;

parse_cmd_mem_error:
        // System failure, exit...
        PRINTERROR("ERROR allocating memory: %s\n", strerror(errno));
        return STATE_SYSTEM_ERROR;
}

// ---------------------- //
//  --- GNU ReadLine ---  //
// ---------------------- //

/**
 *  Read a string, and return a pointer to it. Returns NULL on EOF.
 */
static inline char *rl_gets() {
        char *cmd;

        // Get a line from the user //
        cmd = readline(PROMPT);

        // If the line has any text in it, save it on the history //
        if(cmd && (*cmd > ' ')) {
                add_history(cmd);
        }

        return cmd;
}

/**
 *  This function is the actual completion matching generator. This function
 *  will be called repeatedly, generating a string each time. The argument
 *  'state' lets us to know if this is the first time it is called.
 *  Note: The matching string must be allocated with malloc(); ReadLine will
 *  perform the free.
 */
static char *rl_generator(const char *text, int state) {
        static long len, idx;
        char *ptr;

        if(!text) return NULL;
        if(!state) {                    // First call
                len = strlen(text);
                idx = 0;
        }

        while((ptr = commands[idx++].cname)) {
                if(!strncmp(ptr, text, len))
                        return strdup(ptr);
        }

        return NULL;
}

/**
 *  This function tries to perform the completion. Although the buffer 
 *  'rl_line_buffer' is accessible, the indexes 'start' and 'end' delimitate
 *  the bounds of 'text' (which is the word to perform the completion).
 */
static char** line_completion(const char *text, int start, int end) {
        char **match_list = (char **)NULL;

        match_list = rl_completion_matches(text, rl_generator);

        // Set 'rl_attempted_completion_over' to a non-zero value in order to
        // disable the default completion (if this function returns NULL)
        // rl_attempted_completion_over = 1;

        return match_list;
}

static inline void initialize_readline() {
        // Change the callback for 'autocompletion' functionality
        rl_attempted_completion_function = line_completion;
}

// ------------------------- //
//  --- Shell Main Loop ---  //
// ------------------------- //
int run_shell() {
        char *command;
        int state;

        initialize_readline();

        // Set the Current Working Directory
        if(!getcwd(shell_cwd, sizeof(shell_cwd))) {
                PRINTERROR("[-] Error trying to get the current working directory: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }

        state = STATE_EOF;
        while((command = rl_gets()) != NULL) {
                // Process the input
                if(*command) {
                        state = parse_cmd(command);
                        if(!(STATE_OK & state)) {
                                free(command);
                                break;
                        }
                }
                free(command);
                command = NULL;
                state = STATE_EOF;
        }

        if(STATE_EOF & state) {
                PRINTINFO("quit\n");
                state = STATE_USER_EXIT;
        }

        return 0;
}


int main() {
        return run_shell();
}

