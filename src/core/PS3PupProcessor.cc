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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>
#include <map>

#include <libxml++/libxml++.h>

#include "common/typedefs.h"
#include "common/strings.hh"
#include "common/endianness.hh"
#include "PS3PupProcessor.hh"



using namespace std;



const char *PS3PupProcessor::m_MAGIC = "SCEUF\0\0\0";
const string PS3PupProcessor::m_FILENAME_BASE = "unknown_file_";
const string PS3PupProcessor::m_FILENAME_EXT = "bin";
const string PS3PupProcessor::m_HEX_PREFIX = "0x";



PS3PupProcessor::PS3PupProcessor() :
	m_ConfigLoaded(false)
{
	SetBufferSizeMb(m_DEFAULT_BUFFER_SIZE);
}


PS3PupProcessor::~PS3PupProcessor()
{
	FreeConfig();
}


void PS3PupProcessor::SetBufferSizeMb(u32 arg_size)
{
	m_BufferSize = arg_size * 1024 * 1024;
}


bool PS3PupProcessor::AddFilenameForType(u64 arg_type, string arg_filename)
{
	bool result = false;

	if(m_FilenameMap.find(arg_type) == m_FilenameMap.end())
	{
		m_FilenameMap[arg_type] = arg_filename;
		result = true;
	}

	return result;
}


string PS3PupProcessor::GetFilenameByType(u64 arg_type)
{
	string result;

	if(m_FilenameMap.find(arg_type) != m_FilenameMap.end())
	{
		result = m_FilenameMap[arg_type];
	}
	else
	{
		stringstream sS;
		sS << m_FILENAME_BASE << m_HEX_PREFIX << hex << setw(2)
			<< arg_type << '.' << m_FILENAME_EXT;
		sS >> result;
	}

	return result;
}


void PS3PupProcessor::ClearFilenamesMap()
{
	m_FilenameMap.clear();
}


string PS3PupProcessor::RecordHashToString(u8 *arg_hash)
{
	std::stringstream sS;
	sS << setfill('0');
	sS << hex;

	for(u64 i = 0; i < m_SIGNATURE_SIZE; ++i)
		sS << setw(2) << (int)arg_hash[i];
	return sS.str();
}


void PS3PupProcessor::StringToRecordHash(u8 *ret_hash, string arg_value)
{
	for(int i = 0; i < 20; ++i)
	{
		string digit(m_HEX_PREFIX + string(arg_value, i * 2, 2));
		ret_hash[i] = string_to_u64(digit);
	}
}


void PS3PupProcessor::LoadConfig(string arg_path)
{
	FreeConfig();

	memcpy((char *)m_Header.magic, m_MAGIC, sizeof(m_Header.magic));
	m_Header.records_count = 0;

	xmlpp::TextReader reader(arg_path);
	while(reader.read())
	{
		int depth = reader.get_depth();
		string name = reader.get_name();

		switch(depth)
		{
		case 0:
			if(name == "config" && reader.has_attributes())
			{
				reader.move_to_first_attribute();
				do
				{
					if(reader.get_name() == "version")
					{
						m_Header.version = string_to_u64(reader.get_value());
					}
					if(reader.get_name() == "unknown")
					{
						m_Header.unknown = string_to_u64(reader.get_value());
					}
					if(reader.get_name() == "signature")
					{
						StringToRecordHash(m_Signature.hash,
							reader.get_value());

						memset((void *)m_Signature.padding, 0x00,
							sizeof(m_Signature.padding));
					}
				}while(reader.move_to_next_attribute());
			}
			break;

		case 1:
			if(name == "file" && reader.has_attributes())
			{
				FileRecord record;
				reader.move_to_first_attribute();
				do
				{
					if(reader.get_name() == "type")
					{
						record.location.type =
							string_to_u64(reader.get_value());
					}
					if(reader.get_name() == "signature")
					{
						StringToRecordHash(record.signature.hash,
							reader.get_value());
					}
				}while(reader.move_to_next_attribute());
				
				record.location.padding = 0;
				record.signature.index = m_Records.size();
				record.signature.padding = 0;
				m_Records.push_back(record);
			}
			break;
		}
	}
	
	m_Header.records_count = m_Records.size();
	m_Header.header_size = sizeof(m_Header) +
		sizeof(FileRecord) * m_Header.records_count + sizeof(m_Signature);
}


