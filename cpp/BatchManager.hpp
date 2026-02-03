#pragma once

#include <set>
#include <mutex>
#include <jsi/jsi.h>
#include "AtomCore.hpp"

namespace nitrostate {

using namespace facebook;

/**
 * BatchManager - Batches multiple atom updates
 * 
 * Defers notifications until batch ends, preventing
 * unnecessary re-renders during bulk updates.
 */
class BatchManager {
public:
    BatchManager() = default;
    ~BatchManager() = default;

    // Non-copyable
    BatchManager(const BatchManager&) = delete;
    BatchManager& operator=(const BatchManager&) = delete;

    /**
     * Start a batch operation
     */
    void startBatch();

    /**
     * End batch and notify all queued atoms
     */
    void endBatch();

    /**
     * Queue an atom for notification (called during set)
     */
    void queueNotification(AtomCore* atom);

    /**
     * Check if currently batching
     */
    bool isBatching() const { return batchDepth_ > 0; }

    /**
     * Get singleton instance
     */
    static BatchManager& instance();

private:
    std::set<AtomCore*> pendingNotifications_;
    int batchDepth_ = 0;
    mutable std::mutex mutex_;
};

} // namespace nitrostate
