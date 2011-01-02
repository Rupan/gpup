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



#ifndef STRINGS_HH__
#define STRINGS_HH__



#include <sstream>

#include "typedefs.h"



template <typename T>
inline std::string to_string(const T& arg, bool arg_hex = false)
{
	std::stringstream sS;
	if(arg_hex)
		sS << "0x" << std::hex;
	sS << arg;
	return sS.str();
}

u64 string_to_u64(const std::string &arg_value);
s64 string_to_s64(const std::string &arg_value);



#endif

