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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "endianness.hh"



void swap_bytes(char *ret_bytes, int arg_count)
{
	int half = arg_count & ~1;
	
	if(!half)
		return;

	half /= 2;

	for(int i = 0; i < half; ++i)
	{
		int pair = arg_count - i - 1;
		char temp = ret_bytes[pair];
		ret_bytes[pair] = ret_bytes[i];
		ret_bytes[i] = temp;
	}
}

