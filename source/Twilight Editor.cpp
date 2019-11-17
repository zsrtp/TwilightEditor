#include "Twilight Editor.h"


int main(int argc, char* argv[])
{
	/*
	auto result = parse(argc, argv);
	auto arguments = result.arguments();

	if (TwilightEditor::Settings.VERBOSE)
	{
		std::cout << "Settings:" << std::endl;
		std::cout << "Input File: " << TwilightEditor::Settings.INPUT_FILE << std::endl;
		std::cout << "Output File: " << TwilightEditor::Settings.OUTPUT_FILE << std::endl;
		std::cout << "Mode: " << TwilightEditor::Settings.MODE << std::endl;
		std::cout << "QuestLog: " << TwilightEditor::Settings.QUESTLOG << std::endl;


		std::cout << "Offset: " << TwilightEditor::Settings.OFFSET << std::endl;
		std::cout << "Length: " << TwilightEditor::Settings.LENGTH << std::endl;
		std::cout << "Type: " << TwilightEditor::Settings.TYPE << std::endl;
		std::cout << "Value: " << TwilightEditor::Settings.STR_VALUE << std::endl;

		std::cout << "Verbose: " << TwilightEditor::Settings.VERBOSE << std::endl;
		std::cout << "Force: " << TwilightEditor::Settings.FORCE << std::endl;
	}
	*/
	
	TwilightEditor::Settings.INPUT_FILE = "new.file";
	TwilightEditor::Settings.OUTPUT_FILE = "new.tp";
	TwilightEditor::Settings.FORCE = true;
	TwilightEditor::Settings.LENGTH = 1;
	TwilightEditor::Settings.OFFSET = 0x0A;
	TwilightEditor::Settings.TYPE = TwilightEditor::offsetType::TPUINT;
	TwilightEditor::Settings.MODE = TwilightEditor::mode::GET;

	fastPrint("initializing file...");

	TwilightEditor::initFile();

	TwilightEditor::readChecksums();
	
	switch (TwilightEditor::Settings.MODE)
	{
		case TwilightEditor::mode::GET:
			fastPrint("> GET");

			std::cout << "== BEGIN RESULT ==" << std::endl << TwilightEditor::get() << std::endl << "== END RESULT == "<< std::endl;
		break;

		case TwilightEditor::mode::SET:
			fastPrint("> SET");
			TwilightEditor::set();
			TwilightEditor::setChecksums();
			TwilightEditor::saveFile();
		break;
	}

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
			("hex", " (bool) Output in Hexadecimal format?", cxxopts::value<bool>()->default_value("false"));

		options.add_options("Offset")
			("offset", "*(hex/dec) QuestLog offset", cxxopts::value<uint16_t>(), "<0x000>")
			("length", "*(hex/dec) Length", cxxopts::value<uint16_t>(), "<0x00>")
			("type", "*(string) Value type/interpretation", cxxopts::value<std::string>(), "<uint|sint|flg|uflg|str>")
			("value", "(string) The value you want to apply", cxxopts::value<std::string>()->default_value("0"), "<\"Link\">");

		options.add_options("Additional")
			("v,version", " Display current version information")
			("verbose", " Print additional output information", cxxopts::value<bool>()->default_value("false"))
			("f,force", " Interpret file as raw questlog", cxxopts::value<bool>()->default_value("false"));

		auto result = options.parse(argc, argv);

		// -h, --help
		if (result.count("help"))
		{
			std::cout << options.help({ "General", "Offset", "Additional" }) << std::endl;
			exit(0);
		}
		
		// -v, --version
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
			TwilightEditor::Settings.TYPE = (result["type"].as<std::string>() == "uint" ? TwilightEditor::offsetType::TPUINT : result["type"].as<std::string>() == "sint" ? TwilightEditor::offsetType::TPSINT : result["type"].as<std::string>() == "flg" ? TwilightEditor::offsetType::TPFLAG : result["type"].as<std::string>() == "uflg" ? TwilightEditor::offsetType::TPUFLAG : TwilightEditor::offsetType::TPSTRING);
			if (TwilightEditor::Settings.TYPE == TwilightEditor::offsetType::TPFLAG || TwilightEditor::Settings.TYPE == TwilightEditor::offsetType::TPUFLAG)
			{
				fastPrint("Forced size to 1 since you're supplying a flag!");
				TwilightEditor::Settings.LENGTH = 1;
			}
		}

		// --value
		if (result.count("value"))
		{
			TwilightEditor::Settings.STR_VALUE = result["value"].as<std::string>();
		}

		// --verbose
		TwilightEditor::Settings.VERBOSE = result["verbose"].as<bool>();

		// -f,	--force
		TwilightEditor::Settings.FORCE = result["force"].as<bool>();

		return result;
	}
	catch (const cxxopts::OptionException& e)
	{
		std::cerr << "error parsing options: " << e.what() << std::endl << "Maybe try --help" << std::endl;
		exit(1);
	}
}

