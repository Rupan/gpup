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

#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "typedefs.h"
#include "strings.hh"



u64 string_to_u64(const std::string &arg_value)
{
	return strtoul(arg_value.c_str(), NULL, 0);
}


s64 string_to_s64(const std::string &arg_value)
{
	return strtol(arg_value.c_str(), NULL, 0);
}

