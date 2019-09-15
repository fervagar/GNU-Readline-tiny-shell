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

// -- Custom Preprocessor Macros -- //
#ifndef _RLS_PP_H
#define _RLS_PP_H       1337
// ---

#include <stdio.h>

#define SHELLOUT        (stdout)
#define SHELLERR        (stderr)

#define CMD_ARGS                        long argc, char *argv[]

// --------------------------------------------------------------------------------------
#define STATE_OK                        (1 << 0)        // Shell is running //
#define STATE_EOF                       (1 << 1)        // EOF in the command line //
#define STATE_USER_EXIT                 (1 << 2)        // User wants to exit //
#define STATE_SYSTEM_ERROR              (1 << 3)        // Error //
// --------------------------------------------------------------------------------------

//#define ARRAY_SIZE(__array)             (sizeof(__array)/sizeof(typeof(*__array)))

#define PRINTINFO(format, ...)          fprintf(SHELLOUT, format, ##__VA_ARGS__)
#define PRINTERROR(format, ...)         fprintf(SHELLERR, format, ##__VA_ARGS__)

// ---
#endif
