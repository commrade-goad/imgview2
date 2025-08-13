#include "stateman.h"
#include <iostream>
#include <thread>

StateManager::StateManager() {
    mActive = nullptr;
    mActiveIdx = -1;
    mStates.reserve(10);
    mRunning = true;
}

StateManager::~StateManager() {
    for (auto &state: mStates) {
        delete state;
    }
}

State *StateManager::_searchState(const char *path, int *idx) {
    if (idx) *idx = -1;
    for (auto &s: mStates) {
        if (idx) *idx += 1;
        if (strcmp(s->mPath, path) == 0) { return s; }
    }
    return nullptr;
}

bool StateManager::activateState(size_t idx) {
    if (idx < mStates.size()) {
        mActive = mStates[idx];
        mActiveIdx = idx;
        if (!mActive->mTextureLoaded) {
            mActive->createTexture();
        }
        if (mActive->mFitIn) mActive->fitTextureToWindow();
        return true;
    }
    return false;
}

// NOTE: Worse than the idx... Please use it if necessary only.
bool StateManager::activateState(const char *path) {
    int idx = -1;
    if (auto res = _searchState(path, &idx)) {
        mActiveIdx = idx;
        mActive = res;
        if (!mActive->mTextureLoaded) {
            mActive->createTexture();
        }
        if (mActive->mFitIn) mActive->fitTextureToWindow();
        return true;
    }
    return false;
}

int StateManager::newState(Window *w, const char *path, SDL_ScaleMode scaleMode) {
    State *s = new State(w, path, scaleMode);
    if (s->mError >0) return -1;

    return addState(s);
}

int StateManager::newState(Window *w, const char *path) {
    State *s = new State(w, path, SDL_SCALEMODE_LINEAR);
    if (s->mError >0) return -1;

    return addState(s);
}

size_t StateManager::addState(State *s) {
    std::lock_guard<std::mutex> lock(mMutex);

    mStates.push_back(s);
    mQueue.push(s);

    return mStates.size() - 1;
}

void StateManager::deleteState(const char *path) {
    int idx = -1;
    if (auto state = _searchState(path, &idx)) {
        mStates.erase(mStates.begin() + idx);
        delete state;
    }
}

void StateManager::deleteState(size_t idx) {
    delete mStates[idx];
    mStates.erase(mStates.begin() + idx);
}

void StateManager::mainLoop() {
    static const size_t delayMs = 30;
    static const size_t maxThreadCount = 5;
    std::atomic<size_t> threadCount  = 0;
    threadCount = 0;
    while (mRunning) {
        State *s = nullptr;
        {
            std::unique_lock<std::mutex> lock(mMutex, std::try_to_lock);
            if (!lock) break;
            if (!mQueue.empty() && threadCount < maxThreadCount) {
                s = mQueue.front();
                mQueue.pop();
                ++threadCount;
            }
        }

        if (s) {
            std::thread worker([s, &threadCount]() {
                auto res = s->loadImage();

                if (res.has_value()) std::cerr << res.value();

                --threadCount;
                return;
            });
            worker.detach();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
}

void StateManager::stopLoop() {
    mRunning = false;
}
