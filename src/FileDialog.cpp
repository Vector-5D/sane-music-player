#include "FileDialog.hpp"
#include "Logger.hpp"
#include "nfd.h"

// All static functions so no need for a class
namespace FileDialog {
    std::optional<std::string> openFile(const std::string& filter) {
        nfdchar_t* out_path = nullptr;
        const char* filter_str = filter.empty() ? nullptr : filter.c_str();

        nfdresult_t result = NFD_OpenDialog(filter_str, nullptr, &out_path);
        if (result == NFD_OKAY) {
            std::string path(out_path);
            free(out_path);
            LOG_INFO("File selected: %s", path.c_str());
            return path;
        } else if (result == NFD_CANCEL) {
            LOG_INFO("User cancelled file dialog.");
        } else {
            LOG_ERROR("File dialog error: %s", NFD_GetError());
        }
        return std::nullopt;
    }

    std::optional<std::string> openFolder() {
        nfdchar_t* out_path = nullptr;

        nfdresult_t result = NFD_PickFolder(nullptr, &out_path);
        if (result == NFD_OKAY) {
            std::string path(out_path);
            free(out_path);
            LOG_INFO("Folder selected: %s", path.c_str());
            return path;
        } else if (result == NFD_CANCEL) {
            LOG_INFO("User cancelled folder dialog.");
        } else {
            LOG_ERROR("Folder dialog error: %s", NFD_GetError());
        }
        return std::nullopt;
    }

    std::optional<std::vector<std::string>> openMultiple(const std::string& filter) {
        nfdpathset_t path_set;
        const char* filter_str = filter.empty() ? nullptr : filter.c_str();

        nfdresult_t result = NFD_OpenDialogMultiple(filter_str, nullptr, &path_set);
        if (result == NFD_OKAY) {
            size_t count = NFD_PathSet_GetCount(&path_set);
            std::vector<std::string> paths;
            paths.reserve(count);

            for (size_t i = 0; i < count; i++)
                paths.emplace_back(NFD_PathSet_GetPath(&path_set, i));

            NFD_PathSet_Free(&path_set);
            LOG_INFO("Selected %zu files.", count);
            return paths;
        } else if (result == NFD_CANCEL) {
            LOG_INFO("User cancelled multiple file dialog.");
        } else {
            LOG_ERROR("Multiple file dialog error: %s", NFD_GetError());
        }
        return std::nullopt;
    }
}
