#include "smstrikers_pc/content_mount.h"
#include "smstrikers_pc/game_image.h"

#include <filesystem>
#include <iostream>
#include <string_view>

namespace {

constexpr std::string_view kProgramName = "smstrikers_pc_bootstrap";

void PrintUsage(std::ostream& stream)
{
    stream << "Usage: " << kProgramName << " --game-dir <directory> [--asset <relative-path>]\n"
           << "\n"
           << "Identifies a supported, user-supplied Super Mario Strikers extraction for\n"
           << "a future native PC runtime. It does not emulate or launch the GameCube game.\n"
           << "\n"
           << "Options:\n"
           << "  --game-dir <directory>  Directory containing sys/main.dol.\n"
           << "  --asset <relative-path> Resolve and report a mounted content file.\n"
           << "  --help                  Show this help message.\n";
}

int ValidateGameDirectory(const std::filesystem::path& gameDirectory, const std::filesystem::path* assetPath)
{
    std::string error;
    const auto image = smstrikers::pc::IdentifyGameImage(gameDirectory, error);
    if (!image)
    {
        std::cerr << error << '\n';
        return 1;
    }

    std::cout << "Validated " << image->gameId << " (" << image->region << "): " << image->executablePath << '\n';
    if (assetPath)
    {
        const auto mount = smstrikers::pc::ContentMount::Create(gameDirectory, error);
        if (!mount)
        {
            std::cerr << error << '\n';
            return 1;
        }
        const auto asset = mount->ResolveFile(*assetPath, error);
        if (!asset)
        {
            std::cerr << error << '\n';
            return 1;
        }
        std::error_code filesystemError;
        std::cout << "Resolved asset (" << std::filesystem::file_size(*asset, filesystemError) << " bytes): " << *asset << '\n';
    }
    std::cout << "The native runtime is not implemented yet; see docs/pc_port.md.\n";
    return 0;
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        PrintUsage(std::cout);
        return 0;
    }

    const std::string_view option(argv[1]);
    if (option == "--help")
    {
        if (argc != 2)
        {
            PrintUsage(std::cerr);
            return 2;
        }
        PrintUsage(std::cout);
        return 0;
    }

    if (option == "--game-dir" && argc == 3)
    {
        return ValidateGameDirectory(argv[2], nullptr);
    }
    if (option == "--game-dir" && argc == 5 && std::string_view(argv[3]) == "--asset")
    {
        const std::filesystem::path assetPath(argv[4]);
        return ValidateGameDirectory(argv[2], &assetPath);
    }

    PrintUsage(std::cerr);
    return 2;
}
