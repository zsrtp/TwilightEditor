#include "Twilight Editor.h"


int main(int argc, char* argv[])
{
	auto result = parse(argc, argv);
	auto arguments = result.arguments();

	if (TwilightEditor::Settings.VERBOSE)
	{
		std::cout << "Settings:" << std::endl;
		std::cout << "Input File: " << TwilightEditor::Settings.INPUT_FILE << std::endl;
		std::cout << "Output File: " << TwilightEditor::Settings.OUTPUT_FILE << std::endl;
		std::cout << "Mode: " << TwilightEditor::Settings.MODE << std::endl;
		std::cout << "QuestLog: " << TwilightEditor::Settings.QUESTLOG << std::endl;
		std::cout << "Fix checksum: " << TwilightEditor::Settings.FIX << std::endl;


		std::cout << "Offset: " << TwilightEditor::Settings.OFFSET << std::endl;
		std::cout << "Length: " << TwilightEditor::Settings.LENGTH << std::endl;
		std::cout << "Type: " << TwilightEditor::Settings.TYPE << std::endl;
		std::cout << "Value: " << TwilightEditor::Settings.STR_VALUE << std::endl;

		std::cout << "Verbose: " << TwilightEditor::Settings.VERBOSE << std::endl;
		std::cout << "Force: " << TwilightEditor::Settings.FORCE << std::endl;
	}

	fastPrint("initializing file...");

	TwilightEditor::initFile();

	switch (TwilightEditor::Settings.MODE)
	{
		case TwilightEditor::mode::GET:
			fastPrint("> GET");

			std::cout << TwilightEditor::get() << std::endl;
		break;

		case TwilightEditor::mode::SET:
			fastPrint("> SET");

			TwilightEditor::set();
		break;
	}

	fastPrint("Closing file...");
	fclose(TwilightEditor::currentFilePtr);
	return 0;
}

void fastPrint(const std::string& message, bool newline)
{
	if (TwilightEditor::Settings.VERBOSE)
	{
		std::cout << message;
		if (newline) std::cout << std::endl;
	}
}

cxxopts::ParseResult parse(int argc, char* argv[])
{
	try
	{
		cxxopts::Options options(argv[0], "Twilight Editor | (C) AECX 2019 | *required");

		options.add_options("General")
			("h,help", " Prints this help")
			("i,input", "*(string) Path to input file", cxxopts::value<std::string>(), "<path/to/file>")
			("m,mode", " (string) <GET> or <SET>", cxxopts::value<std::string>()->default_value("GET"), "<GET|SET>")
			("o,output", " (string) If empty: Override input file", cxxopts::value<std::string>(), "<path/to/file>")
			("q,questlog", " (int) QuestLog Index (0 based)", cxxopts::value<std::uint16_t>()->default_value("0"), "<n>")
			("fix", " (bool) Correct checksum?", cxxopts::value<bool>()->default_value("false"))
			("hex", " (bool) Output in Hexadecimal format?", cxxopts::value<bool>()->default_value("false"));

		options.add_options("Offset")
			("offset", "*(hex/dec) QuestLog offset", cxxopts::value<uint16_t>(), "<0x000>")
			("length", "*(hex/dec) Length", cxxopts::value<uint16_t>(), "<0x00>")
			("type", "*(string) Value type/interpretation", cxxopts::value<std::string>(), "<int|flg|uflg|str>")
			("value", "(string) The value you want to apply", cxxopts::value<std::string>()->default_value("0"), "<\"Link\">");

		options.add_options("Additional")
			("v,version", " Display current version information")
			("verbose", " Print additional output information", cxxopts::value<bool>()->default_value("false"))
			("f,force", " Don't ask for file overrides", cxxopts::value<bool>()->default_value("false"));

		auto result = options.parse(argc, argv);

		if (result.count("help"))
		{
			std::cout << options.help({ "General", "Offset", "Additional" }) << std::endl;
			exit(0);
		}

		if (result.count("version"))
		{
			std::cout << "Twilight Editor (" << OS_STRING << ") v" << TE_VERSION_MAIN << "." << TE_VERSION_SUB << " (C) AECX 2019" << std::endl;
			std::cout << "https://editor.tpspeed.run | https://aecx.cc | https://twitter.com/theAECX" << std::endl;
			exit(0);
		}

		// Set values to global settings struct
		if (result.count("input"))
		{
			TwilightEditor::Settings.INPUT_FILE = result["input"].as<std::string>();
		}
		else
		{
			throw cxxopts::OptionException("Missing parameter --input");
		}

		TwilightEditor::Settings.MODE = (result["mode"].as<std::string>() == "GET" ? TwilightEditor::mode::GET : TwilightEditor::mode::SET);

		if (result.count("output"))
		{
			TwilightEditor::Settings.OUTPUT_FILE = result["output"].as<std::string>();
		}
		else
		{
			// Overwrite original
			TwilightEditor::Settings.OUTPUT_FILE = TwilightEditor::Settings.INPUT_FILE;
		}

		TwilightEditor::Settings.QUESTLOG = result["questlog"].as<std::uint16_t>();
		if (TwilightEditor::Settings.QUESTLOG > 2)
		{
			throw cxxopts::OptionException("Invalid QuestLog Index");
		}

		if (result.count("hex"))
		{
			TwilightEditor::Settings.HEX = result["hex"].as<bool>();
		}

		TwilightEditor::Settings.FIX = result["fix"].as<bool>();

		if (result.count("offset"))
		{
			TwilightEditor::Settings.OFFSET = result["offset"].as<uint16_t>();
		}
		else
		{
			throw cxxopts::OptionException("Missing parameter --offset");
		}

		if (result.count("length"))
		{
			TwilightEditor::Settings.LENGTH = result["length"].as<uint16_t>();
		}
		else
		{
			throw cxxopts::OptionException("Missing parameter --length");
		}

		// --type
		if (result.count("type"))
		{
			TwilightEditor::Settings.TYPE = (result["type"].as<std::string>() == "int" ? TwilightEditor::offsetType::TPINT : result["type"].as<std::string>() == "flg" ? TwilightEditor::offsetType::TPFLAG : result["type"].as<std::string>() == "uflg" ? TwilightEditor::offsetType::TPUFLAG : TwilightEditor::offsetType::TPSTRING);
			if (TwilightEditor::Settings.TYPE == TwilightEditor::offsetType::TPFLAG || TwilightEditor::Settings.TYPE == TwilightEditor::offsetType::TPUFLAG)
			{
				fastPrint("Changed size to 1 since you're supplying a flag!");
				TwilightEditor::Settings.LENGTH = 1;
			}
		}

		// --value
		if (result.count("value"))
		{
			TwilightEditor::Settings.STR_VALUE = result["value"].as<std::string>();
		}

		// -v,	--verbose
		TwilightEditor::Settings.VERBOSE = result["verbose"].as<bool>();

		// -f,	--force
		TwilightEditor::Settings.FORCE = result["force"].as<bool>();

		return result;
	}
	catch (const cxxopts::OptionException& e)
	{
		std::cout << "error parsing options: " << e.what() << std::endl << "Maybe try --help" << std::endl;
		exit(1);
	}
}

