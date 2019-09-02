#pragma once

#include <cstdint>
#include <string>

namespace TwilightEditor
{
	enum mode
	{
		GET,
		SET
	};

	enum offsetType
	{
		TPINT,
		TPFLAG,
		TPSTRING
	};

	struct settings
	{
		// General
		std::string INPUT_FILE;
		std::string OUTPUT_FILE;
		mode MODE;
		uint16_t QUESTLOG;
		bool FIX;
		
		// Offset
		uint16_t OFFSET;
		uint16_t LENGTH;
		offsetType TYPE;
		std::string VALUE;

		// Additional
		bool VERBOSE;
		bool FORCE;
	};
}