#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

int main() 
{
	std::fstream file("20240414_ACTIVE.log", std::ios::in);
	std::fstream output("20240414_ACTIVE_output.log",std::ios::out | std::ios::app);

	if (!file.is_open()) 
	{
		return 1;
	}

	std::string line;
	std::vector<std::string> lines;

	while (std::getline(file, line)) 
	{
		lines.push_back(line);
	}

	for (size_t i = 0; i < lines.size(); ++i) 
	{
		std::string& currentLine = lines[i];
		int lineNumber = static_cast<int>(i) + 1; // 현재 줄 번호

		std::regex linePattern(R"(RUN\s+Handler\s+PICK_03_AA_R_PICKUP\s+END\s+PICK_03_AA_R_PICKUP_END)");
		std::vector<std::regex> linePatterns{
			std::regex(R"(RUN\s+EVENT\s+AUTO\s+SET\s+PICK_03_AA_R_PICKUP_CHECK)"),
			std::regex(R"(RUN\s+EVENT\s+AUTO\s+SET\s+PICK_03_AA_R_PICKUP_MOVE)"),
			std::regex(R"(RUN\s+EVENT\s+AUTO\s+SET\s+PICK_03_AA_R_PICKUP_SEARCH)"),
			std::regex(R"(RUN\s+Handler\s+PICK_03_AA_R_PICKUP\s+START\s+PICK_03_AA_R_PICKUP_START)"),

		};

		std::regex numberPattern(R"(Pcb\sWork\sL\s(\d+))");
		if (std::regex_search(currentLine, linePattern)) 
		{
			std::smatch match;
			std::regex pattern("Pcb Work L (\\d+)");
			if (!std::regex_search(currentLine, match, pattern))
				continue;

			int num = std::stoi(match[1]);

			for (int j = 4; j > 0; --j) 
			{
				int targetLineNumber = lineNumber - j;
				if (targetLineNumber >= 1)
				{
					if (!std::regex_search(lines[targetLineNumber - 1], linePatterns[j - 1]))
					{
						continue;
					}

					std::string replaced_line = std::regex_replace(lines[targetLineNumber - 1], numberPattern, "Pcb Work L " + std::to_string(num));

					lines[targetLineNumber - 1] = replaced_line;
				}
			}
		}
	}

	output.seekp(0);
	for (const auto& line : lines) 
	{
		output << line << '\n';
	}

	file.close();
	output.close();

	return 0;
}