bool PS3PupProcessor::LoadConfigFromFirmware(string arg_path)
{
	bool result = false;

	ifstream iF;
	iF.open(arg_path.c_str(), ios::binary);
	if(!iF.is_open())
	{
		cout << "error: cann't open file " << arg_path << endl;
		return result;
	}

	FreeConfig();

	// read header
	iF.read((char *)&m_Header, sizeof(m_Header));

	// check magic
	if(memcmp(m_Header.magic, m_MAGIC, sizeof(m_Header.magic)))
	{
		cout << "error: " << arg_path << " isn't proper PUP-file" << endl;
		return result;
	}

	// convert from big-endian to system endianness
	BE2SYS(m_Header.version);
	BE2SYS(m_Header.unknown);
	BE2SYS(m_Header.records_count);
	BE2SYS(m_Header.header_size);
	BE2SYS(m_Header.data_size);

	//G	m_Records = new FileRecord[m_Header.records_count];

	FileRecord record;
	// read locations
	for(u64 i = 0; i < m_Header.records_count; ++i)
	{
		iF.read((char *)&record.location, sizeof(record.location));

		// convert from big-endian to system endianness
		BE2SYS(record.location.type);
		BE2SYS(record.location.offset);
		BE2SYS(record.location.size);
		BE2SYS(record.location.padding);

		m_Records.push_back(record);
	}

	// read signatures
	for(u64 i = 0; i < m_Header.records_count; ++i)
	{
		iF.read((char *)&m_Records[i].signature,
				sizeof(m_Records[i].signature));

		// convert from big-endian to system endianness
		BE2SYS(m_Records[i].signature.index);
	}
	
	// read global signature
	iF.read((char *)&m_Signature, sizeof(m_Signature));
	
	// TODO - check Sony signature ;)
	
	iF.close();
	
	m_ConfigLoaded = true;
	
	result = true;
	return result;
}


bool PS3PupProcessor::FreeConfig()
{
	bool result = false;
	
	if(m_ConfigLoaded)
	{
		m_Records.clear();
		
		m_ConfigLoaded = false;
		result = true;
	}

	return result;
}


void PS3PupProcessor::SaveConfig(string arg_path)
{
	xmlpp::Document document;

	// create root node
	xmlpp::Element *root_node = document.create_root_node("config");
	root_node->set_attribute("version", to_string<u64>(m_Header.version));
	root_node->set_attribute("unknown", to_string<u64>(m_Header.unknown, true));
	root_node->set_attribute("signature", RecordHashToString(m_Signature.hash));

	// create file records
	for(u64 i = 0; i < m_Header.records_count; ++i)
	{
		root_node->add_child_comment(
				GetFilenameByType(m_Records[i].location.type));

		xmlpp::Element *node_record = root_node->add_child("file");
		node_record->set_attribute("type",
			to_string<u64>(m_Records[i].location.type, true));
		node_record->set_attribute("signature",
			RecordHashToString(m_Records[i].signature.hash));
	}

	document.write_to_file_formatted(arg_path, "utf-8");
}


bool PS3PupProcessor::PrintConfig()
{
	bool result = false;

	if(!m_ConfigLoaded)
		return result;

	cout << setfill('0');

	// output file info
	cout << "GENERAL: " << endl;
	cout << "version: " << m_HEX_PREFIX << hex << setw(2)
		<< m_Header.version << endl;
	cout << "unknown: " << m_HEX_PREFIX << hex << m_Header.unknown << endl;
	cout << "signature: " << m_HEX_PREFIX;
		for(u64 j = 0; j < sizeof(m_Signature.hash); ++j)
			cout << hex << setw(2) << (int)m_Signature.hash[j];
	cout << endl;
	cout << "records count: " << dec << m_Header.records_count << endl << endl;

	// records
	for(u64 i = 0; i < m_Header.records_count; ++i)
	{
		cout << dec << "RECORD " << i + 1 << ": " << endl;
		cout << "type: " << m_HEX_PREFIX << hex << m_Records[i].location.type
			<< " [" << GetFilenameByType(m_Records[i].location.type) << "]" << endl;
		cout << "size: " << dec << m_Records[i].location.size << endl;

		cout << "signature: " << m_HEX_PREFIX;
		for(u64 j = 0; j < sizeof(m_Records[i].signature.hash); ++j)
			cout << hex << setw(2) << (int)m_Records[i].signature.hash[j];
		cout << endl;
		cout << endl;
	}

	cout << endl;

	result = true;
	return result;
}


bool PS3PupProcessor::ExtractFirmware(string arg_path)
{
	bool result = false;

	if(!m_ConfigLoaded)
		LoadConfigFromFirmware(arg_path);

	if(!m_ConfigLoaded)
		return result;

	ifstream iF;
	iF.open(arg_path.c_str(), ios::binary);
	if(!iF.is_open())
	{
		cout << "error: cann't open file " << arg_path << endl;
		return result;
	}

	// data extraction
	char *buffer = new char[m_BufferSize];
	for(u64 i = 0; i < m_Header.records_count; ++i)
	{
		cout << setfill('0');

		// show info about file
		string output_filename = GetFilenameByType(m_Records[i].location.type);
		cout << "extracting file " << dec << i + 1
			<< " [" << output_filename << "]" << endl;
		cout << "type: " << m_HEX_PREFIX << hex
			<< m_Records[i].location.type << endl;
		cout << "size: " << dec << m_Records[i].location.size << endl;
		cout << "signature[" << dec << m_Records[i].signature.index
			<< "]: " << m_HEX_PREFIX << hex;
		for(u64 j = 0; j < sizeof(m_Records[i].signature.hash); ++j)
			cout << setw(2) << (int)m_Records[i].signature.hash[j];
		cout << endl;

		streampos size = (streampos)m_Records[i].location.size;
		streampos left_bytes = size;
		iF.seekg(m_Records[i].location.offset);

		ofstream oF;
		oF.open(output_filename.c_str(), ios::binary);
		if(!oF.is_open())
		{
			cout << "error: cann't create file " << arg_path << endl;
			delete [] buffer;
			return result;
		}

		cout << setfill(' ');
		while(left_bytes)
		{
			int to_read = left_bytes > m_BufferSize ?
				m_BufferSize : (int)left_bytes;

			left_bytes -= to_read;
			iF.read(buffer, to_read);
			oF.write(buffer, to_read);

			int percentage = (1 - left_bytes / (float)size) * 100;
			cout << '\r';
			cout << "progress: ";
			cout.setf(ios::right, ios::basefield);
			cout.width(3);
			cout << percentage << '%' << flush;
		}

		cout << endl << "done" << endl << endl;
		oF.close();
	}

	delete [] buffer;

	iF.close();


	result = true;
	return result;
}