namespace TwilightEditor
{
	FILE* currentFilePtr = NULL;
	std::size_t fileSize = 0;
	uint16_t questLogSize = 0;
	uint32_t questLogReadOffset = 0;

	std::string get()
	{
		try
		{
			if ((Settings.OFFSET + questLogReadOffset) + Settings.LENGTH > fileSize)
			{
				throw std::runtime_error("offset + length exceeds the filesize");
			}

			if (Settings.OFFSET + Settings.LENGTH > questLogSize)
			{
				throw std::runtime_error("offset + length exceeds the QuestLog size");
			}

			uint8_t* buffer = new uint8_t[Settings.LENGTH];
			fseek(currentFilePtr, questLogReadOffset + Settings.OFFSET, SEEK_SET);
			fread(buffer, sizeof(uint8_t), Settings.LENGTH, currentFilePtr);

			return TwilightEditor::Converter::toStr(buffer, Settings.TYPE, Settings.LENGTH, Converter::u8(Settings.STR_VALUE));
		}
		catch (const std::runtime_error& ex)
		{
			std::cerr << "error getting from file: " << ex.what() << std::endl;
			exit(1);
		}
		catch (...)
		{
			std::cerr << "unknown error while getting from file" << std::endl;
			exit(1);
		}
	}

	void set()
	{
		try
		{
			// write --value to --offset[--length]
			// Save into --output

			if (fs::exists(Settings.OUTPUT_FILE))
			{
				if (!Settings.FORCE && Settings.INPUT_FILE != Settings.OUTPUT_FILE)
				{
					std::cout << "Output file already exists, do you want to override it? (y/n)" << std::endl << "Override? ";
					int result = getchar();

					if (result != 'Y' && result != 'y')
					{
						std::cout << "Exiting application..." << std::endl;
						exit(1);
					}
				}
			}

			FILE* outputFile = fopen(Settings.OUTPUT_FILE.c_str(), "wb");

			if (outputFile == NULL)
			{
				throw std::runtime_error("couldn't create output file");
			}

			// Templates
			uint16_t t16 = 0;
			uint32_t t32 = 0;

			// buffer for str
			char* data = new char[Settings.LENGTH];
			memset(data, NULL, Settings.LENGTH);

			// buffer for single char
			unsigned* buffer = new unsigned[1];

			std::size_t i = 0;
			for (i; i < fileSize; i++)
			{
				// Loop through entire file
				if (i == Settings.OFFSET)
				{
					// Modify this part

					// TODO convert --value to big endian and write it
					if (Settings.TYPE == offsetType::TPINT)
					{
						switch (Settings.LENGTH)
						{
							case 1:
								// Big endian == little endian, no conversion
								buffer[0] = Converter::u8(Settings.STR_VALUE);
								
								fwrite(buffer, sizeof(uint8_t), 1, outputFile);
							break;

							case 2:
								t16 = Converter::u16(Settings.STR_VALUE);
								t16 = ((t16 & 0x00FF) << 8 | (t16 & 0xFF00) >> 8);
								
								fwrite(&t16, sizeof(uint16_t), 1, outputFile);
							break;

							case 4:
								t32 = Converter::u32(Settings.STR_VALUE);
								t32 = ((t32 & 0xFF000000) >> 24 |
									  (t32 & 0x000000FF) << 24 |
									  (t32 & 0x00FF0000) >> 8 |
									  (t32 & 0x0000FF00) << 8);

								fwrite(&t32, sizeof(uint32_t), 1, outputFile);
							break;

							default:
								throw std::runtime_error("invalid length supplied for type int");
							break;
						}
					} // Integer

					else if (Settings.TYPE == offsetType::TPFLAG || Settings.TYPE == offsetType::TPUFLAG)
					{
						unsigned flag = Converter::u8(Settings.STR_VALUE);
						if (Settings.TYPE == offsetType::TPFLAG)
						{
							// Set the flag
							buffer[0] = fgetc(currentFilePtr) | flag;
						}
						else
						{
							// Unset the flag
							buffer[0] = (fgetc(currentFilePtr) & ~flag);
						}
						
						fwrite(buffer, sizeof(char), 1, outputFile);
					} // Flag

					else
					{
						std::strncpy(data, Settings.STR_VALUE.c_str(), Settings.LENGTH);

						fwrite(data, sizeof(char), Settings.LENGTH, outputFile);
					} // String/Data

					// Continue after the offset + length
					i += (Settings.LENGTH - 1); // - 1 because at the end of the loop it's ++ agian
					fseek(currentFilePtr, Settings.LENGTH, SEEK_CUR);
				}
				else
				{
					// Write next byte to input file
					buffer[0] = fgetc(currentFilePtr);
					fwrite(reinterpret_cast<void*>(buffer), sizeof(char), 1, outputFile);
				}
			}

			fclose(outputFile);
		}
		catch (const std::runtime_error& ex)
		{
			std::cerr << "error setting to file: " << ex.what() << std::endl;
			exit(1);
		}
		catch (...)
		{
			std::cerr << "unknown error while setting to file" << std::endl;
			exit(1);
		}
	}

