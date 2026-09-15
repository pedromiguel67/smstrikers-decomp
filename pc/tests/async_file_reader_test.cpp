#include "smstrikers_pc/async_file_reader.h"

#include <filesystem>
#include <fstream>
#include <iostream>

int main()
{
    const std::filesystem::path root = std::filesystem::temp_directory_path() / "smstrikers_pc_async_file_reader_test";
    std::error_code errorCode;
    std::filesystem::remove_all(root, errorCode);
    std::filesystem::create_directories(root / "files", errorCode);
    {
        std::ofstream output(root / "files" / "sample.bin", std::ios::binary);
        output << "async";
    }

    std::string error;
    const auto mount = smstrikers::pc::ContentMount::Create(root, error);
    if (!mount)
    {
        std::cerr << error << '\n';
        return 1;
    }

    const smstrikers::pc::AsyncFileReader reader(*mount);
    const smstrikers::pc::FileReadResult completed = reader.Read("files/sample.bin").get();
    const smstrikers::pc::FileReadResult missing = reader.Read("files/missing.bin").get();
    std::filesystem::remove_all(root, errorCode);

    if (!completed.Succeeded() || completed.bytes.size() != 5 || missing.Succeeded() || missing.error.empty())
    {
        std::cerr << "Asynchronous content reads did not return the expected results.\n";
        return 1;
    }
    return 0;
}
