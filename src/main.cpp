#include <iostream>
#include "window.h"
#include "argparse.h"

#include "state.h"

#define WINSIZE_X 1280
#define WINSIZE_Y 720
#define WINFPS    60

int main(int argc, char **argv) {
    ProgramOpt opt(argc, argv, 2);
    if (opt.mError > 0) return -1;

    Window w(WINSIZE_X, WINSIZE_Y, WINFPS, "imgview");
    std::optional<std::string> fail = w.initWindow();
    if (fail.has_value()) std::cerr << fail.value();

    State s(&w, opt.mInputFile[0].c_str());
    if (s.mError >0) return -1;

    w.startWindowLoops();
    return 0;
}
