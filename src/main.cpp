#include <iostream>
#include <thread>
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
    if (opt.mError > 0) return 1;
    if (opt.instantExit) return 1;

    Window w(WINSIZE_X, WINSIZE_Y, WINFPS, WINAME);
    auto fail = w.initWindow();
    if (fail.has_value()) std::cerr << fail.value();

    StateManager sm = {};
    std::thread smThread(&StateManager::mainLoop, &sm, opt.mThreadCount);

    for (auto &fileIn: opt.mInputFile) {
        int idx = sm.newState(&w, fileIn.c_str());
        if (idx < 0) {
            continue;
        }
    }

    if (!sm.activateState(0ul)) {
        sm.stopLoop();
        smThread.join();
        return 1;
    }

    if (!w.startWindowLoop(&sm)) {
        sm.stopLoop();
        smThread.join();
        return 1;
    }

    sm.stopLoop();
    smThread.join();

    return 0;
}
