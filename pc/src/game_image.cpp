#include "smstrikers_pc/game_image.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace smstrikers::pc {
namespace {

constexpr std::uint32_t RotateLeft(std::uint32_t value, unsigned int bits)
{
    return (value << bits) | (value >> (32U - bits));
}

void ProcessBlock(const std::array<std::uint8_t, 64>& block, std::array<std::uint32_t, 5>& hash)
{
    std::array<std::uint32_t, 80> words{};
    for (std::size_t index = 0; index < 16; ++index)
    {
        const std::size_t offset = index * 4;
        words[index] = (static_cast<std::uint32_t>(block[offset]) << 24U) |
                       (static_cast<std::uint32_t>(block[offset + 1]) << 16U) |
                       (static_cast<std::uint32_t>(block[offset + 2]) << 8U) |
                       static_cast<std::uint32_t>(block[offset + 3]);
    }
    for (std::size_t index = 16; index < words.size(); ++index)
    {
        words[index] = RotateLeft(words[index - 3] ^ words[index - 8] ^ words[index - 14] ^ words[index - 16], 1);
    }

    std::uint32_t a = hash[0];
    std::uint32_t b = hash[1];
    std::uint32_t c = hash[2];
    std::uint32_t d = hash[3];
    std::uint32_t e = hash[4];
    for (std::size_t index = 0; index < words.size(); ++index)
    {
        std::uint32_t function = 0;
        std::uint32_t constant = 0;
        if (index < 20)
        {
            function = (b & c) | ((~b) & d);
            constant = 0x5A827999;
        }
        else if (index < 40)
        {
            function = b ^ c ^ d;
            constant = 0x6ED9EBA1;
        }
        else if (index < 60)
        {
            function = (b & c) | (b & d) | (c & d);
            constant = 0x8F1BBCDC;
        }
        else
        {
            function = b ^ c ^ d;
            constant = 0xCA62C1D6;
        }
        const std::uint32_t next = RotateLeft(a, 5) + function + e + constant + words[index];
        e = d;
        d = c;
        c = RotateLeft(b, 30);
        b = a;
        a = next;
    }
    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
}

} // namespace

std::string ComputeSha1(const std::filesystem::path& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input)
    {
        throw std::runtime_error("Unable to open file for SHA-1: " + path.string());
    }

    std::array<std::uint32_t, 5> hash{0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
    std::array<std::uint8_t, 64> block{};
    std::uint64_t byteCount = 0;
    while (true)
    {
        input.read(reinterpret_cast<char*>(block.data()), static_cast<std::streamsize>(block.size()));
        const std::streamsize bytesRead = input.gcount();
        byteCount += static_cast<std::uint64_t>(bytesRead);

        if (bytesRead == static_cast<std::streamsize>(block.size()))
        {
            ProcessBlock(block, hash);
            continue;
        }
        if (!input.eof())
        {
            throw std::runtime_error("Unable to read file for SHA-1: " + path.string());
        }

        std::fill(block.begin() + bytesRead, block.end(), 0);
        block[static_cast<std::size_t>(bytesRead)] = 0x80;
        if (bytesRead >= 56)
        {
            ProcessBlock(block, hash);
            block.fill(0);
        }
        break;
    }
    const std::uint64_t bitCount = byteCount * 8;
    for (std::size_t index = 0; index < 8; ++index)
    {
        block[63 - index] = static_cast<std::uint8_t>(bitCount >> (index * 8));
    }
    ProcessBlock(block, hash);

    std::ostringstream output;
    output << std::hex << std::setfill('0');
    for (const std::uint32_t word : hash)
    {
        output << std::setw(8) << word;
    }
    return output.str();
}

std::optional<GameImageInfo> IdentifyGameImage(const std::filesystem::path& gameDirectory, std::string& error)
{
    const std::filesystem::path executablePath = gameDirectory / "sys" / "main.dol";
    std::error_code filesystemError;
    if (!std::filesystem::is_regular_file(executablePath, filesystemError))
    {
        error = "Expected a retail executable at: " + executablePath.string();
        return std::nullopt;
    }

    const std::string digest = ComputeSha1(executablePath);
    if (digest == "376d699c99b6b0949abe1b4ceccefdef7828d2b5")
    {
        return GameImageInfo{"G4QE01", "North America", executablePath};
    }
    if (digest == "6dc83dc91d0a5887f0056623498d4cbcd88bc463")
    {
        return GameImageInfo{"G4QP01", "Europe", executablePath};
    }
    if (digest == "d116f02b778a4f69725fd1c00656012d16ebf94a")
    {
        return GameImageInfo{"G4QJ01", "Japan", executablePath};
    }

    error = "Unsupported main.dol SHA-1: " + digest;
    return std::nullopt;
}

} // namespace smstrikers::pc
