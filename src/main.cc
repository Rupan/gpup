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

#include <stdlib.h>
#include <iostream>

#include "core/PS3PupProcessor.hh"
#include "main.hh"



using namespace std;



int main(int argc, char *argv[])
{
	cout << "Gpup v" << PACKAGE_VERSION << ", Copyright 2008 G."
		<< endl << endl;

	if(argc != 3)
	{
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	
	string command(argv[1]);
	string filename(argv[2]);

	PS3PupProcessor processor;
	/* Contains the version as a string terminated by EOF */
	processor.AddFilenameForType(0x100, string("ps3version.txt"));
	/* XML strings to be displayed during installation */
	processor.AddFilenameForType(0x101, string("license.xml"));
	/* Found only in promotion package, contained only one byte: 30 ("0" in ASCII). */
	processor.AddFilenameForType(0x102, string("promo.txt"));
	/* console_Type.txt: 0000 its for retail firmwares (CEX), 0100 for debug firmwares (DEX), 00300 for shop firmwares(SEX???) */
	processor.AddFilenameForType(0x103, string("console_Type.txt"));
	/* An SELF executable or nothing */
	processor.AddFilenameForType(0x200, string("ps3swu.self"));
	/* Contains XMB configurations files when FW version is less than 1.50 */
	processor.AddFilenameForType(0x201, string("vsh.tar"));
	/* Contains just "..." in ASCII, in 2.30 it contains "msg_updater" XML strings. */
	processor.AddFilenameForType(0x202, string("dots.txt"));
	/* firmware files */
	processor.AddFilenameForType(0x300, string("update.tar"));
	
	if(command == "x")
	{
		processor.LoadConfigFromFirmware(filename);
		processor.PrintConfig();
		processor.SaveConfig(filename + string(".xml"));
		processor.ExtractFirmware(filename);
	}
	else if(command == "c")
	{
		processor.LoadConfig(filename + string(".xml"));
		processor.PrintConfig();
		processor.CreateFirmware(filename);
	}
	else
	{
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}


void print_usage(char *arg_name)
{
	cout << "usage: " << arg_name << " <COMMAND> <FILENAME.PUP>" << endl;
	cout << "where COMMAND is:" << endl;
	cout << "    x - extract firmware" << endl;
	cout << "    c - create firmware" << endl;
}

