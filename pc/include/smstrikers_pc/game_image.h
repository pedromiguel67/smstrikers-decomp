#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace smstrikers::pc {

struct GameImageInfo {
    std::string gameId;
    std::string region;
    std::filesystem::path executablePath;
};

std::string ComputeSha1(const std::filesystem::path& path);
std::optional<GameImageInfo> IdentifyGameImage(const std::filesystem::path& gameDirectory, std::string& error);

} // namespace smstrikers::pc
