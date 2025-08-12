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

State *StateManager::_searchState(const char *path, size_t *idx) {
    for (auto &s: mStates) {
        if (idx) *idx += 1;
        if (strncmp(s->mPath, path, strlen(path)) == 0) {
            return s;
        }
    }
    return nullptr;
}

bool StateManager::activeteState(size_t idx) {
    if (idx < mStates.size()) {
        mActive = mStates[idx];
        mActiveIdx = idx;
        if (!mActive->mTextureLoaded) {
            mActive->createTexture();
        }
        return true;
    }
    return false;
}

// NOTE: Worse than the idx... Please use it if necessary only.
bool StateManager::activeteState(const char *path) {
    size_t idx = -1;
    if (auto res = _searchState(path, &idx)) {
        mActiveIdx = idx;
        mActive = res;
        if (!mActive->mTextureLoaded) {
            auto status = mActive->loadEverythingSync();
            if (status.has_value()) {
                std::cerr << status.value();
                return false;
            }
        }
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
    std::lock_guard<std::mutex> lock(mMutex);

    mStates.push_back(s);
    mQueue.push(s);

    return mStates.size() - 1;
}

void StateManager::deleteState(const char *path) {
    size_t idx = 0;
    if (auto state = _searchState(path, &idx)) {
        mStates.erase(mStates.begin() + idx);
        delete state;
    }
}

void StateManager::deleteState(size_t idx) {
    mStates.erase(mStates.begin() + idx);
}

void StateManager::mainLoop() {
    static const size_t maxThreadCount = 5;
    std::atomic<size_t> threadCount  = 0;
    threadCount = 0;
    while (mRunning) {
        State *s = nullptr;
        {
            std::unique_lock<std::mutex> lock(mMutex, std::try_to_lock);
            if (!lock) break;
            if (!mQueue.empty() && threadCount <= maxThreadCount) {
                s = mQueue.front();
                mQueue.pop();
                ++threadCount;
            }
        }

        if (s) {
            std::thread worker([s, &threadCount]() {
                    auto res = s->loadImage();
                    if (res.has_value()) std::cerr << res.value();
                    else std::cout << "[WORKER]: done processing " << s->mPath << std::endl;
                    ++threadCount;
                    return;
                    });
            worker.detach();
            // worker.join();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void StateManager::stopLoop() {
    mRunning = false;
}
