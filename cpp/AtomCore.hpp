#pragma once

#include <functional>
#include <vector>
#include <mutex>
#include <memory>
#include <jsi/jsi.h>

namespace nitrostate {

using namespace facebook;

/**
 * AtomCore - The fundamental reactive primitive
 * 
 * Stores a JSI value and notifies subscribers when it changes.
 * Thread-safe for concurrent access.
 */
class AtomCore {
public:
    using SubscriberId = size_t;
    using Callback = std::function<void()>;

    explicit AtomCore(jsi::Runtime& rt, const jsi::Value& initialValue);
    ~AtomCore() = default;

    // Non-copyable, movable
    AtomCore(const AtomCore&) = delete;
    AtomCore& operator=(const AtomCore&) = delete;
    AtomCore(AtomCore&&) = default;
    AtomCore& operator=(AtomCore&&) = default;

    /**
     * Get the current value
     */
    jsi::Value get(jsi::Runtime& rt) const;

    /**
     * Set a new value and notify subscribers
     */
    void set(jsi::Runtime& rt, const jsi::Value& value);

    /**
     * Subscribe to value changes
     * @return Subscriber ID for unsubscription
     */
    SubscriberId subscribe(Callback callback);

    /**
     * Unsubscribe from value changes
     */
    void unsubscribe(SubscriberId id);

    /**
     * Notify all subscribers (called after batch ends)
     */
    void notify();

    /**
     * Check if value has changed (for batch optimization)
     */
    bool isDirty() const { return dirty_; }

    /**
     * Mark as clean after notification
     */
    void markClean() { dirty_ = false; }

private:
    std::shared_ptr<jsi::Value> value_;
    std::vector<std::pair<SubscriberId, Callback>> subscribers_;
    mutable std::mutex mutex_;
    SubscriberId nextId_ = 0;
    bool dirty_ = false;
};

} // namespace nitrostate
