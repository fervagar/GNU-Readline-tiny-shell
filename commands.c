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
#include <sys/wait.h>

// --------------------------- //
//  --- Command Callbacks ---  //
// --------------------------- //

int command_pwd(CMD_ARGS) {
        extern char shell_cwd[PATH_MAX];

        PRINTINFO("%s\n", shell_cwd);

        return 0;
}

int command_cd(CMD_ARGS) {
        extern char shell_cwd[PATH_MAX];
        char *newd;

        newd = (!argv[1] || !(*argv[1]))? NULL : argv[1];       // TODO: Change NULL for ~ 

        if(chdir(newd)) {
                PRINTERROR("[-] %s: %s\n", newd, strerror(errno));
                return -1;
        }

        // Update cwd
        if(!getcwd(shell_cwd, sizeof(shell_cwd))) {
                PRINTERROR("[-] Error trying to get the current working directory: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }

        return 0;
}

int command_ls(CMD_ARGS) {
        char *execvec[argc + 1];
        int i, status;
        pid_t child;

        execvec[0] = "/bin/ls";
        for(i = 1; i < argc; i++) {
                execvec[i] = argv[i];
        }
        execvec[i] = NULL;

        if(!(child = vfork())) {
                execv(execvec[0], execvec);
                PRINTERROR("[-] %s: %s\n", execvec[0], strerror(errno));
                exit(EXIT_FAILURE);
        }

        waitpid(child, &status, 0);
        return 0;
}

int command_exit(CMD_ARGS) {
        int ec = (argc > 1)? atoi(argv[1]) : 0;
        exit(ec);
}

int command_help(CMD_ARGS) {
        PRINTINFO("[DEBUG] TODO: command 'help'\n");

        return 0;
}

