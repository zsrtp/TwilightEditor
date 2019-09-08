#pragma once
#include "settings.h"

#include <string>
#include <iostream>
#include <cstdint>
#include <sstream>

namespace TwilightEditor
{
	namespace Converter
	{
		uint32_t u32(std::string& value);
		uint16_t u16(std::string& value);
		uint8_t u8(std::string& value);

		uint32_t bigEndian(uint32_t value);
		uint16_t bigEndian(uint16_t value);

		void flag(uint8_t* value, std::string& flag, bool set = true);

		std::string toStr(uint8_t* data, offsetType type, std::size_t length, uint8_t flag = 0);
	}
}