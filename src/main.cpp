#include <iostream>
#include "window.h"
#include "argparse.h"

#include "stateman.h"

#define WINSIZE_X 1280
#define WINSIZE_Y 720
#define WINFPS    60

int main(int argc, char **argv) {
    ProgramOpt opt(argc, argv, 2);
    if (opt.mError > 0) return -1;

    Window w(WINSIZE_X, WINSIZE_Y, WINFPS, "imgview");
    std::optional<std::string> fail = w.initWindow();
    if (fail.has_value()) std::cerr << fail.value();

    StateManager sm = {};

    State *s = new State(&w, opt.mInputFile[0].c_str());
    if (s->mError >0) return -1;

    size_t idx = sm.addState(s);
    if (!sm.activeState(idx)) return -1;

    w.startWindowLoops(&sm);

    delete s;
    return 0;
}
