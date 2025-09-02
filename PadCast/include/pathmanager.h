#ifndef PADCAST_PATHMANAGER_H
#define PADCAST_PATHMANAGER_H

#include <filesystem>
#include <string>
#include <cstdlib>
//#include <cstring>
#include <iostream>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif

    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    
    #define NOGDI
    #define NOUSER
    #define NODRAWTEXT

    #include <Windows.h>

    #undef NOGDI
    #undef NOUSER
    #undef NODRAWTEXT

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
            std::filesystem::path config_base;
            const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME");

            if (xdg_config_home && xdg_config_home[0] != '\0')
            {
                config_base = xdg_config_home;
            }
            else
            {
                const char* home = std::getenv("HOME");
                if (home && home[0] != '\0')
                {
                    config_base = std::filesystem::path(home) / ".config";
                }
                else
                {
                    // Unlikely fallback to current directory if HOME and XDG_CONFIG_HOME are not set.
                    return (std::filesystem::current_path() / "config.ini").string();
                }
            }

            auto config_dir = config_base / "padcast";
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
                        std::cerr << "ERROR: Default config file not found at " << padcast_config.string() << std::endl;
                    }
                }
            }
            catch (const std::filesystem::filesystem_error& e) 
            {
                 std::cerr << "ERROR: Failed to copy default config file: " << e.what() << std::endl;
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
            std::cerr << "ERROR: Resource not found: " << path << std::endl;
            return false;
        }
        return true;
    }
};

#endif