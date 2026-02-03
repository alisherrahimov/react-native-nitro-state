#include "BatchManager.hpp"

namespace nitrostate {

BatchManager& BatchManager::instance() {
    static BatchManager instance;
    return instance;
}

void BatchManager::startBatch() {
    std::lock_guard<std::mutex> lock(mutex_);
    batchDepth_++;
}

void BatchManager::endBatch() {
    std::set<AtomCore*> atomsToNotify;
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        batchDepth_--;
        
        if (batchDepth_ == 0) {
            atomsToNotify = std::move(pendingNotifications_);
            pendingNotifications_.clear();
        }
    }
    
    // Notify outside lock
    for (auto* atom : atomsToNotify) {
        atom->notify();
        atom->markClean();
    }
}

void BatchManager::queueNotification(AtomCore* atom) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (batchDepth_ > 0) {
        pendingNotifications_.insert(atom);
    }
}

} // namespace nitrostate
