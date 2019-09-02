#include "Twilight Editor.h"
#include "settings.h"
#include "cxxopts.h"
#include "config.h"

namespace TwilightEditor
{
	settings Settings;
}

cxxopts::ParseResult parse(int argc, char* argv[])
{
	try
	{
		cxxopts::Options options(argv[0], "Twilight Editor | (C) @theAECX");

		options.add_options("General")
			("h,help", "Prints this help")
			("i,input", "Path to input file", cxxopts::value<std::string>(), "[path/to/file]")
			("o,output", "If empty: Override input file", cxxopts::value<std::string>(), "[path/to/file]")
			("q,quest-log", "QuestLog Index (0 based)", cxxopts::value<std::uint8_t>()->default_value("0"), "[n]")
			("m,mode", "Specifies whether to Read or Write", cxxopts::value<std::string>(), "[GET|SET]")
			("f,fix", "Correct checksums?", cxxopts::value<bool>()->default_value("true"));

		options.add_options("Offset")
			("offset", "Hexadecimal QuestLog offset", cxxopts::value<std::string>(), "[0x000]")
			("length", "Hexadecimal Length", cxxopts::value<std::string>(), "[0x00]")
			("type", "Value type ('tpflag', 'tpint', 'tpstring')", cxxopts::value<std::string>(), "[tpflag|tpint|tpstring]");

		options.add_options("Additional")
			("v,verbose", "Print additional output information", cxxopts::value<bool>());

		auto result = options.parse(argc, argv);

		if (result.count("help"))
		{
			std::cout << options.help({ "General", "Offset", "Additional" }) << std::endl;
			exit(0);
		}

		if (result.count("input"))
		{
			TwilightEditor::Settings.INPUT_FILE = result["input"].as<std::string>();
		}

		return result;

	}
	catch (const cxxopts::OptionException& e)
	{
		std::cout << "error parsing options: " << e.what() << std::endl;
		exit(1);
	}
}

int main(int argc, char* argv[])
{
	auto result = parse(argc, argv);
	auto arguments = result.arguments();
	
	std::cout << TwilightEditor::Settings.INPUT_FILE << std::endl;

	getchar();

	return 0;
}