// testing writable code

#include <cstdio>

#include <iostream>
#include <fstream>
#include <string>

int main() {
    FILE* outFile;

    // open the file
    outFile = fopen("test_file.txt", "w");
    if(outFile == nullptr) {
        fprintf(stderr, "Cannot open test_file.txt for writing\n");
        return 1;
    }

    // write to the file
    fprintf(outFile, "Hello World\n");

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
            std::cout << line << std::endl;
    }
    }
    // now using fstream
    std::ofstream outFile2("fstream_file.txt");

    if(!outFile2) {
        std::cerr << "Failed to open fstream_file.txt for writing" << std::endl;
        return 1;
    }

    // write to the file (will go into build by default)
    outFile2 << "Hello Big Bad World" << std::endl;

    // close the file when done writing
    outFile2.close();

    std::ifstream inFile2("fstream_file.txt");

    if(!inFile2) {
        std::cerr << "Failed to open fstream_file.txt for reading" << std::endl;
        return 1;
    }

    std::string str;

    // read from the file
    while (inFile2 >> str) {
        std::cout << str << std::endl;
    }

    // close the file when done reading
    inFile2.close();

    return 0;
}
