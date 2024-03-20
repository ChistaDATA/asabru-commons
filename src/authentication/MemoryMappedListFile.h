#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <shared_mutex>
#include <sstream>

class MemoryMappedListFile
 {
private:
    std::vector<std::string> list;
    std::string filePath;
    char *mappedData = nullptr;
    size_t mappedLength = 0;
    mutable std::shared_mutex mutex; // Shared mutex for read-write locking

    void ensureFileExists() {
        std::ofstream ofs(filePath, std::ios::app); // Open for appending to create if not exists
        ofs.close(); // Immediately close the file, we just want to ensure it exists
    }

    void loadFromFile() {
        std::unique_lock<std::shared_mutex> lock(mutex);

        unmapFile(); // Ensure any previous mapping is cleared

        ensureFileExists(); // Make sure the file exists before attempting to open it

        int fd = open(filePath.c_str(), O_RDONLY);
        if (fd == -1) {
            throw std::runtime_error("Could not open file");
        }

        struct stat fileInfo;
        if (fstat(fd, &fileInfo) == -1) {
            close(fd);
            throw std::runtime_error("Could not get file size");
        }

        if (fileInfo.st_size == 0) { // Handle empty file gracefully
            close(fd);
            return; // Nothing to map or load, so return early
        }

        mappedLength = fileInfo.st_size;
        mappedData = static_cast<char *>(mmap(nullptr, mappedLength, PROT_READ, MAP_PRIVATE, fd, 0));
        close(fd);

        if (mappedData == MAP_FAILED) {
            throw std::runtime_error("Memory mapping failed");
        }

        std::string content(mappedData, mappedLength);
        std::istringstream iss(content);
        std::string line;
        list.clear();
        while (std::getline(iss, line)) {
            if (!line.empty()) {
                list.push_back(line);
            }
        }

        std::sort(list.begin(), list.end());
    }

    void unmapFile() {
        if (mappedData) {
            munmap(mappedData, mappedLength);
            mappedData = nullptr;
            mappedLength = 0;
        }
    }

    void writeToFile() {
        std::unique_lock<std::shared_mutex> lock(mutex);

        std::ofstream ofs(filePath, std::ofstream::out | std::ofstream::trunc);
        for (const auto &line : list) {
            ofs << line << "\n";
        }
    }

public:
    MemoryMappedListFile(const std::string &path) : filePath(path) {
        loadFromFile();
    }

    ~MemoryMappedListFile() {
        unmapFile();
    }

    void add(const std::string &value)
    {
        {
            std::unique_lock<std::shared_mutex> lock(mutex); // Exclusive lock for writing
            auto it = std::lower_bound(list.begin(), list.end(), value);
            if (it == list.end() || *it != value)
            { // Prevent duplicates
                list.insert(it, value);
            }
        } // The lock is released here

        writeToFile();
        loadFromFile(); // Refresh mapping
    }

    void remove(const std::string &value)
    {
        {
            std::unique_lock<std::shared_mutex> lock(mutex); // Exclusive lock for writing
            auto it = std::find(list.begin(), list.end(), value);
            if (it != list.end())
            {
                list.erase(it);
            }
        } // The lock is released here

        writeToFile();
        loadFromFile(); // Refresh mapping to reflect changes
    }

    void removeList(const std::vector<std::string> &valuesToRemove)
    {
        {
            std::unique_lock<std::shared_mutex> lock(mutex); // Exclusive lock for writing

            auto shouldBeRemoved = [&valuesToRemove](const std::string &item)
            {
                return std::find(valuesToRemove.begin(), valuesToRemove.end(), item) != valuesToRemove.end();
            };

            auto newEnd = std::remove_if(list.begin(), list.end(), shouldBeRemoved);
            list.erase(newEnd, list.end());
        } // The lock is released here

        writeToFile();
        loadFromFile(); // Refresh mapping to reflect changes
    }

    std::vector<std::string> getList() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex); // Shared lock for reading
        return list;                                     // Return a copy of the list for safety
    }

    bool isPresent(const std::string &token) const
    {
        std::shared_lock<std::shared_mutex> lock(mutex); // Shared lock for reading
        return std::find(list.begin(), list.end(), token) != list.end();
    }
};
