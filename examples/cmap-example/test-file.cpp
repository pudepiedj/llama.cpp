// testing writable code

#include <cstdio>

#include <iostream>
#include <fstream>
#include <string>

static std::vector<std::string> divide_string(const std::string& str, const std::string& splitter) {
    std::vector<std::string> tokens;
    std::string token;
    size_t start = 0;
    size_t end = 0;
    size_t opening_tag = 0;
    size_t closing_tag = 0;
    size_t prev_closing_tag = 0;

    while ((end = str.find(splitter, start)) != std::string::npos){
        if (end == prev_closing_tag) {
            start = closing_tag;
        }
        opening_tag = str.find("<", start);
        prev_closing_tag = closing_tag;
        closing_tag = str.find(">", start);

        if ((end < opening_tag) && (opening_tag != std::string::npos)){
            // not inside a tag
        } else {
            if ((opening_tag <= end) && (closing_tag >= end)) {
                start = opening_tag;
                end = closing_tag + 1;
                // inside a tag-pair
            } else {
                if (closing_tag < end) {
                    // not inside a tag
                } else {
                    if ((end > prev_closing_tag) && (end < closing_tag) && (opening_tag == std::string::npos) && (closing_tag != std::string::npos)) {
                        end = str.find(">", prev_closing_tag) + 1;
                        // between a closing tag and another one
                    }
                }
            }
        }
        // all paths lead here
        token = str.substr(start, end - start);
        tokens.push_back(token);
        start = end + 1;
    }
    tokens.push_back(str.substr(start));    // add the residue
    if (!tokens.empty()){
        return tokens;
    } else {
        tokens.push_back(str.substr(0));
        return tokens;
    }
}

int main() {
    FILE* outFile;

    // open the file
    outFile = fopen("test_file.txt", "w");
    if(outFile == nullptr) {
        fprintf(stderr, "Cannot open test_file.txt for writing\n");
        return 1;
    }

    // write to the file
    fprintf(outFile, "Hello <if you are there> World and <if you are not> tough and <if you are and not tough> even tougher!\n");
    fprintf(outFile, "Hello <if you are there> World and <if you are not> tough and <if you are <really trying to be awkward> and not> even tougher!\n");
    // close the file
    fclose(outFile);

    std::ifstream inFile("test_file.txt");
    std::string line;

    if (not inFile) {
        printf("Cannot open test_file.txt for reading\n");
        return 1;
    } else {
    // read from the file line by line and print to console

        while(getline(inFile, line)) {
            std::cout << std::endl << "**** Here is an unsplit line to see if the split works ****" << std::endl << std::endl;
            std::vector<std::string> pieces;
            std::string piece;

            std::cout << line << std::endl;

            pieces = divide_string(line, " ");
            std::cout << "**** Splitting the file lines ****" << std::endl;
            for(const std::string& piece : pieces) {
                std::cout << piece;
                if (&piece != &(*std::prev(pieces.end()))) {
                    std::cout << " - ";
                }
            }
            std::cout << std::endl;
        }
    }
    // now using fstream
    std::ofstream outFile2("fstream_file.txt");

    if(!outFile2) {
        std::cerr << "Failed to open fstream_file.txt for writing" << std::endl;
        return 1;
    }
    return 0;
}
