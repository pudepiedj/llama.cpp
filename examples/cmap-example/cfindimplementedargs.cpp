// essentially a C++ port of find-implemented-args.py

#include <iostream>
#include <cstdio>   // prob unnecessary
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

// function to replace hyphens with underscores
static std::string replace_hyphens_with_underscores(const std::string& input) {
    std::string result = input;
    for (size_t i = 0; i < result.length(); i++) {
        if (result[i] == '-') {
            result[i] = '_';
        }
    }
    return result;
}

// function to replace underscores with hyphens
static std::string replace_underscores_with_hyphens(const std::string& input) {
    std::string result = input;
    for (size_t i = 0; i < result.length(); i++) {
        if (result[i] == '_') {
            result[i] = '-';
        }
    }
    return result;
}

// The function to replace dashes/hyphens with underscores in a file
static void replace_dashes_with_underscores(const std::string& filename) {
    std::ifstream in_file(filename);
    std::string content, replacedContent;

    if (in_file) {
        std::stringstream buffer;
        buffer << in_file.rdbuf();
        content = buffer.str();
        } else {
            std::cerr << "Failed to open input file." << std::endl;
        } // this closes the file which is otherwise made blank

        replacedContent = replace_hyphens_with_underscores(content);

        // this inadvertently zeroes c_help_file.txt
        std::ofstream out_file(filename);
        if (out_file) {
            out_file << replacedContent;
            out_file.close();
        } else {
            std::cerr << "Failed to open output file." << std::endl;
    }
}

// the function(s) to capture everything between quotes in print statements)
static bool is_in_print_statement(const std::string& line) {
    return line.find("    printf(") != std::string::npos;
}

static std::string extract_print_content(const std::string& line) {
    size_t start = line.find("\"") + 1;
    size_t end = line.find_last_of("\"");
    if (start != std::string::npos && end != std::string::npos && end > start) {
        return line.substr(start, end - start);
    } else {
        return "";
    }
}

// The function to update the source file
static void update_help_file(const std::string& file_from, const std::string& file_to) {
    std::ifstream in_file(file_from);
    std::ofstream out_file(file_to, std::ios::out);  // Change the file mode to append

    if (in_file && out_file) {
        std::string line;

        while (std::getline(in_file, line)) {
            if (is_in_print_statement(line)) {
                std::string content = extract_print_content(line);
                printf("Content: %s\n",content.c_str());
                out_file << "Content: " << content << std::endl;  // Write content to the output file
            }
        }
        in_file.close();
        out_file.flush();
        out_file.close();
    } else {
        if (not in_file) {
            std::cerr << "Failed to open in_file." << std::endl;
        }
        if (not out_file) {
            std::cerr << "Failed to open out_file." << std::endl;
        }
    }
}

// match the params strings up to the stipulated next character
static std::string capture_after_params(const std::string& content) {
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
static std::unordered_map<std::string, std::unordered_set<std::string>> find_arguments(const std::string& directory) {
    std::unordered_map<std::string, std::unordered_set<std::string>> arguments;
    std::unordered_set<std::string> parameter_list;

    // fs is defined as the abbrevation for std::filesystem at the top
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.path().extension().string() == ".cpp") {
            std::ifstream in_file(entry.path(), std::ios::in);
            if (!fs::exists(entry.path())) {
                printf("Accessed file does not exist\n"); // Handle the case when file does not exist
            } else {}
                printf("entry path = %s, entry path extension = %s\n", entry.path().extension().c_str(), entry.path().c_str());
                if (in_file) {
                    std::stringstream buffer;
                    buffer << in_file.rdbuf();
                    std::string content = buffer.str();

                    std::unordered_set<std::string> matches;
                    // this seems indiscriminate; we don't want literally everything after params ...
                    std::size_t pos = 0;
                    while (pos != std::string::npos) {
                        pos = content.find_first_of("params.", pos);
                        if (pos != std::string::npos) {
                            matches.insert(capture_after_params(content.substr(pos)));
                            pos++;
                        }
                        // pos = content.find_first_of("params->", pos);
                        // if (pos != std::string::npos) {
                        //     matches.insert(capture_after_params(content.substr(pos)));
                        //    pos++;
                        // }
                    }

                    for (const auto& match : matches) {
                        std::cout << match << std::endl;
                        parameter_list.insert(match);
                    }
                    fs::path p(entry.path());
                    std::cout << "The file name is: " << p.filename() << '\n';
                    arguments.insert({p.filename(), parameter_list});
                    in_file.close();
            } else {
                std::cerr << "Failed to open file: " << entry.path().string() << std::endl;
            }
            }
        }
    return arguments;
}

