#pragma once

#include "Twilight Editor.h"

#include <cstdint>
#include <iostream>

namespace TwilightEditor
{
	/**
	 * Generates a tp-valid checksum for data
	 * and stores the results in csum and nsum
	*/
	void checkSum(uint8_t* data, std::size_t length, uint32_t* csum, uint32_t* nsum);
}