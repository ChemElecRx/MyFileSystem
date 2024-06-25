#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <stack>

class File {
public:
    std::string name;
    std::string content;

    File(std::string name) : name(name) {}
};

class Directory {
public:
    std::string name;
    std::unordered_map<std::string, File*> files;
    std::unordered_map<std::string, Directory*> directories;

    Directory(std::string name) : name(name) {}

    ~Directory() {
        // Destructor to free allocated memory for files and directories
        for (auto& pair : files) {
            delete pair.second;
        }
        for (auto& pair : directories) {
            delete pair.second;
        }
    }

    // Method to create a new file in the directory
    void createFile(std::string fileName) {
        if (files.find(fileName) == files.end()) {
            files[fileName] = new File(fileName);
        } else {
            std::cerr << "File already exists: " << fileName << std::endl;
        }
    }

    // Method to create a new sub-directory in the directory
    void createDirectory(std::string dirName) {
        if (directories.find(dirName) == directories.end()) {
            directories[dirName] = new Directory(dirName);
        } else {
            std::cerr << "Directory already exists: " << dirName << std::endl;
        }
    }

    // Method to get a file by name from the directory
    File* getFile(std::string fileName) {
        if (files.find(fileName) != files.end()) {
            return files[fileName];
        }
        return nullptr;
    }

    // Method to get a sub-directory by name from the directory
    Directory* getDirectory(std::string dirName) {
        if (directories.find(dirName) != directories.end()) {
            return directories[dirName];
        }
        return nullptr;
    }

    // Method to list contents (files and sub-directories) of the directory
    void listContents() {
        std::cout << "Directories:" << std::endl;
        for (const auto& pair : directories) {
            std::cout << "  " << pair.first << "/" << std::endl;
        }
        std::cout << "Files:" << std::endl;
        for (const auto& pair : files) {
            std::cout << "  " << pair.first << std::endl;
        }
    }
};

class FileSystem {
public:
    Directory* root;
    Directory* currentDirectory;
    std::vector<std::string> pathStack;

    FileSystem() {
        // Constructor to initialize the file system
        root = new Directory("/");
        currentDirectory = root;
        pathStack.push_back("/");
    }

    // Destructor to free allocated memory for the file system
    ~FileSystem() {
        delete root;
    }

    // Method to create a file at a specified path
    void createFile(std::string path) {
        Directory* dir = navigateToDirectory(path);
        std::string fileName = getFileName(path);
        if (dir) {
            dir->createFile(fileName);
        }
    }

    // Method to write content to a file at a specified path
    void writeFile(std::string path, std::string content) {
        Directory* dir = navigateToDirectory(path);
        std::string fileName = getFileName(path);
        if (dir) {
            File* file = dir->getFile(fileName);
            if (file) {
                file->content = content;
            } else {
                std::cerr << "File not found: " << fileName << std::endl;
            }
        }
    }

    // Method to read content from a file at a specified path
    std::string readFile(std::string path) {
        Directory* dir = navigateToDirectory(path);
        std::string fileName = getFileName(path);
        if (dir) {
            File* file = dir->getFile(fileName);
            if (file) {
                return file->content;
            }
        }
        return "";
    }

    // Method to list contents of a directory at a specified path
    void listDirectory(std::string path) {
        Directory* dir = navigateToDirectory(path);
        if (dir) {
            dir->listContents();
        }
    }

    // Method to change current directory to a specified path
    void changeDirectory(std::string path) {
        Directory* dir = navigateToDirectory(path);
        if (dir) {
            currentDirectory = dir;
            updatePathStack(path);
        } else {
            std::cerr << "Directory not found: " << path << std::endl;
        }
    }

    // Method to print the current working directory path
    void printWorkingDirectory() {
        for(const std::string& dir : pathStack) {
            std::cout << dir;
        }
        std::cout << std::endl;
    }

private:
    // Private method to navigate to a directory specified by path
    Directory* navigateToDirectory(std::string path) {
        Directory* current = (path[0] == '/') ? root : currentDirectory;
        if (path == "/") {
            return root;
        }
        std::istringstream pathStream(path);
        std::string token;
        while (std::getline(pathStream, token, '/')) {
            if (token == "..") {
                current = findParentDirectory(current);
            } else if (token != "." && !token.empty()) {
                if (current->directories.find(token) != current->directories.end()) {
                    current = current->directories[token];
                } else {
                    std::cerr << "Directory not found: " << token << std::endl;
                    return nullptr;
                }
            }
        }
        return current;
    }

    // Private method to find the parent directory of a given directory
    Directory* findParentDirectory(Directory* dir) {
        if (dir == root) {
            return root;
        }
        Directory* parent = root;
        for (size_t i = 1; i < pathStack.size() - 1; ++i) {
            parent = parent->directories[pathStack[i].substr(0, pathStack[i].length() - 1)];
        }
        return parent;
    }

    // Private method to update the path stack based on a new path
    void updatePathStack(std::string path) {
        if (path[0] == '/') {
            pathStack.clear();
            pathStack.push_back("/");
        }
        std::istringstream pathStream(path);
        std::string token;
        while (std::getline(pathStream, token, '/')) {
            if (token == "..") {
                if (pathStack.size() > 1) {
                    pathStack.pop_back();
                }
            } else if (token != "." && !token.empty()) {
                pathStack.push_back(token + "/");
            }
        }
    }

    // Private method to extract the file name from a path
    std::string getFileName(std::string path) {
        size_t pos = path.find_last_of('/');
        return path.substr(pos + 1);
    }
};

int main() {
    // Main function to interact with the file system through command-line interface
    FileSystem fs;
    std::string command;

    std::cout << "Welcome to the simple file system. Type 'exit' to quit." << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        }

        // Parse command and arguments
        std::istringstream iss(command);
        std::string cmd, arg1, arg2;
        iss >> cmd >> arg1;
        std::getline(iss, arg2);
        if (!arg2.empty() && arg2[0] == ' ') {
            arg2 = arg2.substr(1);
        }

        // Execute commands based on user input
        if (cmd == "mkdir") {
            fs.currentDirectory->createDirectory(arg1);
        } else if (cmd == "mkfile") {
            fs.createFile(arg1);
        } else if (cmd == "write") {
            fs.writeFile(arg1, arg2);
        } else if (cmd == "read") {
            std::cout << fs.readFile(arg1) << std::endl;
        } else if (cmd == "ls") {
            fs.listDirectory(arg1);
        } else if (cmd == "cd") {
            fs.changeDirectory(arg1);
        } else if (cmd == "pwd") {
            fs.printWorkingDirectory();
        } else {
            std::cerr << "Unknown command: " << cmd << std::endl;
        }
    }

    return 0;
}