// The function to output the results
static void output_results(const std::unordered_map<std::string, std::unordered_set<std::string>>& result) {

    std::cout << "Filename: c_help_list.txt, arguments: " << std::endl;

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
static std::string concatenate(const std::vector<std::string>& v) {
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

// list all the equivalences between declarations in common.h and common.cpp that define the help
// these are used to substitute the searched params.attributes (keys) with help attributes (values)
// this double declaration avoids some weird destruction warning that I don't quite get
static std::unordered_map<std::string, std::string>& getSubDict() {
    static std::unordered_map<std::string, std::string> sub_dict {
    {"n_threads", "threads"},
    {"n_ctx", "ctx_size"},
    {"n_draft", "draft"},
    {"n_threads_batch", "threads_batch"},
    {"n_chunks", "chunks"},
    {"n_batch", "batch_size"},
    {"n_sequences", "sequences"},
    {"n_parallel", "parallel"},
    {"n_beams", "beams"},
    {"n_keep", "keep"},
    {"n_probs", "nprobs"},
    {"path_prompt_cache", "prompt_cache"},
    {"prompt_file", "prompt_file"},
    {"input_prefix", "in_prefix"},
    {"input_suffix", "in_suffix"},
    {"input_prefix_bos", "in_prefix_bos"},
    {"antiprompt", "reverse_prompt"},
    {"mul_mat_q", "no_mul_mat_q"},
    {"use_mmap", "no_mmap"},
    {"use_mlock", "mlock"},
    {"model_alias", "alias"},
    {"tfs_z", "tfs"},
    {"use_color", "color"},
    {"logit_bias", "logit_bias"},
    {"ignore_eos", "ignore_eos"},
    {"mirostat_tau", "mirostat_ent"},
    {"mirostat_eta", "mirostat_lr"},
    {"penalize_nl", "no_penalize_nl"},
    {"typical_p", "typical"},
    {"mem_size", "mem_size"},
    {"mem_buffer", "mem_buffer"},
    {"no_alloc", "no_alloc"}
    };
    return sub_dict;
}

static std::unordered_set<std::string> substitution_list(const std::unordered_set<std::string>& parameters) {
    std::unordered_set<std::string> new_parameters;
    const auto& sub_dict = getSubDict(); // Get reference to the sub_dict
    for (const std::string& parameter : parameters) {
        if (sub_dict.count(parameter) > 0) {
            new_parameters.insert(parameter);
            new_parameters.insert(sub_dict.at(parameter));
        } else {
            new_parameters.insert(parameter);
        }
    }
    return new_parameters;
}


static std::vector<std::pair<std::string, std::unordered_set<std::string>>> convert_to_sorted_vector(const std::unordered_map<std::string, std::unordered_set<std::string>>& result) {
    // Convert the unordered_map to a vector of pairs
    std::vector<std::pair<std::string, std::unordered_set<std::string>>> sorted_vector(result.begin(), result.end());

    // Sort the vector based on key (if needed)
    std::sort(sorted_vector.begin(), sorted_vector.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    return sorted_vector;
}

static void title_print(std::string filename) {
    std::cout << "Title: " << filename << std::endl;
}

// The function to find parameters in the help file
static void find_parameters(const std::string& file, const std::vector<std::pair<std::string, std::unordered_set<std::string>>>& sorted_result) {    std::ifstream helpfile(file);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(helpfile, line)) {
        lines.push_back(line);
    }
    helpfile.close();

    for (const auto& pair : sorted_result) {
        std::string filename = pair.first;
        std::unordered_set<std::string> arguments = substitution_list(pair.second);
        std::unordered_set<std::string> parameters;

        for (const std::string& line2 : lines) { // avoid Wshadow warning from line 270
            for (const std::string& argument : arguments) {
                std::string pattern = "(?:--" + argument + "\\s)|(?:params\\." + argument + "(?=[\\s.,\\.\\(\\);]|\\.+\\w))";
                std::regex regex(pattern);
                if (std::regex_search(line2.substr(0, 50), regex)) {
                    parameters.insert(line2);
                }
            }
        }

        std::unordered_set<std::string> all_parameters(parameters.begin(), parameters.end());

        title_print(filename);
        std::cout << "\nCommand-line arguments available and gpt-params functions implemented (TODO: multi-line helps NEED SOME WORK):\n";

        if (all_parameters.empty()) {
            std::cout << "    \033[032mNone\033[0m\n";
        } else {
            int help_count = 0;
            for (const std::string& parameter : all_parameters) {
                std::string replaced_param = replace_underscores_with_hyphens(parameter);
                if (parameter.compare(0, 4, "    ") != 0) {
                    help_count++;
                    std::cout << help_count << " help: \033[33m" << replaced_param << "<30}\033[0m\n";
                } else {
                    std::cout << "   help: \033[33m" << replaced_param << "<30}\033[0m\n";
                }
            }

            std::unordered_map<std::string, std::vector<std::string>> readcommonh_parameters;
            std::cout << "\nNow we extract the original gpt_params definition from common.h with the defaults for implemented arguments:\n";
            int gpt_count = 0;
            for (const auto& pair2 : readcommonh_parameters) {
                const std::string& k = pair2.first;
                const std::vector<std::string>& v = pair2.second;
                if (readcommonh_parameters.empty()) {
                    std::cout << "    \033[032mNone\033[0m\n";
                } else if (std::find(arguments.begin(), arguments.end(), k) != arguments.end()) {
                    std::string concatenated_element = concatenate(v);
                    gpt_count++;
                    std::cout << gpt_count << " gpt_param: \033[32m" << k << ">19}; \033[34mrole: \033[33m" << concatenated_element << "<60}\033[0m;  \033[34mdefault: \033[30m" << v[1] << "<10}\033[0m\n";
                }
            }

            std::cout << "\nSearching the other way round is more efficient:\n";
            int key_count = 0;
            for (const std::string& argument : std::unordered_set<std::string>(arguments.begin(), arguments.end())) {
                if (readcommonh_parameters.count(argument) > 0) {
                    key_count++;
                    const std::vector<std::string>& parameter_info = readcommonh_parameters[argument];
                    std::string concatenated_element = concatenate(parameter_info);
                    std::cout << key_count << " key: " << argument << ">25}; role: " << concatenated_element << "<60}; default: " << parameter_info[1] << "<10}\n";
                }
            }

            if (help_count == gpt_count && gpt_count == key_count) {
                std::cout << "\n\033[032mNo unresolved help-list incompatibilities with \033[33m" << filename.substr(filename.find_last_of('/') + 1) << "\033[0m\n";
            } else {
                std::cout << "\n\033[031mThis app requires some attention regarding help-function consistency.\033[0m\n";
            }
        }
    }
}

int main() {

    // ADD LOG FILE USING CODE FROM MAIN.CPP

    std::string directory = "/Users/edsilm2/llama.cpp/examples";
    std::string common_source = "/Users/edsilm2/llama.cpp/common/common.cpp";
    std::string target = "/Users/edsilm2/llama.cpp/examples/cmap-example/c_help_list.txt";

    update_help_file(common_source, target);
    replace_dashes_with_underscores(target);

    auto result = find_arguments(directory);
    // we get here with c_help_test.txt correctly populated
    // but there seems to be something wrong with how results affects things
    // and it contains an absurd number of elements in parameters
    output_results(result);
    auto sorted_result = convert_to_sorted_vector(result);
    find_parameters(target, sorted_result);

    return 0;
}
