#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <thread>
#include <chrono>

class CachedFileList
{
private:
    std::unordered_map<std::string, bool> cache; // Map string to its existence
    std::string filename;
    mutable std::shared_mutex mutex; // Shared mutex for read-write locking

    void populateCacheFromFile()
    {
        std::ifstream file(filename);
        if (!file)
        {
            std::cout << "File does not exist, continuing without error." << std::endl;
            return;
        }

        std::string line;
        while (getline(file, line))
        {
            if (!line.empty() && line[0] != '\v')
            {
                cache[line] = true;
            }
        }
    }

public:
    CachedFileList(const std::string &file) : filename(file)
    {
        populateCacheFromFile();
    }

    void add(const std::string &str)
    {
        std::unique_lock<std::shared_mutex> lock(mutex);

        if (cache.find(str) != cache.end())
        {
            return;
        }

        std::ofstream outFile(filename, std::ios::out | std::ios::app);
        if (outFile.is_open())
        {
            outFile << str << std::endl;
            cache[str] = true;
        }
    }

    void remove(const std::string &str)
    {
        std::unique_lock<std::shared_mutex> lock(mutex);

        if (cache.find(str) == cache.end())
        {
            return;
        }

        std::fstream file(filename, std::ios::in | std::ios::out);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file for removal." << std::endl;
            return;
        }

        std::string line;
        long pos = 0;
        while (getline(file, line))
        {
            if (line == str)
            {
                file.seekp(pos);
                file.put('\v');
                break;
            }
            pos += line.length() + 1; // +1 for the newline character
        }

        cache.erase(str);
    }

    bool find(const std::string &str) const
    {
        std::shared_lock<std::shared_mutex> lock(mutex); // Shared access for reads

        return cache.find(str) != cache.end();
    }

    std::vector<std::string> getAll() const
    {
        std::shared_lock<std::shared_mutex> lock(mutex); // Shared access for reads

        std::vector<std::string> result;
        for (const auto &[key, _] : cache)
        {
            result.push_back(key);
        }
        return result;
    }

    void compactFile()
    {
        std::unique_lock<std::shared_mutex> lock(mutex); // Exclusive access

        std::ifstream inFile(filename, std::ios::in);
        if (!inFile.is_open())
        {
            std::cerr << "Failed to open input file for compaction." << std::endl;
            return;
        }

        std::ofstream tempFile(filename + ".temp", std::ios::out | std::ios::trunc);
        if (!tempFile.is_open())
        {
            std::cerr << "Failed to open temporary file for compaction." << std::endl;
            return;
        }

        std::string line;
        while (getline(inFile, line))
        {
            if (line[0] != '\v')
            { // Skip lines marked for deletion
                tempFile << line << std::endl;
            }
        }

        inFile.close();
        tempFile.close();

        std::remove(filename.c_str());
        std::rename((filename + ".temp").c_str(), filename.c_str());
    }
};
