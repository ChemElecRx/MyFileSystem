#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>

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
        for (auto& pair : files) {
            delete pair.second;
        }
        for (auto& pair : directories) {
            delete pair.second;
        }
    }

    void createFile(std::string fileName) {
        files[fileName] = new File(fileName);
    }

    void createDirectory(std::string dirName) {
        directories[dirName] = new Directory(dirName);
    }

    File* getFile(std::string fileName) {
        if (files.find(fileName) != files.end()) {
            return files[fileName];
        }
        return nullptr;
    }

    Directory* getDirectory(std::string dirName) {
        if (directories.find(dirName) != directories.end()) {
            return directories[dirName];
        }
        return nullptr;
    }

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

    FileSystem() {
        root = new Directory("/");
        currentDirectory = root;
    }

    ~FileSystem() {
        delete root;
    }

    void createFile(std::string path) {
        Directory* dir = navigateToDirectory(path);
        std::string fileName = getFileName(path);
        if (dir) {
            dir->createFile(fileName);
        }
    }

    void writeFile(std::string path, std::string content) {
        Directory* dir = navigateToDirectory(path);
        std::string fileName = getFileName(path);
        if (dir) {
            File* file = dir->getFile(fileName);
            if (file) {
                file->content = content;
            }
        }
    }

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

    void listDirectory(std::string path) {
        Directory* dir = navigateToDirectory(path);
        if (dir) {
            dir->listContents();
        }
    }

    void changeDirectory(std::string path) {
        Directory* dir = navigateToDirectory(path);
        if (dir) {
            currentDirectory = dir;
        } else {
            std::cout << "Directory not found: " << path << std::endl;
        }
    }

private:
    Directory* navigateToDirectory(std::string path) {
        Directory* current = (path[0] == '/') ? root : currentDirectory;
        size_t pos = (path[0] == '/') ? 1 : 0; // Skip initial '/' if absolute path
        while (pos < path.size()) {
            size_t nextPos = path.find('/', pos);
            std::string dirName = path.substr(pos, nextPos - pos);
            if (current->directories.find(dirName) != current->directories.end()) {
                current = current->directories[dirName];
            } else {
                return nullptr;
            }
            if (nextPos == std::string::npos) break;
            pos = nextPos + 1;
        }
        return current;
    }

    std::string getFileName(std::string path) {
        size_t pos = path.find_last_of('/');
        return path.substr(pos + 1);
    }
};

int main() {
    FileSystem fs;
    std::string command;

    std::cout << "Welcome to the simple file system. Type 'exit' to quit." << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        }

        std::istringstream iss(command);
        std::string cmd, arg1, arg2;
        iss >> cmd >> arg1;
        std::getline(iss, arg2);
        if (!arg2.empty() && arg2[0] == ' ') {
            arg2 = arg2.substr(1);
        }

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
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
        }
    }

    return 0;
}
