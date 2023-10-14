// essentially a C++ port of find-implemented-args.py

#include <iostream>
#include <cstdio>   // prob unnecessary
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <dirent.h>

static void title_print(std::string filename) {
    int totalLength = 7 + filename.length(); // Calculate the total length
    std::string hashtagString(totalLength, '#'); // Create the string of "#" signs
    std::cout << hashtagString << std::endl;
    std::cout << "Title: " << filename << std::endl;
    std::cout << hashtagString << std::endl;
}

static void recursive_directory_iterator(const std::string& directory, std::vector<std::string>& files) {
    DIR* dir = opendir(directory.c_str());
    if (dir == nullptr) {
        return;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (filename == "." || filename == "..") {
            continue;
        }

        std::string fullpath = directory + "/" + filename;
        files.push_back(fullpath);

        if (entry->d_type == DT_DIR) {
            recursive_directory_iterator(fullpath, files);
        }
    }

    closedir(dir);
}

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

// Function to replace dashes/hyphens with underscores in a file
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

// Function(s) to capture everything between quotes in print statements)
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

// Function to update the source file
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

// match the params strings up to the first of the stipulated next characters
static std::string capture_after_params(const std::string& content, const std::size_t& length, const std::string& search_string) {
    std::size_t startPos = length; // this avoids using the npos default
    std::string delimiter = " .})(,>;"; // newline removed from list because we never reach it
    // this list of delimiters could also be a passed parameter to improve flexibility

    if (content.find(search_string) <= length) {
        startPos = content.find(search_string) + search_string.length();  // Adjust for the length of "params->"
    }

    if (startPos <= length) {
        std::size_t endPos = content.find_first_of(delimiter, startPos);
        if (endPos <= length) {
            return content.substr(startPos, endPos - startPos);
        } else {
            return content.substr(startPos);  // Capture until end of string [does it?]
        }
    }
    return "";  // Pattern not found
}

// extract file extension
static std::string getFileExtension(const std::string& fileName) {
    size_t dotIndex = fileName.find_last_of(".");
    if (dotIndex != std::string::npos) {
        return fileName.substr(dotIndex + 1);
    }
    return "";
}

// Function to find arguments from *.cpp files in a directory
static std::unordered_map<std::string, std::unordered_set<std::string>> find_arguments(const std::string& directory) {
    std::unordered_map<std::string, std::unordered_set<std::string>> arguments;
    std::unordered_set<std::string> parameter_list;

    // rewritten to comply with C++11 standards avoiding fs:filesystem
    std::vector<std::string> files;
    recursive_directory_iterator(directory, files);
    for (const auto& file : files) {
        if (getFileExtension(file) == "cpp") {
            title_print(file);
            std::ifstream input_file(file);
            if (!input_file) {
                std::cerr << "Failed to open file: " << file << std::endl;
                continue;
            } else {
                std::stringstream buffer;
                buffer << input_file.rdbuf();
                std::string content = buffer.str();

                std::size_t content_length = content.length();

                // this will eventually be a file we read in; for now a manual list
                std::vector<std::string> search_strings = {"params.", "params->", "gpt params ", "llama params", "my_params", "gpt_params", "gpt-params"};
                for (std::string search_string : search_strings) {
                    std::unordered_set<std::string> matches; // reset each loop
                    // this seems indiscriminate; we don't want literally everything after params ...
                    std::size_t pos = 0; // rezero each loop
                    while (pos < content_length) {
                        // we search for strings of interest
                        // but need to trap cases where it searches to 2^64-1 and crashes
                        // and can't let it do it then correct it; damage already done
                        pos = content.find(search_string, pos);
                        if (pos >= content_length) {
                            pos = content_length + 1;
                            continue;
                        } else {
                            matches.insert(capture_after_params(content.substr(pos), content_length, search_string));
                            pos++;
                        }
                    }
                    // now save matched strings with the search string prefixed
                    // doing it here collects by search_string
                    if (matches.empty()) {
                        std::cout  << std::endl << "********** No instances of " << search_string << " were found *************" << std::endl;
                    } else {
                        std::cout  << std::endl << "\033[33m************* Instances of " << search_string << " were found in " << file << " *************\033[0m" << std::endl << std::endl;
                        for (const auto& match : matches) {
                            std::cout << search_string + match << std::endl;
                            // parameter_list needs a more appropriate structure
                            parameter_list.insert(search_string + match);
                        }
                    }
                printf("\n\033[33mEntry path for previous output:\033[0m %s \n\n", file.c_str());
                arguments.insert({file, parameter_list});
                input_file.close();
                }
            }
        }
    }
    return arguments;
}

