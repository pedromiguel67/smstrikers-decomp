#include "smstrikers_pc/async_file_reader.h"

#include <future>
#include <utility>

namespace smstrikers::pc {

bool FileReadResult::Succeeded() const
{
    return error.empty();
}

AsyncFileReader::AsyncFileReader(const ContentMount& mount)
    : mount_(mount)
{
}

std::future<FileReadResult> AsyncFileReader::Read(const std::filesystem::path& logicalPath) const
{
    return std::async(std::launch::async, [this, logicalPath] {
        std::string error;
        const auto bytes = mount_.ReadFile(logicalPath, error);
        if (!bytes)
        {
            return FileReadResult{{}, std::move(error)};
        }
        return FileReadResult{*bytes, {}};
    });
}

} // namespace smstrikers::pc
