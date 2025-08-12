#include "stateman.h"

StateManager::StateManager() {
    mActive = nullptr;
    mStates.reserve(10);
}

StateManager::~StateManager() {
    for (auto &state: mStates) {
        delete state;
    }
}

State *StateManager::_searchState(const char *path, size_t *idx) {
    for (auto &s: mStates) {
        if (strncmp(s->mPath, path, strlen(path)) == 0) {
            return s;
        }
        if (idx) *idx += 1;
    }
    return nullptr;
}

bool StateManager::activeState(size_t idx) {
    if (idx < mStates.size()) {
        mActive = mStates[idx];
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

int StateManager::makeNewState(Window *w, const char *path, SDL_ScaleMode scaleMode) {
    State *s = new State(w, path, scaleMode);
    if (s->mError >0) return -1;

    return addState(s);
}

int StateManager::makeNewState(Window *w, const char *path) {
    State *s = new State(w, path, SDL_SCALEMODE_NEAREST);
    if (s->mError >0) return -1;

    return addState(s);
}

size_t StateManager::addState(State *s) {
    mStates.push_back(s);
    return mStates.size() - 1;
}

void StateManager::deleteState(const char *path) {
    size_t idx = 0;
    if (_searchState(path, &idx)) {
        mStates.erase(mStates.begin() + idx);
    }
}
void StateManager::deleteState(size_t idx) { mStates.erase(mStates.begin() + idx); }
