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



#ifndef PS3_PUP_PROCESSOR_HH__
#define PS3_PUP_PROCESSOR_HH__



#include <string.h>
#include <map>
#include <vector>

#include "common/typedefs.h"



using namespace std;



class PS3PupProcessor
{
private:
	static const char *m_MAGIC;
	static const u32 m_SIGNATURE_SIZE = 20;
	static const string m_FILENAME_BASE;
	static const string m_FILENAME_EXT;
	static const string m_HEX_PREFIX;
	static const u32 m_DEFAULT_BUFFER_SIZE = 10;
	
	struct Header
	{
		u8 magic[8];
		u64 version;
		u64 unknown;
		u64 records_count;
		u64 header_size;
		u64 data_size;
	};

	struct FileRecord
	{
		struct Location
		{
			u64 type;
			u64 offset;
			u64 size;
			u64 padding;
		} location;
	
		struct Signature
		{
			u64 index;
			u8 hash[20];
			u32 padding;
		} signature;
	};

	struct GlobalSignature
	{
		u8 hash[20];
		u8 padding[12];
	};
	
	u32 m_BufferSize;
	Header m_Header;
	vector<FileRecord> m_Records;
	GlobalSignature m_Signature;
	bool m_ConfigLoaded;
	map<u64, string> m_FilenameMap;

public:
	PS3PupProcessor();
	~PS3PupProcessor();
	void SetBufferSizeMb(u32 arg_size);
	bool AddFilenameForType(u64 arg_type, string arg_filename);
	string GetFilenameByType(u64 arg_type);
	void ClearFilenamesMap();
	string RecordHashToString(u8 *arg_hash);
	void StringToRecordHash(u8 *ret_hash, string arg_value);
	void LoadConfig(string arg_path);
	bool LoadConfigFromFirmware(string arg_path);
	bool FreeConfig();
	void SaveConfig(string arg_path);
	bool PrintConfig();
	bool ExtractFirmware(string arg_path);
	bool CreateFirmware(string arg_path);
	streampos GetFileSize(ifstream &arg_if);
};



#endif

