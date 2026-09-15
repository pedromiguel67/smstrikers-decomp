#include "smstrikers_pc/content_mount.h"

#include <filesystem>
#include <fstream>
#include <iostream>

int main()
{
    const std::filesystem::path root = std::filesystem::temp_directory_path() / "smstrikers_pc_content_mount_test";
    std::error_code errorCode;
    std::filesystem::remove_all(root, errorCode);
    std::filesystem::create_directories(root / "files", errorCode);
    {
        std::ofstream output(root / "files" / "sample.bin", std::ios::binary);
        output << "test";
    }

    std::string error;
    const auto mount = smstrikers::pc::ContentMount::Create(root, error);
    if (!mount)
    {
        std::cerr << error << '\n';
        return 1;
    }
    const auto file = mount->ResolveFile("files/sample.bin", error);
    const auto bytes = mount->ReadFile("files/sample.bin", error);
    const auto escaped = mount->ResolveFile("../outside.bin", error);
    std::filesystem::remove_all(root, errorCode);

    if (!file || !bytes || bytes->size() != 4 || escaped)
    {
        std::cerr << "Content mount did not enforce the expected read-only boundary.\n";
        return 1;
    }
    return 0;
}