bool PS3PupProcessor::CreateFirmware(string arg_path)
{
	bool result = false;
	
	ofstream oF;
	oF.open(arg_path.c_str(), ios::binary);
	if(!oF.is_open())
	{
		cout << "error: cann't create file " << arg_path << endl;
		return result;
	}

	m_Header.data_size = 0;	
	oF.seekp(m_Header.header_size);
	
	char *buffer = new char[m_BufferSize];
	for(u64 i = 0; i < m_Header.records_count; ++i)
	{
		cout << setfill('0');
		
		string input_filename = GetFilenameByType(m_Records[i].location.type);
		ifstream iF;
		iF.open(input_filename.c_str(), ios::binary);
		if(!iF.is_open())
		{
			cout << "error: cann't open file " << input_filename << endl;
			delete [] buffer;
			return result;
		}
		
		m_Records[i].location.offset = oF.tellp();
		m_Records[i].location.size = GetFileSize(iF);
		m_Header.data_size += m_Records[i].location.size;

		cout << "writing file " << dec << i + 1
			<< " [" << input_filename << "]" << endl;
		cout << "type: " << m_HEX_PREFIX << hex
			<< m_Records[i].location.type << endl;
		cout << "size: " << dec << m_Records[i].location.size << endl;
		cout << "signature[" << dec << m_Records[i].signature.index
			<< "]: " << m_HEX_PREFIX << hex;
		for(u64 j = 0; j < sizeof(m_Records[i].signature.hash); ++j)
			cout << setw(2) << (int)m_Records[i].signature.hash[j];
		cout << endl;
		
		streampos size = (streampos)m_Records[i].location.size;
		streampos left_bytes = size;
		
		cout << setfill(' ');
		while(left_bytes)
		{
			int to_write = left_bytes > m_BufferSize ?
				m_BufferSize : (int)left_bytes;
			
			left_bytes -= to_write;
			iF.read(buffer, to_write);
			oF.write(buffer, to_write);

			int percentage = (1 - left_bytes / (float)size) * 100;
			cout << '\r';
			cout << "progress: ";
			cout.setf(ios::right, ios::basefield);
			cout.width(3);
			cout << percentage << '%' << flush;
		}
		
		cout << endl << "done" << endl << endl;
		
		iF.close();
	}
	
	// write header
	oF.seekp(0, ios::beg);
	
	SYS2BE(m_Header.version);
	SYS2BE(m_Header.unknown);
	SYS2BE(m_Header.records_count);
	SYS2BE(m_Header.header_size);
	SYS2BE(m_Header.data_size);
	oF.write((char *)&m_Header, sizeof(m_Header));
	BE2SYS(m_Header.version);
	BE2SYS(m_Header.unknown);
	BE2SYS(m_Header.records_count);
	BE2SYS(m_Header.header_size);
	BE2SYS(m_Header.data_size);

	for(u64 i = 0; i < m_Header.records_count; ++i)
	{
		SYS2BE(m_Records[i].location.type);
		SYS2BE(m_Records[i].location.offset);
		SYS2BE(m_Records[i].location.size);
		SYS2BE(m_Records[i].location.padding);
		oF.write((char *)&m_Records[i].location, sizeof(m_Records[i].location));
		BE2SYS(m_Records[i].location.type);
		BE2SYS(m_Records[i].location.offset);
		BE2SYS(m_Records[i].location.size);
		BE2SYS(m_Records[i].location.padding);
	}

	for(u64 i = 0; i < m_Header.records_count; ++i)
	{
		SYS2BE(m_Records[i].signature.index);
		oF.write((char *)&m_Records[i].signature, sizeof(m_Records[i].signature));
		BE2SYS(m_Records[i].signature.index);
	}
	
	oF.write((char *)&m_Signature, sizeof(m_Signature));
	
	delete [] buffer;
	
	oF.close();
	
	result = true;
	return result;
}


streampos PS3PupProcessor::GetFileSize(ifstream &arg_if)
{
	streampos last_pos = arg_if.tellg();
	arg_if.seekg(0, ios::end);
	streampos result = arg_if.tellg();
	arg_if.seekg(last_pos);
	
	return result;
}

