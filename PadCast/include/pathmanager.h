#ifndef PADCAST_PATHMANAGER_H
#define PADCAST_PATHMANAGER_H

#include <filesystem>
#include <string>
#include <cstdlib>
#include <cstring>
#include <print>

class PathManager
{
private:
    static bool isPortableMode()
    {
        // Check if we're portable (For Windows, or source built into a build directory)
        // This will look for the config/resources directory in the directory the binary is in
        return (std::filesystem::exists(std::filesystem::current_path() / "config" / "config.ini")
                || std::filesystem::exists(std::filesystem::current_path() / "resources"));
        // false = we're probably in /usr/bin
    }

    static std::filesystem::path getExecutableDir()
    {
        // If on Linux, try to get binary path
        #ifdef __linux__
        std::filesystem::path execPath = std::filesystem::read_symlink("/proc/self/exe");
        return execPath.parent_path();
        #else
        // Otherwise we're likely on Windows
        return std::filesystem::current_path();
        #endif
    }

public:
    static std::string getConfigFilePath()
    {
        if (isPortableMode())
        {
            // Portable = config next to .exe
            return (std::filesystem::current_path() / "config" / "config.ini").string();
        }
        else
        {
            // otherwise, Linux system
            // try XDG first
            const char* xdg_config = std::getenv("XDG_CONFIG_HOME");
            std::filesystem::path config_dir;

            if (xdg_config && strlen(xdg_config) > 0)
            {
                config_dir = std::filesystem::path(xdg_config) / "padcast";
            }
            else
            {
                // if not XDG, then try manual
                const char* home = std::getenv("HOME");
                if (home)
                {
                    config_dir = (std::filesystem::path(home) / ".config" / "padcast");
                }
                else
                {
                    // Fallback to current dir
                    return (std::filesystem::current_path() / "config.ini").string();
                }
            }

            // Create directory if it doesn't exist
            std::filesystem::create_directories(config_dir);

            std::filesystem::path config_file = config_dir / "config.ini";

            // If user config doesn't exist, make copy from program's
            if (!std::filesystem::exists(config_file))
            {
                std::filesystem::path padcast_config = "/usr/share/padcast/config/config.ini";
                if (std::filesystem::exists(padcast_config))
                {
                    std::filesystem::copy_file(padcast_config, config_file);
                }
            }

            return config_file.string();
        }
    }

    static std::string getResourcePath(const std::string& relativePath)
    {
        if (isPortableMode())
        {
            // Portable (Windows), resource next to exe
            return (std::filesystem::current_path() / "resources" / relativePath).string();
        }
        else
        {
            // Check if we're running from build directory
            std::filesystem::path build_dir = (std::filesystem::current_path() / "resources" / relativePath);
            if (std::filesystem::exists(build_dir))
            {
                return build_dir.string();
            }

            // Linux system install
            std::filesystem::path systemPath = std::filesystem::path("/usr/share/padcast/resources") / relativePath;
            return systemPath.string();
        }
    }

    static bool validateResourcePath(const std::string& path)
    // Not used but here for safety checks later
    {
        if (!std::filesystem::exists(path))
        {
            std::println("ERROR: Resource not found: {}", path);
            return false;
        }
        return true;
    }
};

#endif