// Function to output the results
static void output_results(const std::unordered_map<std::string, std::unordered_set<std::string>>& result) {

    title_print("\033[33mWe are now inside output_results.\033[0m");
    std::cout << std::endl << "Filename: c_help_list.txt, arguments: " << std::endl << std::endl;

    for (auto it = result.begin(); it != result.end(); ++it) {
        const auto& filename = it->first;
        const auto& arguments = it->second;
        title_print(filename);
        for (const auto& argument : arguments) {
            std::cout << argument << std::endl;
        }
    std::cout << std::endl;
    }
}

// Function to concatenate elements after "//"
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
        auto iter = sub_dict.find(parameter);
        if (iter != sub_dict.end()) { // Key exists in sub_dict
            new_parameters.insert(iter->second);
        } else { // Key does not exist in sub_dict
            // Handle absence of key, if required
            new_parameters.insert(parameter + " **** not found **** ");
        }
    }
    return new_parameters;
}

static std::vector<std::pair<std::string, std::unordered_set<std::string>>> convert_to_sorted_vector(const std::unordered_map<std::string, std::unordered_set<std::string>>& result) {
    // Convert the unordered_map to a vector of pairs
    std::vector<std::pair<std::string, std::unordered_set<std::string>>> sorted_vector(result.begin(), result.end());

    // Sort the vector based on key (if needed)
    std::sort(sorted_vector.begin(), sorted_vector.end(), [] \
        (const std::pair<std::string, std::unordered_set<std::string>>& alpha, \
        const std::pair<std::string, std::unordered_set<std::string>>& beta) {
        return alpha.first < beta.first;
    });
    return sorted_vector;
}

// Function to find parameters in the help file
static void find_parameters(const std::string& file, std::vector<std::pair<std::string, std::unordered_set<std::string>>>& sorted_result) {
    std::ifstream helpfile(file);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(helpfile, line)) {
        lines.push_back(line);
    }
    helpfile.close();

    for (std::pair<std::string, std::unordered_set<std::string>>& p : sorted_result) {
        std::cout << p.first << std::endl;
        std::string filename = p.first;
        std::unordered_set<std::string> arguments = substitution_list(p.second);
        std::unordered_set<std::string> parameters;

        for (const std::string& line2 : lines) { // line2 to avoid Wshadow warning from line 270
            for (const std::string& argument : arguments) {
                std::string pattern = "--" + argument + " ";
                size_t pos = line2.find(pattern);

                if (pos != std::string::npos) {
                    parameters.insert(line2);
                } else {
                    pattern = "params." + argument;
                    pos = line2.find(pattern);
                    if (pos != std::string::npos) {
                        // Check if the following character is a valid delimiter
                        char delimiter = line2[pos + pattern.length()];
                        if (std::isspace(delimiter) || delimiter == '.' || delimiter == ',' || delimiter == '(' || delimiter == ')' || delimiter == ';') {
                            parameters.insert(line2);
                        }
                    }
                }
            }
        }

        std::unordered_set<std::string> all_parameters(parameters.begin(), parameters.end());

        title_print(filename);
        std::cout << "\nCommand-line arguments available and gpt-params functions implemented (TODO: multi-line helps NEED SOME WORK):\n";

        if (all_parameters.empty()) {
            std::cout << "    **** None ****\n";
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

    std::string directory = "/Users/edsilm2/llama.cpp";
    std::string common_source = "/Users/edsilm2/llama.cpp/common/common.cpp";
    std::string target = "/Users/edsilm2/llama.cpp/examples/cmap-example/c_help_list.txt";
    std::unordered_map<std::string, std::unordered_set<std::string>> result;

    update_help_file(common_source, target);
    replace_dashes_with_underscores(target);

    result = find_arguments(directory);
    // we get here with c_help_test.txt correctly populated
    // but there seems to be something wrong with how results affects things
    // and it contains an absurd number of elements in parameters
    output_results(result);

    auto sorted_result = convert_to_sorted_vector(result);

    find_parameters(target, sorted_result);

    return 0;
}
