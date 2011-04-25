/*
    ********************************************
    *   Albatross - A 2D multiplayer shooter   *
    ********************************************
    Copyright (C) 2011  Arthur Buldauskas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef TYPES_HEADER
#define TYPES_HEADER

#ifndef NULL
	#define NULL	0
#endif

#include<stdint.h>

typedef void(*fp)(void*);
typedef void(*fp_void)(void);

#define D2R(d)	float((float(d))/(57.29578))
#define R2D(r)	float((float(r))*(57.29578))

#endif
