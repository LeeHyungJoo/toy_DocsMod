#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <list>
#include <chrono>

int main() 
{
	auto lastTime = std::chrono::steady_clock::now();

	std::cout << "파일명 입력하세요. \n:";

	std::string inputFileName;
	std::cin >> inputFileName;

	std::fstream inputFile(inputFileName, std::ios::in);

	if (!inputFile.is_open()) 
	{
		std::cout << "해당 파일을 열 수 없습니다.\n";
		return 1;
	}

	std::string outputFileName;
	auto extensionItr = std::find(inputFileName.begin(), inputFileName.end(), '.');
	outputFileName.append(inputFileName.begin(), extensionItr);
	outputFileName.append("_output");
	outputFileName.append(extensionItr, inputFileName.end());

	std::fstream outputFile(outputFileName, std::ios::out | std::ios::app);
	if (!outputFile.is_open())
	{
		std::cout << "출력 파일을 생성하는데 실패했습니다.\n";
		return 1;
	}


	std::regex linePattern(R"(RUN\s+Handler\s+PICK_03_AA_R_PICKUP\s+END\s+PICK_03_AA_R_PICKUP_END)");
	std::vector<std::regex> linePatterns{
		std::regex(R"(RUN\s+EVENT\s+AUTO\s+SET\s+PICK_03_AA_R_PICKUP_CHECK)"),
		std::regex(R"(RUN\s+EVENT\s+AUTO\s+SET\s+PICK_03_AA_R_PICKUP_MOVE)"),
		std::regex(R"(RUN\s+EVENT\s+AUTO\s+SET\s+PICK_03_AA_R_PICKUP_SEARCH)"),
		std::regex(R"(RUN\s+Handler\s+PICK_03_AA_R_PICKUP\s+START\s+PICK_03_AA_R_PICKUP_START)"),

	};

	std::regex replacePattern(R"(Pcb\sWork\sL\s(\d+))");
	std::string replaceStr("Pcb Work L ");

	std::string line;
	std::vector<std::string> lines;
	std::list<unsigned long long> indices;
	unsigned long long lineCount = 0ULL;


	std::cout << "파일을 로드 중 입니다. \n";
	while (std::getline(inputFile, line)) 
	{
		if (std::regex_search(line, linePattern))
			indices.push_back(lineCount);

		lines.push_back(line);
		lineCount++;
	}

	std::cout << "파일 로드를 완료하였습니다. " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTime).count() << "ms" << std::endl;
	lastTime = std::chrono::steady_clock::now();

	unsigned long long progressCount = 0ULL;
	unsigned long long wholeProgressCount = indices.size();
	unsigned long long lastProgressRatio = 0ULL;
	unsigned long long progressRatio = 0ULL;

	std::cout << "변환 시작합니다.\n";
	for (auto index : indices)
	{
		std::string& currentLine = lines[index];
		int lineNumber = static_cast<int>(index) + 1;

		std::smatch match;
		if (!std::regex_search(currentLine, match, replacePattern))
			continue;

		progressCount++;
		int num = std::stoi(match[1]);

		for (int j = 4; j > 0; --j)
		{
			int targetLineNumber = lineNumber - j;
			if (targetLineNumber >= 1)
			{
				if (!std::regex_search(lines[targetLineNumber - 1], linePatterns[j - 1]))
					continue;

				lines[targetLineNumber - 1] = std::regex_replace(lines[targetLineNumber - 1], replacePattern, replaceStr.append(std::to_string(num)));
			}
		}

		progressRatio = static_cast<unsigned long long>(((double)progressCount / wholeProgressCount) * 100);
		if (progressRatio - lastProgressRatio > 10)
		{
			lastProgressRatio = progressRatio;
			std::cout << progressRatio << '%' <<std::endl;
		}
	}

	std::cout << progressRatio << '%' << std::endl;

	std::cout << "변환을 마쳤습니다. " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTime).count()  << "ms" << std::endl;
	lastTime = std::chrono::steady_clock::now();

	std::cout << "출력 파일을 생성하고 있습니다.\n";
	outputFile.seekp(0);
	for (const auto& line : lines) 
		outputFile << line << '\n';

	inputFile.close();
	outputFile.close();

	std::cout << "모든 작업을 마쳤습니다.\n";
	system("pause");
	return 0;
}
