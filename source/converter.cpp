#include "converter.h"

namespace TwilightEditor
{
	namespace Converter
	{
		uint32_t u32(std::string& value)
		{
			try
			{
				return static_cast<uint32_t>(std::stoul(value, nullptr, 0));
			}
			catch (const std::invalid_argument& ia)
			{
				std::cerr << "error: " << ia.what() << std::endl;
				exit(1);
			}
			catch (const std::out_of_range& oor)
			{
				std::cerr << "error: " << oor.what() << std::endl;
				exit(1);
			}
			catch (...)
			{
				std::cerr << "unknown error while parsing u32" << value << std::endl;
				exit(1);
			}
		}

		uint16_t u16(std::string& value)
		{
			try
			{
				return static_cast<uint16_t>(std::stoul(value, nullptr, 0));
			}
			catch (const std::invalid_argument& ia)
			{
				std::cerr << "error: " << ia.what() << std::endl;
				exit(1);
			}
			catch (const std::out_of_range& oor)
			{
				std::cerr << "error: " << oor.what() << std::endl;
				exit(1);
			}
			catch (...)
			{
				std::cerr << "unknown error while parsing u16" << value << std::endl;
				exit(1);
			}
		}

		uint8_t u8(std::string& value)
		{
			try
			{
				return static_cast<uint8_t>(std::stoul(value, nullptr, 0));
			}
			catch (const std::invalid_argument& ia)
			{
				std::cerr << "error: " << ia.what() << std::endl;
				exit(1);
			}
			catch (const std::out_of_range& oor)
			{
				std::cerr << "error: " << oor.what() << std::endl;
				exit(1);
			}
			catch (...)
			{
				std::cerr << "unknown error while parsing u8" << value << std::endl;
				exit(1);
			}
		}

		uint32_t bigEndian(uint32_t value)
		{
			try
			{
				return ((value & 0xFF000000) >> 24 | (value & 0x000000FF) << 24 | (value & 0x00FF0000) >> 8 | (value & 0x0000FF00) << 8);
			}
			catch (...)
			{
				std::cerr << "unknown error while converting little u32 to big u32" << value << std::endl;
				exit(1);
			}
		}

		uint16_t bigEndian(uint16_t value)
		{
			try
			{
				return ((value & 0x00FF) << 8 | (value & 0xFF00) >> 8);
			}
			catch (...)
			{
				std::cerr << "unknown error while converting little u16 to big u16" << value << std::endl;
				exit(1);
			}
		}

		void flag(uint8_t* value, std::string& flag, bool set)
		{
			try
			{
				uint8_t _flag = u8(flag);

				if (set)
				{
					// Add the flags to the value
					*value |= _flag;
				}
				else
				{
					// Remove the flags from the value
					*value = (*value & ~_flag);
				}
			}
			catch (...)
			{
				std::cerr << "unknown error while " << (set ? "flagging " : "unflagging ") << value << " with " << flag << std::endl;
				exit(1);
			}
		}

		std::string toStr(uint8_t* data, offsetType type, std::size_t length, uint8_t flag)
		{
			try
			{
				std::string value("");

				// Templates
				bool flagstate;

				std::stringstream ss;

				uint16_t t16 = 0;
				uint32_t t32 = 0;


				if (type == offsetType::TPINT)
				{
					switch (length)
					{
						case 1:
							value = std::to_string(data[0]);
							if (Settings.HEX)
							{
								ss << std::hex << static_cast<unsigned>(data[0]);
								value = ss.str();
							}
						break;

						case 2:
							t16 = (data[1]| data[0] << 8);
							value = std::to_string(t16);
							if (Settings.HEX)
							{
								ss << std::hex << t16;
								value = ss.str();
							}
						break;

						case 4:
							t32 = (data[3] | data[2] << 8 | data[1] << 16 | data[0] << 24);
							value = std::to_string(t32);
							if (Settings.HEX)
							{
								ss << std::hex << t32;
								value = ss.str();
							}
						break;

						default:
							throw std::runtime_error("invalid length supplied for type int");
						break;
					}
				}

				else if (type == offsetType::TPFLAG || type == offsetType::TPUFLAG)
				{
					flagstate = (flag == (data[0] & flag));

					value = flagstate ? "true" : "false";
				}

				else
				{
					for (int i = 0; i < length; i++)
					{
						if (Settings.HEX)
						{
							// Not really optimized but functional
							ss << std::hex << static_cast<unsigned>(data[i]) << " ";
							value = ss.str();
						}
						else
						{
							// We're reading it as a proper string, if data[i] is null the string is terminated
							if (data[i] == 0x0)
							{
								break;
							}
							value += data[i];
						}
					}
				}

				return value;
			}
			catch (const std::runtime_error& ex)
			{
				std::cerr << "error converting data to string: " << ex.what() << std::endl;
				exit(1);
			}
			catch (...)
			{
				std::cerr << "unknown error while converting data to string" << std::endl;
				exit(1);
			}
		}
	}
}