#pragma once

#include <vector>
#include <string>
#include <fstream>

namespace FileUtils
{
    inline bool Exists(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        return file.good();
    }

    inline bool SaveBinary(const std::string& path, const std::vector<uint8_t>& data)
    {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) return false;

        file.write(
            reinterpret_cast<const char*>(data.data()),
            data.size()
        );

        return file.good();
    }

    inline std::vector<uint8_t> LoadBinary(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) return {};

        file.seekg(0, std::ios::end);
        size_t size = (size_t)file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> data(size);

        file.read(
            reinterpret_cast<char*>(data.data()),
            size
        );

        return data;
    }
}