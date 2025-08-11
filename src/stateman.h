#pragma once

#include "state.h"
#include <vector>

class StateManager {
    public:
        State *mActive;
        std::vector<State *> mStates;

        StateManager();
        ~StateManager() = default;

        bool activeState(size_t idx);
        bool activeState(const char *path);

        size_t addState(State *s);
        void deleteState(const char *path);
        void deleteState(size_t idx);

    private:
        State *_searchState(const char *path, size_t *idx);
};
