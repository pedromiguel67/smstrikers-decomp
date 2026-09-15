#pragma once

#include "smstrikers_pc/content_mount.h"

#include <future>
#include <string>
#include <vector>

namespace smstrikers::pc {

struct FileReadResult {
    std::vector<std::byte> bytes;
    std::string error;

    [[nodiscard]] bool Succeeded() const;
};

class AsyncFileReader {
public:
    explicit AsyncFileReader(const ContentMount& mount);

    [[nodiscard]] std::future<FileReadResult> Read(const std::filesystem::path& logicalPath) const;

private:
    const ContentMount& mount_;
};

} // namespace smstrikers::pc
