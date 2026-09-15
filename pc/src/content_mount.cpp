#include "smstrikers_pc/content_mount.h"

#include <algorithm>
#include <fstream>
#include <iterator>

namespace smstrikers::pc {
namespace {

bool IsInsideRoot(const std::filesystem::path& root, const std::filesystem::path& candidate)
{
    const std::filesystem::path relative = candidate.lexically_relative(root);
    return !relative.empty() && std::ranges::none_of(relative, [](const std::filesystem::path& component) {
        return component == "..";
    });
}

} // namespace

ContentMount::ContentMount(std::filesystem::path root)
    : root_(std::move(root))
{
}

std::optional<ContentMount> ContentMount::Create(const std::filesystem::path& root, std::string& error)
{
    std::error_code filesystemError;
    if (!std::filesystem::is_directory(root, filesystemError))
    {
        error = "Content root is unavailable: " + root.string();
        return std::nullopt;
    }

    const std::filesystem::path canonicalRoot = std::filesystem::weakly_canonical(root, filesystemError);
    if (filesystemError)
    {
        error = "Unable to resolve content root: " + root.string();
        return std::nullopt;
    }
    return ContentMount(canonicalRoot);
}

const std::filesystem::path& ContentMount::Root() const
{
    return root_;
}

std::optional<std::filesystem::path> ContentMount::ResolveFile(const std::filesystem::path& logicalPath,
                                                                 std::string& error) const
{
    if (logicalPath.empty() || logicalPath.is_absolute())
    {
        error = "Content path must be a non-empty relative path: " + logicalPath.string();
        return std::nullopt;
    }

    std::error_code filesystemError;
    const std::filesystem::path candidate = std::filesystem::weakly_canonical(root_ / logicalPath, filesystemError);
    if (filesystemError || !IsInsideRoot(root_, candidate))
    {
        error = "Content path escapes the mounted root: " + logicalPath.string();
        return std::nullopt;
    }
    if (!std::filesystem::is_regular_file(candidate, filesystemError))
    {
        error = "Content file is unavailable: " + logicalPath.string();
        return std::nullopt;
    }
    return candidate;
}

std::optional<std::vector<std::byte>> ContentMount::ReadFile(const std::filesystem::path& logicalPath,
                                                              std::string& error) const
{
    const auto resolved = ResolveFile(logicalPath, error);
    if (!resolved)
    {
        return std::nullopt;
    }

    std::ifstream input(*resolved, std::ios::binary);
    if (!input)
    {
        error = "Unable to open content file: " + logicalPath.string();
        return std::nullopt;
    }
    const std::vector<char> characters((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    if (input.bad())
    {
        error = "Unable to read content file: " + logicalPath.string();
        return std::nullopt;
    }
    std::vector<std::byte> bytes(characters.size());
    std::transform(characters.begin(), characters.end(), bytes.begin(), [](char character) {
        return static_cast<std::byte>(static_cast<unsigned char>(character));
    });
    return bytes;
}

} // namespace smstrikers::pc
