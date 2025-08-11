#include "stateman.h"

StateManager::StateManager() {
    mActive = nullptr;
    mStates.reserve(10);
}

State *StateManager::_searchState(const char *path, size_t *idx) {
    for (auto &s: mStates) {
        if (strncmp(s.mPath, path, strlen(path)) == 0) {
            return &s;
        }
        if (idx) *idx += 1;
    }
    return nullptr;
}

bool StateManager::activeState(size_t idx) {
    if (idx < mStates.size()) {
        mActive = &mStates[idx];
        return true;
    }
    return false;
}

// NOTE: Worse than the idx... Please use it if necessary only.
bool StateManager::activeState(const char *path) {
    if (auto res = _searchState(path, nullptr)) {
        mActive = res;
        return true;
    }
    return false;
}

void StateManager::addState(State s) { mStates.push_back(s); }

void StateManager::deleteState(const char *path) {
    size_t idx = 0;
    if (_searchState(path, &idx)) {
        mStates.erase(mStates.begin() + idx);
    }
}
void StateManager::deleteState(size_t idx) { mStates.erase(mStates.begin() + idx); }
