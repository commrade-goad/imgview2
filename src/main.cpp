#include <iostream>
#include "window.h"

#define WINSIZE_X 1280
#define WINSIZE_Y 720
#define WINFPS    60

int main() {
    Window w(WINSIZE_X, WINSIZE_Y, WINFPS, "imgview");
    std::optional<std::string> fail = w.initWindow();
    if (fail.has_value()) std::cerr << fail.value();
    w.startWindowLoops();
    return 0;
}
