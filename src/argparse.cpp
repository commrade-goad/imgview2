#include "argparse.h"
#include <cstring>
#include <iostream>
#include <cstring>

std::optional<std::string> ProgramOpt::checkArgs(int argc, size_t min) {
    std::string buffer;
    buffer.resize(512);
    if (argc < (int)min) {
        snprintf(buffer.data(), buffer.size(), "ERROR: Need atleast %zu args.\n", min - 1);
    }
    if (strlen(buffer.data()) > 0) return buffer;
    else                           return std::nullopt;
}

ProgramOpt::ProgramOpt(int argc, char **argv, size_t min) {
    mError = 0;
    mCheckWayland = true;
    mWindowSize = std::pair<size_t, size_t> (1280, 720);

    auto status = checkArgs(argc, min);
    if (status.has_value()) {
        std::cerr << status.value();
        mError++;
    }

    bool capture_mode = false;
    char flag = '\0';

    for (int i = 1; i < argc; i++) {
        char *current = argv[i];
        size_t current_len = strlen(current);

        if (current[0] == '-') {
            for (size_t j = 1; j < current_len; j++) {
                switch (current[j]) {
                case 'x':
                    mCheckWayland = false;
                    break;
                case 'w':
                    capture_mode = true;
                    flag = 'w';
                    break;
                case 'j':
                    capture_mode = true;
                    flag = 'j';
                    break;
                default:
                    std::cerr << "ERROR: Unknown option: " << current[j] << std::endl;
                    mError++;
                    break;
                }
            }
        } else {
            if (!capture_mode) {
                mInputFile.push_back(current);
                continue;
            }

            switch (flag) {
                case 'w':
                    {
                        int xpos = -1;
                        for (int j = 0; j < (int)current_len; j++) {
                            if (current[j] == 'x') {
                                xpos = j;
                                break;
                            }
                        }

                        if (xpos < 0) {
                            std::cerr << "ERROR: Not a valid WxH format.\n";
                            mError++;
                        }

                        size_t rside = atoll(current + (xpos + 1));
                        current[xpos] = '\0';
                        size_t lside = atoll(current);
                        current[xpos] = 'x';

                        mWindowSize =
                            std::pair<int, int>(
                                    lside > 100 ? lside : 1280,
                                    rside > 100 ? rside : 720
                                    );
                        flag = '\0';
                        break;
                    }
                case 'j':
                    {
                        mThreadCount = atol(current);
                        flag = '\0';
                        break;
                    }

                default:
                    break;
            }
            capture_mode = false;
        }
    }
}
