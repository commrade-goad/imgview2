#pragma once

#include <cstdio>

class Image {
    public:
        int x, y, n;
        unsigned char *data;
        FILE *file_handler;
};
