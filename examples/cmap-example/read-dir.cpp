// helper to read the files in a directory

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <unordered_set>
#include <unordered_map>
#include <string>

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
        if (sub_dict.count(parameter) == 0) {
            new_parameters.insert(parameter);
            new_parameters.insert(sub_dict.at(parameter));
        } else {
            new_parameters.insert(sub_dict.at(parameter));
        }
    }
    return new_parameters;
}

static std::unordered_set<std::string> substitution_list_two(const std::unordered_set<std::string>& parameters) {
    std::unordered_set<std::string> new_parameters;
    const auto& sub_dict = getSubDict(); // Get reference to the sub_dict
    for (const std::string& parameter : parameters) {
        auto iter = sub_dict.find(parameter);
        if (iter != sub_dict.end()) { // Key exists in sub_dict
            new_parameters.insert(iter->second);
        } else { // Key does not exist in sub_dict
            // Handle absence of key, if required
            new_parameters.insert(parameter + " **** not found ****");
        }
    }
    return new_parameters;
}

static void perform_substitutions_two(const std::unordered_set<std::string> &parameters) {
    std::unordered_set<std::string> arguments = substitution_list(parameters);

    std::cout << std::endl << "Now printing substituted strings of parameters:" << std::endl;
    for (std::string argument : arguments){
        std::cout << argument << std::endl;
    }
}

static void perform_substitutions(const std::unordered_set<std::string> &parameters) {
    std::unordered_set<std::string> arguments = substitution_list_two(parameters);

    std::cout << std::endl << "Now printing substituted strings of parameters:" << std::endl;
    for (std::string argument : arguments){
        std::cout << argument << std::endl;
    }
}

static void processFile(const std::string& filePath) {
    // Process the file as needed
    std::cout << filePath << std::endl;
}

static void iterateDirectory(const std::string& directory) {
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string fileName = entry->d_name;
            if (fileName == "." || fileName == "..") {
                continue;
            }
            std::string filePath = directory + '/' + fileName;
            // std::cout << fileName << std::endl;

            // Only include directories in processing
            if (entry->d_type != DT_DIR) {
                // Process the file
                processFile(filePath);
            } else {
            if (entry->d_type == DT_DIR) {
                std::cout << "Directory: " << fileName.c_str() << std::endl;
                iterateDirectory(filePath.c_str());
                }
            }

            {
            // code
            }
        }
        // std::cout << "Closing directory " << fileName.c_str() << std::endl;
        closedir(dir);  // note that this is the stream opened by opendir() above
    } else {
        std::cerr << "Failed to open directory: " << directory << std::endl;
    }
}

int main() {
    std::string directory = "/Users/edsilm2/llama.cpp/examples";
    std::unordered_set<std::string> parameters = {"n_batch", "john is cool", "n_chunks", "path_prompt_cache", "n_threads", "n_parallel", "mirostat_tau", "mirostat_eta", "input_prefix_bos"};

    iterateDirectory(directory);
    perform_substitutions(parameters);
    perform_substitutions_two(parameters);

    return 0;
}
