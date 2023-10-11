// essentially a C++ port of find-implemented-args.py

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>
#include <unordered_map>
#include <algorithm>

// function to replace hyphens with underscores
std::string replace_hyphens_with_underscores(const std::string& input) {
    std::string result = input;
    for (size_t i = 0; i < result.length(); i++) {
        if (result[i] == '-') {
            result[i] = '_';
        }
    }
    return result;
}

// function to replace underscores with hyphens
std::string replace_underscores_with_hyphens(const std::string& input) {
    std::string result = input;
    for (size_t i = 0; i < result.length(); i++) {
        if (result[i] == '_') {
            result[i] = '-';
        }
    }
    return result;
}

// The function to replace dashes with underscores in a file
void replace_dashes_with_underscores(const std::string& filename) {
    std::ifstream in_file(filename);
    std::string content, replacedContent;

    if (in_file) {
        std::stringstream buffer;
        buffer << in_file.rdbuf();
        content = buffer.str();

        replace_hyphens_with_underscores(content);

        in_file.close();

        std::ofstream out_file(filename);
        if (out_file) {
            out_file << replacedContent;
            out_file.close();
        } else {
            std::cerr << "Failed to open output file." << std::endl;
        }
    } else {
        std::cerr << "Failed to open input file." << std::endl;
    }
}

// the function(s) to capture everything between quotes in print statements)
bool is_in_print_statement(const std::string& line) {
    return line.find("printf(") != std::string::npos;
}

std::string extract_print_content(const std::string& line) {
    size_t start = line.find("\"") + 1;
    size_t end = line.find_last_of("\"");
    if (start != std::string::npos && end != std::string::npos && end > start) {
        return line.substr(start, end - start);
    }
    return "";
}

// The function to update the source file
void update_file(const std::string& file_from, const std::string& file_to = "c_help_list.txt") {
    std::ifstream in_file(file_from);
    std::ofstream out_file(file_to);

    if (in_file && out_file) {
        std::string line;

        while (std::getline(in_file, line)) {
            if (is_in_print_statement(line)) {
                std::string content = extract_print_content(line);
                printf("Content: %s",content.c_str());
            }
        }
        in_file.close();
        out_file.close();
    } else {
        std::cerr << "Failed to open files." << std::endl;
    }
}

// match the params strings up to the stipulated next character
std::string capture_after_params(const std::string& content) {
    std::size_t startPos = std::string::npos;
    std::string delimiter = " \n})(,>;";

    if (content.find("params->") != std::string::npos) {
        startPos = content.find("params->") + 8;  // Adjust for the length of "params->"
    } else if(content.find("params.") != std::string::npos) {
        startPos = content.find("params.") + 7;   // Adjust for the length of "params."
    }

    if (startPos != std::string::npos) {
        std::size_t endPos = content.find_first_of(delimiter, startPos);
        if (endPos != std::string::npos) {
            return content.substr(startPos, endPos - startPos);
        } else {
            return content.substr(startPos);  // Capture until end of string
        }
    }
    return "";  // Pattern not found
}

// The function to find arguments in a directory
std::unordered_map<std::string, std::unordered_set<std::string>> find_arguments(const std::string& directory) {
    std::unordered_map<std::string, std::unordered_set<std::string>> arguments;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.path().extension().string() == ".cpp") {
            std::ifstream in_file(entry.path());

            if (in_file) {
                std::stringstream buffer;
                buffer << in_file.rdbuf();
                std::string content = buffer.str();

                std::unordered_set<std::string> matches;

                std::size_t pos = 0;
                while (pos != std::string::npos) {
                    pos = content.find_first_of("params.", pos);
                    if (pos != std::string::npos) {
                        matches.insert(capture_after_params(content.substr(pos)));
                        pos++;
                    }
                    pos = content.find_first_of("params->", pos);
                    if (pos != std::string::npos) {
                        matches.insert(capture_after_params(content.substr(pos)));
                        pos++;
                    }
                }

                for (const auto& match : matches) {
                    std::cout << match << std::endl;
                }

                    in_file.close();
        } else {
                        std::cerr << "Failed to open file: " << entry.path().string() << std::endl;
                        }
                    }
                }
    return arguments;
}

// The function to output the results
void output_results(const std::unordered_map<std::string, std::unordered_set<std::string>>& result) {
    std::cout << "Filename: help_list.txt, arguments: " << std::endl;

    std::unordered_set<std::string> all_arguments;

    for (const auto& [filename, arguments] : result) {
        all_arguments.insert(arguments.begin(), arguments.end());
    }

    for (const auto& argument : all_arguments) {
        std::cout << argument << " ";
    }

    std::cout << std::endl;
}

// The function to concatenate elements after "//"
std::string concatenate(const std::vector<std::string>& v) {
    std::string concatenated_element;
    bool concatenate = false;

    for (const auto& element : v) {
        if (concatenate) {
            concatenated_element += " " + element;
        } else if (element == "//") {
            concatenated_element = element;
            concatenate = true;
        }
    }

    return concatenated_element;
}

// The function to find parameters in the help file
void find_parameters(const std::string& file, const std::unordered_map<std::string, std::unordered_set<std::string>>& result) {
    std::ifstream help_file(file);
    std::string line;

    if (help_file) {
        std::vector<std::string> lines;
        while (std::getline(help_file, line)) {
            lines.push_back(line);
        }

        std::unordered_map<std::string, std::vector<std::string>> parameters;

        for (const auto& [filename, arguments] : result) {
            std::unordered_set<std::string> substituted_arguments;
            for (const auto& argument : arguments) {
                substituted_arguments.insert(replace_underscores_with_hyphens(argument));
            }

            std::vector<std::string> matched_lines;
            for (const auto& line : lines) {
                for (const auto& argument : substituted_arguments) {
                    std::regex pattern(R"(--)" + argument + R"(\s)|(?:params\.)" + argument + R"(\b(?=[\s\.,;]|\.+\w)))");

                    if (std::regex_search(line, pattern)) {
                        matched_lines.push_back(line);
                    }
                }
            }
            parameters[filename] = matched_lines;
        }

        for (const auto& [filename, matched_lines] : parameters) {
            std::cout << "Filename: " << filename << std::endl;
            std::cout << "Command-line arguments available and gpt-params functions implemented (TODO: multi-line helps NEED SOME WORK):" << std::endl;

            if (matched_lines.empty()) {
                std::cout << "    None" << std::endl;
            } else {
                int helpCount = 0;
                for (const auto& line : matched_lines) {
                    if (line.find("    ") != 0) {
                        helpCount++;
                    }
                    std::cout << helpCount << " help: " << line << std::endl;
                }
            }

            std::cout << std::endl;
        }
        help_file.close();
    } else {
        std::cerr << "Failed to open help file: " << file << std::endl;
    }
}

int main() {
    std::string directory = "/Users/edsilm2/llama.cpp/examples";

    update_file("common/common.cpp", "help_list.txt");
    replace_dashes_with_underscores("help_list.txt");

    auto result = find_arguments(directory);
    output_results(result);
    find_parameters("help_list.txt", result);

    return 0;
}
