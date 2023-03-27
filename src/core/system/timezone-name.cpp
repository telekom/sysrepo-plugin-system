#include "timezone-name.hpp"

#include <core/common.hpp>

#include <filesystem>

namespace ietf::sys {
/**
 * @brief Get system timezone name from /etc/localtime.
 *
 * @return Timezone name.
 */
TimezoneName getTimezoneName()
{
    if (std::filesystem::exists(ietf::sys::TIMEZONE_FILE_PATH) && std::filesystem::is_symlink(ietf::sys::TIMEZONE_FILE_PATH)) {
        auto link_path = std::filesystem::read_symlink(ietf::sys::TIMEZONE_FILE_PATH);
        auto dir = std::filesystem::path(ietf::sys::TIMEZONE_DIR_PATH);
        auto rel_path = std::filesystem::relative(link_path, dir);
        return rel_path;
    } else {
        throw std::runtime_error("Failed to get timezone name.");
    }
}

/**
 * @brief Set system timezone name. Throws a runtime_error if unable to set timezone.
 *
 * @param timezoneName Timezone name.
 */
void setTimezoneName(const TimezoneName& timezone_name)
{
    namespace fs = std::filesystem;

    // change timezone-name
    auto tz_dir = fs::path(ietf::sys::TIMEZONE_DIR_PATH);
    auto tz_file = tz_dir / timezone_name;

    // check if the file exists
    auto status = fs::status(tz_file);
    if (!fs::exists(status)) {
        throw std::runtime_error("Timezone file does not exist.");
    }

    // check for /etc/localtime symlink
    auto localtime = fs::path("/etc/localtime");
    if (fs::exists(localtime)) {
        // remove the symlink
        try {
            if (auto err = fs::remove(localtime); err != 0) {
                throw std::runtime_error("Failed to remove /etc/localtime symlink.");
            }
        } catch (fs::filesystem_error& err) {
            throw std::runtime_error("Failed to remove /etc/localtime symlink.");
        }
    }

    // symlink removed; create a new one
    try {
        fs::create_symlink(tz_file, localtime);
    } catch (fs::filesystem_error& err) {
        throw std::runtime_error("Failed to create /etc/localtime symlink.");
    }
}
}