/*
	This file is part of Gpup.

	Copyright 2008 G

	Gpup is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Gpup is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Gpup.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef ENDIANNESS_HH__
#define ENDIANNESS_HH__



#ifdef WORDS_BIGENDIAN
	#define BE2SYS(arg)
	#define LE2SYS(arg)	swap_bytes((char *)&arg, sizeof(arg))
	#define SYS2BE(arg)
	#define SYS2LE(arg)	swap_bytes((char *)&arg, sizeof(arg))
#else
	#define BE2SYS(arg) swap_bytes((char *)&arg, sizeof(arg))
	#define LE2SYS(arg)
	#define SYS2BE(arg) swap_bytes((char *)&arg, sizeof(arg))
	#define SYS2LE(arg)
#endif


void swap_bytes(char *ret_bytes, int arg_count);



#endif

