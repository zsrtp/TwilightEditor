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
		TPUINT,
		TPSINT,
		TPFLAG,
		TPUFLAG,
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
		bool HEX;

		// Offset
		uint16_t OFFSET;
		uint16_t LENGTH;
		offsetType TYPE;
		std::string STR_VALUE;

		// Additional
		bool VERBOSE;
		bool FORCE;
	};

	extern settings Settings;
}