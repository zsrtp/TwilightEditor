#pragma once

#include "settings.h"
#include "cxxopts.h"
#include "config.h"
#include "converter.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]);
void fastPrint(const std::string& message, bool newline = true);
cxxopts::ParseResult parse(int argc, char* argv[]);

namespace TwilightEditor
{
	extern FILE* currentFilePtr;
	extern std::size_t fileSize;
	extern uint16_t questLogSize;
	extern uint32_t questLogReadOffset;
	
	void initFile();
	void set();
	std::string get();
}