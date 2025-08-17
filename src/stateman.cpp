#include "stateman.h"
#include <iostream>
#include <thread>
#include <algorithm>

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

        size_t before = std::clamp(mActiveIdx - 3, 0, (int)mStates.size());
        size_t after  = std::clamp(mActiveIdx + 3, 0, (int)mStates.size());

        for (size_t i = 0; i < mStates.size(); i++) {
            if ((i >= before && i <= after) || mActive->mPath == mStates[i]->mPath) {
                if (!mStates[i]->mImageDataLoaded)
                    addToQueue(mStates[i]);
            } else {
                if (mActiveIdx == 0) continue;
                mStates[i]->resetTextureAndImage();
            }
        }

        // if (!mActive->mImageDataLoaded)
        //     addToQueue(mActive);
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
    if (s->mError > 0) {
        delete s;
        return -1;
    };

    return addState(s);
}

int StateManager::newState(Window *w, const char *path) {
    State *s = new State(w, path, SDL_SCALEMODE_LINEAR);
    if (s->mError > 0) {
        delete s;
        return -1;
    };

    return addState(s);
}

size_t StateManager::addState(State *s) {
    mStates.push_back(s);
    // addToQueue(s);
    return mStates.size() - 1;
}

void StateManager::addToQueue(State *s) {
    std::lock_guard<std::mutex> lock(mMutex);
    auto exists = std::find_if(mQueue.begin(), mQueue.end(),
            [&](State* st) { return st->mPath == s->mPath; });

    if (exists != mQueue.end()) return;
    mQueue.push_back(s);

}

void StateManager::deleteState(const char *path) {
    std::lock_guard<std::mutex> lock(mMutex);
    int idx = -1;
    if (auto state = _searchState(path, &idx)) {
        // TODO: Delete inside the queue too.
        mStates.erase(mStates.begin() + idx);
        delete state;
    }
}

void StateManager::deleteState(size_t idx) {
    std::lock_guard<std::mutex> lock(mMutex);
    // TODO: Delete inside the queue too.
    delete mStates[idx];
    mStates.erase(mStates.begin() + idx);
}

void StateManager::mainLoop(size_t maxThreadCount) {
    static const size_t delayMs = 30;
    std::atomic<size_t> threadCount = 0;
    while (mRunning) {
        State *s = nullptr;
        {
            std::unique_lock<std::mutex> lock(mMutex, std::try_to_lock);
            if (!lock) break;
            if (!mQueue.empty() && threadCount < maxThreadCount) {
                s = mQueue.front();
                mQueue.pop_front();
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
