#include "checksum.h"
#include "settings.h"

namespace TwilightEditor
{
	void checkSum(uint8_t* data, std::size_t length, uint32_t* csum, uint32_t* nsum)
	{
		try
		{
			*csum = 0;
			*nsum = 0;

			for (uint32_t i = 0; i < length; i++)
			{
				*csum += data[i];
			}
			*nsum = -(*csum + length);
		}
		catch (...)
		{
			std::cout << "fatal error during checksum calculation..." << std::endl;
			exit(1);
		}
	}
}