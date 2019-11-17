#pragma once

#include "settings.h"
#include "cxxopts.h"
#include "config.h"
#include "converter.h"
#include "checksum.h"

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]);
void fastPrint(const std::string& message, bool newline = true);
cxxopts::ParseResult parse(int argc, char* argv[]);

namespace TwilightEditor
{
	extern uint32_t checksum;
	extern uint32_t nchecksum;

	extern FILE* currentFilePtr;
	extern FILE* outputFile;

	extern uint8_t* questLogData;

	extern uint8_t* inputFileBuf;
	extern uint8_t* outputFileBuf;

	extern uintmax_t fileSize;
	extern uint16_t questLogLength;
	extern uint32_t questLogReadOffset;
	
	void initFile();
	void saveFile();
	void set();
	void readChecksums();
	void setChecksums();
	std::string get();
}