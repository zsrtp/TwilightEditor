#include "Twilight Editor.h"
#include "settings.h"
#include "cxxopts.h"
#include "config.h"

#include <filesystem>

namespace fs = std::filesystem;
namespace TE = TwilightEditor;

namespace TwilightEditor
{
	settings Settings;
}

cxxopts::ParseResult parse(int argc, char* argv[])
{
	try
	{
		cxxopts::Options options(argv[0], "Twilight Editor | (C) @theAECX | *required");

		options.add_options("General")
			("h,help", " Prints this help")
			("i,input", "*(string) Path to input file", cxxopts::value<std::string>(), "<path/to/file>")
			("m,mode", " (string) <GET> or <SET>", cxxopts::value<std::string>()->default_value("GET"), "<GET|SET>")
			("o,output", " (string) If empty: Override input file", cxxopts::value<std::string>(), "<path/to/file>")
			("q,questlog", " (int) QuestLog Index (0 based)", cxxopts::value<std::uint16_t>()->default_value("0"), "<n>")
			("fix", " (bool) Correct checksum?", cxxopts::value<bool>()->default_value("false"));

		options.add_options("Offset")
			("offset", "*(hex/dec) Hexadecimal QuestLog offset", cxxopts::value<uint16_t>(), "<0x000>")
			("length", "*(hex/dec) Hexadecimal Length", cxxopts::value<uint16_t>(), "<0x00>")
			("type", "*(string) Value type/interpretation", cxxopts::value<std::string>(), "<int|flg|str>")
			("value", "*(string) The value you want to apply", cxxopts::value<std::string>(), "<\"Link\">");

		options.add_options("Additional")
			("v,verbose", " Print additional output information", cxxopts::value<bool>()->default_value("false"))
			("f,force", " Don't ask for file overrides", cxxopts::value<bool>()->default_value("false"));

		auto result = options.parse(argc, argv);

		if (result.count("help"))
		{
			std::cout << options.help({ "General", "Offset", "Additional" }) << std::endl;
			exit(0);
		}

		// Set values to global settings struct
		if (result.count("input"))
		{
			TE::Settings.INPUT_FILE = result["input"].as<std::string>();
		}
		else
		{
			throw cxxopts::OptionException("Missing parameter --input");
		}

		TE::Settings.MODE = (result["mode"].as<std::string>() == "GET" ? TE::mode::GET : TE::mode::SET);

		if (result.count("output"))
		{
			TE::Settings.OUTPUT_FILE = result["output"].as<std::string>();
		}
		else
		{
			// Overwrite original
			TE::Settings.OUTPUT_FILE = TE::Settings.INPUT_FILE;
		}

		TE::Settings.QUESTLOG = result["questlog"].as<std::uint16_t>();
		if (TE::Settings.QUESTLOG > 2)
		{
			throw cxxopts::OptionException("Invalid QuestLog Index");
		}

		TE::Settings.FIX = result["fix"].as<bool>();

		if (result.count("offset"))
		{
			TE::Settings.OFFSET = result["offset"].as<uint16_t>();
		}
		else
		{
			throw cxxopts::OptionException("Missing parameter --offset");
		}

		if (result.count("length"))
		{
			TE::Settings.LENGTH = result["length"].as<uint16_t>();
		}
		else
		{
			throw cxxopts::OptionException("Missing parameter --length");
		}

		if (result.count("type"))
		{
			TE::Settings.TYPE = (result["type"].as<std::string>() == "int" ? TE::offsetType::TPINT : result["type"].as<std::string>() == "flg" ? TE::offsetType::TPFLAG : TE::offsetType::TPSTRING);
		}

		if (result.count("value"))
		{
			TE::Settings.VALUE = result["value"].as<std::string>();
		}
		else
		{
			throw cxxopts::OptionException("Missing parameter --value");
		}

		// -v,	--verbose
		TE::Settings.VERBOSE = result["verbose"].as<bool>();

		// -f,	--force
		TE::Settings.FORCE = result["force"].as<bool>();

		return result;
	}
	catch (const cxxopts::OptionException& e)
	{
		std::cout << "error parsing options: " << e.what() << std::endl << "Maybe try --help" << std::endl;
		exit(1);
	}
}

int main(int argc, char* argv[])
{
	auto result = parse(argc, argv);
	auto arguments = result.arguments();

	std::cout << "Results:" << std::endl;
	std::cout << "Input File: " << TE::Settings.INPUT_FILE << std::endl;
	std::cout << "Output File: " << TE::Settings.OUTPUT_FILE << std::endl;
	std::cout << "Mode: " << TE::Settings.MODE << std::endl;
	std::cout << "QuestLog: " << TE::Settings.QUESTLOG << std::endl;
	std::cout << "Fix checksum: " << TE::Settings.FIX << std::endl;


	std::cout << "Offset: " << TE::Settings.OFFSET << std::endl;
	std::cout << "Length: " << TE::Settings.LENGTH << std::endl;
	std::cout << "Type: " << TE::Settings.TYPE << std::endl;
	std::cout << "Value: " << TE::Settings.VALUE << std::endl;

	std::cout << "Verbose: " << TE::Settings.VERBOSE << std::endl;
	std::cout << "Force: " << TE::Settings.FORCE << std::endl;

	getchar();

	return 0;
}