#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace smstrikers::pc {

class ContentMount {
public:
    static std::optional<ContentMount> Create(const std::filesystem::path& root, std::string& error);

    [[nodiscard]] const std::filesystem::path& Root() const;
    [[nodiscard]] std::optional<std::filesystem::path> ResolveFile(const std::filesystem::path& logicalPath,
                                                                     std::string& error) const;
    [[nodiscard]] std::optional<std::vector<std::byte>> ReadFile(const std::filesystem::path& logicalPath,
                                                                   std::string& error) const;

private:
    explicit ContentMount(std::filesystem::path root);

    std::filesystem::path root_;
};

} // namespace smstrikers::pc
