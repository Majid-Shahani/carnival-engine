#pragma once
#include <string>
#include <vector>

namespace Carnival{

    std::vector<char> readFileCharVec(const std::string& filename);
    std::vector<char> readFileCharVec(const std::filesystem::path& filepath);
}