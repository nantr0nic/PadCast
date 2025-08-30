#ifndef PADCAST_PATHMANAGER_H
#define PADCAST_PATHMANAGER_H

#include <filesystem>
#include <string>
#include <cstdlib>
#include <cstring>
#include <print>

#ifdef _WIN32
#include <Windows.h>
// MAX_PATH is 260, which might not be enough for modern Windows.
// Using a larger buffer is safer.
#ifndef MAX_PATH_LONG
#define MAX_PATH_LONG 32767
#endif
#endif

// PADCAST_DATA_DIR should be configured/defined in CMakeLists but if its not
// this is the fallback
#ifndef PADCAST_DATA_DIR
#define PADCAST_DATA_DIR "/usr/share/padcast"
#endif

class PathManager
{
private:
    // Caches the executable's directory path on first call.
    static const std::filesystem::path& getExecutableDir()
    {
        // This is a static lambda that runs only once to initialize execDir.
        static const std::filesystem::path execDir = []() -> std::filesystem::path 
        {
        #ifdef _WIN32
            wchar_t path[MAX_PATH_LONG];
            // GetModuleFileNameW is the modern, Unicode-safe way to get the executable path.
            if (GetModuleFileNameW(NULL, path, MAX_PATH_LONG) == 0) 
            {
                // Fallback in case of an error
                return std::filesystem::current_path();
            }
            return std::filesystem::path(path).parent_path();
        #elif defined(__linux__)
            try 
            {
                // /proc/self/exe is a symbolic link to the running executable.
                return std::filesystem::read_symlink("/proc/self/exe").parent_path();
            } 
            catch (const std::filesystem::filesystem_error&) 
            {
                // Fallback if /proc isn't available (unlikely on modern Linux)
                return std::filesystem::current_path();
            }
        #else
            // A generic, less reliable fallback for other OSes (like macOS).
            // A more robust solution for macOS would use _NSGetExecutablePath.
            return std::filesystem::current_path();
        #endif
        }();
        return execDir;
    }

    static bool isPortableMode()
    {
        const auto& execDir = getExecutableDir();
        // Portable mode is detected if a 'resources' or 'config' folder
        // exists in the same directory as the executable. This works for
        // Windows releases and Linux development builds.
        return (std::filesystem::exists(execDir / "resources") ||
                std::filesystem::exists(execDir / "config"));
    }

public:
    static std::string getConfigFilePath()
    {
        if (isPortableMode())
        {
            // Portable mode: config is next to the executable.
            const auto& execDir = getExecutableDir();
            auto configDir = execDir / "config";
            std::filesystem::create_directories(configDir);
            return (configDir / "config.ini").string();
        }
        else
        {
            // System install (Linux): Use XDG Base Directory Specification.
            const char* xdg_config = std::getenv("XDG_CONFIG_HOME");
            std::filesystem::path config_dir;

            if (xdg_config && strlen(xdg_config) > 0)
            {
                config_dir = std::filesystem::path(xdg_config) / "padcast";
            }
            else
            {
                // Fallback if XDG_CONFIG_HOME isn't set.
                const char* home = std::getenv("HOME");
                if (home)
                {
                    config_dir = (std::filesystem::path(home) / ".config" / "padcast");
                }
                else
                {
                    // Unlikely fallback to current directory if HOME is not set.
                    return (std::filesystem::current_path() / "config.ini").string();
                }
            }

            std::filesystem::create_directories(config_dir);

            std::filesystem::path config_file = config_dir / "config.ini";
            // If user config doesn't exist, copy the default one from the system data dir.
            try
            {
                if (!std::filesystem::exists(config_file))
                {
                    std::filesystem::path padcast_config = std::filesystem::path(PADCAST_DATA_DIR) / "config" / "config.ini";
                    if (std::filesystem::exists(padcast_config))
                    {
                        std::filesystem::copy_file(padcast_config, config_file);
                    } 
                    else 
                    {
                        std::println("ERROR: Default config file not found at {}", padcast_config.string());
                    }
                }
            }
            catch (const std::filesystem::filesystem_error& e) 
            {
                 std::println("ERROR: Failed to copy default config file: {}", e.what());
            }

            return config_file.string();
        }
    }

    static std::string getResourcePath(const std::string& relativePath)
    {
        if (isPortableMode())
        {
            // Portable mode: resources are next to the executable.
            return (getExecutableDir() / "resources" / relativePath).string();
        }
        else
        {
            // System install (Linux): resources are in the shared data directory
            // defined by CMake.
            return (std::filesystem::path(PADCAST_DATA_DIR) / "resources" / relativePath).string();
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