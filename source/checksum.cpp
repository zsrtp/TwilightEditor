#include "checksum.h"
#include "settings.h"

namespace TwilightEditor
{
	void checkSum(uint8_t* data, std::size_t length, uint32_t* csum, uint32_t* nsum)
	{
		try
		{
			uint32_t sum = 0;
			uint32_t _sum = 0;

			for (uint32_t i = 0; i < length; i++)
			{
				sum += data[i];
			}
			_sum = -(sum + 0x0a8c);

			*csum = sum;
			*nsum = _sum;
		}
		catch (...)
		{
			std::cout << "Fatal error during checksum calculation..." << std::endl;
			exit(1);
		}
	}
}