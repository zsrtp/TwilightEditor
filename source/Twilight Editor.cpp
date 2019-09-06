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
			("offset", "*(hex/dec) Hexadecimal QuestLog offset", cxxopts::value<uint16_t>(), "<0x000>")
			("length", "*(hex/dec) Hexadecimal Length", cxxopts::value<uint16_t>(), "<0x00>")
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

	std::string get()
	{
		try
		{
			if (Settings.OFFSET + Settings.LENGTH > fileSize)
			{
				throw std::runtime_error("offset + length exceeds the filesize");
			}

			uint8_t* buffer = new uint8_t[Settings.LENGTH];
			fseek(currentFilePtr, Settings.OFFSET, SEEK_SET);
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
				break;

				case 0xE00:
					// WiiU/QuestLog
					fastPrint("Operating in Wii U mode");
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