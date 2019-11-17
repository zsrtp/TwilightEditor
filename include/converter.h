#pragma once
#include "settings.h"

#include <string>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <iomanip>

namespace TwilightEditor
{
	namespace Converter
	{
		uint32_t u32(std::string& value);
		uint16_t u16(std::string& value);
		uint8_t u8(std::string& value);

		int32_t s32(std::string& value);
		int16_t s16(std::string& value);
		int8_t s8(std::string& value);

		uint32_t bigEndian(uint32_t value);
		uint16_t bigEndian(uint16_t value);

		int32_t bigEndian(int32_t value);
		int16_t bigEndian(int16_t value);

		// Sets or unsets a flag in value
		void flag(uint8_t* value, std::string& flag_value, bool set = true);

		// Reads big endian data and returns its string equivalent depending on type and length
		std::string toStr(uint8_t* data, offsetType type, std::size_t length, bool hex = false, uint8_t flag = 0);
	}
}