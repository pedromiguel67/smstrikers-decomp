#include "smstrikers_pc/game_image.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main()
{
    const std::filesystem::path fixture = std::filesystem::temp_directory_path() / "smstrikers_pc_sha1_test.txt";
    const std::vector<std::pair<std::string, std::string>> vectors{
        {"", "da39a3ee5e6b4b0d3255bfef95601890afd80709"},
        {"abc", "a9993e364706816aba3e25717850c26c9cd0d89d"},
        {std::string(55, 'a'), "c1c8bbdc22796e28c0e15163d20899b65621d65a"},
        {std::string(56, 'a'), "c2db330f6083854c99d4b5bfb6e8f29f201be699"},
        {std::string(64, 'a'), "0098ba824b5c16427bd7a1122a5a442a25ec644d"},
        {std::string(65, 'a'), "11655326c708d70319be2610e8a57d9a5b959d3b"},
    };
    for (const auto& [contents, expected] : vectors)
    {
        std::ofstream output(fixture, std::ios::binary);
        output << contents;
        output.close();

        const std::string digest = smstrikers::pc::ComputeSha1(fixture);
        if (digest != expected)
        {
            std::cerr << "Unexpected SHA-1: " << digest << '\n';
            return 1;
        }
    }

    std::error_code error;
    std::filesystem::remove(fixture, error);
    return 0;
}
