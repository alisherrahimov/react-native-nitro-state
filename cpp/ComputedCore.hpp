#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <jsi/jsi.h>
#include "AtomCore.hpp"

namespace nitrostate {

using namespace facebook;

/**
 * ComputedCore - Derived/computed reactive value
 * 
 * Lazily computes a value based on dependencies.
 * Automatically re-computes when dependencies change.
 */
class ComputedCore {
public:
    using ComputeFn = std::function<jsi::Value(jsi::Runtime&)>;

    explicit ComputedCore(ComputeFn compute);
    ~ComputedCore();

    // Non-copyable, movable
    ComputedCore(const ComputedCore&) = delete;
    ComputedCore& operator=(const ComputedCore&) = delete;
    ComputedCore(ComputedCore&&) = default;
    ComputedCore& operator=(ComputedCore&&) = default;

    /**
     * Get the computed value (lazy evaluation)
     */
    jsi::Value get(jsi::Runtime& rt);

    /**
     * Add a dependency atom
     */
    void addDependency(AtomCore* atom);

    /**
     * Mark as dirty (called when any dependency changes)
     */
    void markDirty();

    /**
     * Check if needs recomputation
     */
    bool isDirty() const { return dirty_; }

private:
    ComputeFn compute_;
    std::vector<AtomCore*> dependencies_;
    std::vector<AtomCore::SubscriberId> subscriptionIds_;
    std::shared_ptr<jsi::Value> cachedValue_;
    bool dirty_ = true;
};

} // namespace nitrostate
