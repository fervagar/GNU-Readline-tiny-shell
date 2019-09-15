/*
 * Copyright (C) 2017 Fernando Vano Garcia
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

#define NULLCOMMAND { NULL /* cname */, NULL /* callback */, NULL /* doc */ }

{ "pwd",        command_pwd,
                "Print name of current/working directory",
},
{ "cd",         command_cd,
                "Change current working directory",
},
{ "ls",         command_ls,
                "List directory contents",
},
{ "exit",       command_exit,
                "Exit shell", 
},
{ "help",       command_help,
                "Print list of commands or help of a specific command usage",
},
NULLCOMMAND     /* NULLCOMMAND must be the last element */

