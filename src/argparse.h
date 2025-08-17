#pragma once

#include <utility>
#include <vector>
#include <string>
#include <optional>

class ProgramOpt{
    public:
        int mError;
        bool mCheckWayland;
        bool instantExit;
        size_t mThreadCount;
        std::pair<int, int> mWindowSize;
        std::vector<std::string> mInputFile;

        ProgramOpt(int argc, char **argv, size_t min);
        ~ProgramOpt() = default;
        std::optional<std::string> checkArgs(int argc, size_t min);
};