	void initFile()
	{
		try
		{
			if (!fs::exists(Settings.INPUT_FILE))
			{
				throw std::runtime_error("file does not exist");
			}

			fastPrint("Opening file for reading...");
			currentFilePtr = fopen(Settings.INPUT_FILE.c_str(), "r");
			fs::path filePath{ Settings.INPUT_FILE };

			if (currentFilePtr == NULL)
			{
				throw std::runtime_error("file does not exist or is invalid");
			}

			fileSize = static_cast<std::size_t>(fs::file_size(filePath));
			
			switch (fileSize)
			{
				case 0x8040:
					// GCI/Container
					fastPrint("Operating in GCI mode");
					questLogSize = 0xA94;
					switch (Settings.QUESTLOG)
					{
						case 2:
							questLogReadOffset = 0x5570;
						break;

						case 1:
							questLogReadOffset = 0x4ADC;
						break;

						default:
							questLogReadOffset = 0x4048;
						break;
					}
				break;

				case 0xE00:
					// WiiU/QuestLog
					fastPrint("Operating in Wii U mode");
					questLogSize = 0xE00;
					questLogReadOffset = 0;
				break;

				default:
					if (!Settings.FORCE)
					{
						std::cout << "File could not be identified, do you want to treat it as a single QuestLog? (y/n)" << std::endl << "Is this a Questlog? ";
						int result = getchar();
						if (result != 'Y' && result != 'y')
						{
							std::cout << "Exiting application..." << std::endl;
							exit(1);
						}
						questLogSize = static_cast<uint16_t>(fileSize);
						questLogReadOffset = 0;
					}
				break;
			}
		}
		catch (const std::runtime_error& ex)
		{
			std::cerr << "error opening file " << Settings.INPUT_FILE  << ": " << ex.what() << std::endl;
			exit(1);
		}
		catch (...)
		{
			std::cerr << "unknown error opening file " << Settings.INPUT_FILE << std::endl;
			exit(1);
		}
	}
}