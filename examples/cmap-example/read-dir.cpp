// helper to read the files in a directory

#include <iostream>
#include <fstream>
#include <dirent.h>

void processFile(const std::string& filePath) {
    // Process the file as needed
    std::cout << filePath << std::endl;
}

void iterateDirectory(const std::string& directory) {
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
                /* code */
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
    iterateDirectory(directory);

    return 0;
}