namespace TwilightEditor
{
	FILE* currentFilePtr = nullptr;

	// QL Data including checksums
	uint8_t* questLogData = nullptr;

	uint8_t* inputFileBuf = nullptr;
	uint8_t* outputFileBuf = nullptr;

	uintmax_t fileSize = 0;
	uint16_t questLogLength = 0;
	uint32_t questLogReadOffset = 0;

	std::string get()
	{
		try
		{
			if ((Settings.OFFSET + Settings.LENGTH) > questLogLength)
			{
				throw std::runtime_error("Read offset + length exceed the QuestLog");
			}

			return Converter::toStr(&questLogData[Settings.OFFSET], Settings.TYPE, Settings.LENGTH, Settings.HEX, Converter::u8(Settings.STR_VALUE));
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
			if ((Settings.OFFSET + Settings.LENGTH) > questLogLength)
			{
				throw std::runtime_error("Read offset + length exceed the QuestLog");
			}

			// Pointer to where we want to write the Settings.VALUE
			void* target = &questLogData[Settings.OFFSET];

			// Written data counter
			int i = Settings.LENGTH;

			switch (Settings.TYPE)
			{
				case offsetType::TPUINT:
					switch (Settings.LENGTH)
					{
					case 4:
						*reinterpret_cast<uint32_t*>(target) = Converter::bigEndian(Converter::u32(Settings.STR_VALUE));
						break;

					case 2:
						*reinterpret_cast<uint16_t*>(target) = Converter::bigEndian(Converter::u16(Settings.STR_VALUE));
						break;

					default:
						*reinterpret_cast<uint8_t*>(target) = Converter::u8(Settings.STR_VALUE);
						break;
					}
					// Set uInt
				break;

				case offsetType::TPSINT:
					switch (Settings.LENGTH)
					{
					case 4:
						*reinterpret_cast<int32_t*>(target) = Converter::bigEndian(Converter::s32(Settings.STR_VALUE));
						break;

					case 2:
						*reinterpret_cast<int16_t*>(target) = Converter::bigEndian(Converter::s16(Settings.STR_VALUE));
						break;

					default:
						*reinterpret_cast<int8_t*>(target) = Converter::s8(Settings.STR_VALUE);
						break;
					}
					// Set sInt
				break;

				case offsetType::TPFLAG:
					Converter::flag(reinterpret_cast<uint8_t*>(target), Settings.STR_VALUE, true);
					// Set flag
					break;

				case offsetType::TPUFLAG:
					Converter::flag(reinterpret_cast<uint8_t*>(target), Settings.STR_VALUE, false);
					// Unset flag
					break;

				case offsetType::TPSTRING:
					// Fill 0
					memset(target, 0x0, Settings.LENGTH);
					strncpy(reinterpret_cast<char*>(target), Settings.STR_VALUE.c_str(), Settings.LENGTH);

					// Write string
					break;
			}

			std::cout << "Total bytes written: " << i << std::endl;
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
			currentFilePtr = fopen(Settings.INPUT_FILE.c_str(), "rb");

			if (currentFilePtr == NULL)
			{
				throw std::runtime_error("couldn't open input file for read");
			}

			fseek(currentFilePtr, 0, SEEK_END);
			fileSize = ftell(currentFilePtr);

			// Back to 0 now that we know the length
			fseek(currentFilePtr, 0, SEEK_SET);

			switch (fileSize)
			{
				case 0x8040:
					// GCI/Container
					fastPrint("Operating in GCI mode (0xA94)");
					questLogLength = 0xA94;
					switch (Settings.QUESTLOG)
					{
						case 2:
							questLogReadOffset = 0x5570;
						break;

						case 1:
							questLogReadOffset = 0x4ADC;
						break;

						case 0:
							questLogReadOffset = 0x4048;
						break;

						default:
							throw std::runtime_error("invalid Quest Log index");
						break;
					}
				break;

				case 0xE00:
					// WiiU/QuestLog
					fastPrint("Operating in Wii U mode (0xE00)");
					questLogLength = 0xE00;
					questLogReadOffset = 0;
				break;

				default:
					if (!Settings.FORCE)
					{
						throw std::runtime_error("File could not be identified as a valid TP save. Use -f,--force to treat it as a QuestLog");
					}
					else
					{
						fastPrint("Operating in QuestLog mode (QL = FileSize)");
						questLogLength = fileSize;
						questLogReadOffset = 0;
					}
				break;
			}

			// Set filebuffers
			inputFileBuf = new uint8_t[fileSize];
			outputFileBuf = new uint8_t[fileSize];


			fread(inputFileBuf, sizeof(uint8_t), fileSize, currentFilePtr);
			fclose(currentFilePtr);

			memcpy(outputFileBuf, inputFileBuf, fileSize);

			// Directly edit the output file
			questLogData = &outputFileBuf[questLogReadOffset];
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

	void saveFile()
	{
		try
		{
			currentFilePtr = fopen(Settings.OUTPUT_FILE.c_str(), "wb");

			if (currentFilePtr == NULL)
			{
				throw std::runtime_error("couldn't open input file for write");
			}

			fseek(currentFilePtr, 0, SEEK_SET);

			// Questlogdata is a pointer to outputfilebuf so it should already be contained in here
			fwrite(outputFileBuf, sizeof(uint8_t), fileSize, currentFilePtr);
			
			fclose(currentFilePtr);

			std::cout << "Contents written to: '" << Settings.OUTPUT_FILE << "'" << std::endl;
		}
		catch (const std::runtime_error &ex)
		{
			std::cerr << "error saving file " << Settings.OUTPUT_FILE << ": " << ex.what() << std::endl;
			exit(1);
		}
		catch (...)
		{
			std::cerr << "unknown error saving file " << Settings.OUTPUT_FILE << std::endl;
			exit(1);
		}
	}

	// Reads current active checksums (no calculation)
	void readChecksums()
	{
		try
		{
			std::cout << "Reading checksums:" << std::endl;
			std::cout << "+Checksum: " << Converter::toStr(&questLogData[questLogLength - 8], TwilightEditor::offsetType::TPSINT, 4, true) << std::endl;
			std::cout << "-Checksum: " << Converter::toStr(&questLogData[questLogLength - 4], TwilightEditor::offsetType::TPSINT, 4, true) << std::endl;
		}
		catch (const std::runtime_error & ex)
		{
			std::cerr << "error reading checksums " << Settings.INPUT_FILE << " QuestLog: " << Settings.QUESTLOG << ": " << ex.what() << std::endl;
			exit(1);
		}
		catch (...)
		{
			std::cerr << "unknown error while reading checksums " << Settings.INPUT_FILE << " QuestLog: " << Settings.QUESTLOG << std::endl;
			exit(1);
		}
	}

	void setChecksums()
	{
		try
		{
			uint32_t* csum = reinterpret_cast<uint32_t*>(&questLogData[questLogLength - 8]);
			uint32_t* nsum = reinterpret_cast<uint32_t*>(&questLogData[questLogLength - 4]);

			checkSum(questLogData, questLogLength - 8, csum, nsum);

			// Update endianess
			*csum = Converter::bigEndian(*csum);
			*nsum = Converter::bigEndian(*nsum);

			std::cout << "New checksums:" << std::endl;
			std::cout << "+Checksum: " << Converter::toStr(&questLogData[questLogLength - 8], TwilightEditor::offsetType::TPSINT, 4, true) << std::endl;
			std::cout << "-Checksum: " << Converter::toStr(&questLogData[questLogLength - 4], TwilightEditor::offsetType::TPSINT, 4, true) << std::endl;
		}
		catch (...)
		{
			std::cerr << "unknown error calculating checksums" << std::endl;
			exit(1);
		}
	}
}