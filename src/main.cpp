#include <iostream>
#include "window.h"
#include "argparse.h"
#include "stateman.h"

#define WINAME    "imgview"
#define WINSIZE_X 1280
#define WINSIZE_Y 720
#define WINFPS    60
#define MINARGS   2

int main(int argc, char **argv) {
    ProgramOpt opt(argc, argv, MINARGS);
    if (opt.mError > 0) return -1;

    Window w(WINSIZE_X, WINSIZE_Y, WINFPS, WINAME);
    auto fail = w.initWindow();
    if (fail.has_value()) std::cerr << fail.value();

    StateManager sm = {};

    int idx = sm.makeNewState(&w, opt.mInputFile[0].c_str());
    if (idx < 0) return -1;

    if (!sm.activeState(idx)) return -1;
    if (!w.startWindowLoops(&sm)) return -1;

    return 0;
}
