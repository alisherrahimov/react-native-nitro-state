#include "HybridNitroState.hpp"

namespace margelo::nitro::nitrostate {

// ----- Atom Operations -----

void HybridNitroState::createAtom(
    const std::string& key,
    const std::shared_ptr<AnyMap>& initialValue
) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (atoms_.find(key) != atoms_.end()) {
        throw std::runtime_error("Atom with key '" + key + "' already exists");
    }
    
    atoms_[key] = initialValue;
}

std::shared_ptr<AnyMap> HybridNitroState::getAtomValue(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = atoms_.find(key);
    if (it == atoms_.end()) {
        throw std::runtime_error("Atom with key '" + key + "' not found");
    }
    
    return it->second;
}

void HybridNitroState::setAtomValue(
    const std::string& key,
    const std::shared_ptr<AnyMap>& value
) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = atoms_.find(key);
    if (it == atoms_.end()) {
        throw std::runtime_error("Atom with key '" + key + "' not found");
    }
    
    it->second = value;
    
    // Notify subscribers
    if (isBatching_) {
        pendingNotifications_.push_back(key);
    } else {
        auto subIt = subscribers_.find(key);
        if (subIt != subscribers_.end()) {
            for (const auto& [id, callback] : subIt->second) {
                callback();
            }
        }
    }
}

std::function<void()> HybridNitroState::subscribeAtom(
    const std::string& key,
    const std::function<void()>& callback
) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = atoms_.find(key);
    if (it == atoms_.end()) {
        throw std::runtime_error("Atom with key '" + key + "' not found");
    }
    
    size_t subscriberId = nextSubscriberId_++;
    subscribers_[key].push_back({subscriberId, callback});
    
    // Return unsubscribe function
    return [this, key, subscriberId]() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& subs = subscribers_[key];
        subs.erase(
            std::remove_if(subs.begin(), subs.end(),
                [subscriberId](const auto& pair) { return pair.first == subscriberId; }),
            subs.end()
        );
    };
}

void HybridNitroState::deleteAtom(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    atoms_.erase(key);
    subscribers_.erase(key);
}

// ----- Computed Operations -----

void HybridNitroState::createComputed(
    const std::string& key,
    const std::vector<std::string>& dependencies,
    const std::function<std::shared_ptr<Promise<std::shared_ptr<AnyMap>>>()>& compute
) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (computed_.find(key) != computed_.end()) {
        throw std::runtime_error("Computed with key '" + key + "' already exists");
    }
    
    // Store compute function
    computeFns_[key] = compute;
    
    // Subscribe to dependencies to invalidate cache
    for (const auto& depKey : dependencies) {
        if (atoms_.find(depKey) != atoms_.end()) {
            // When dependency changes, clear cached value
            subscribeAtom(depKey, [this, key]() {
                std::lock_guard<std::mutex> lock(mutex_);
                computed_.erase(key);
            });
        }
    }
}

std::shared_ptr<AnyMap> HybridNitroState::getComputedValue(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check cache first
    auto cachedIt = computed_.find(key);
    if (cachedIt != computed_.end()) {
        return cachedIt->second;
    }
    
    // Compute value
    auto fnIt = computeFns_.find(key);
    if (fnIt == computeFns_.end()) {
        throw std::runtime_error("Computed with key '" + key + "' not found");
    }
    
    // Call compute function and wait for result
    auto promise = fnIt->second();
    auto future = promise->await();
    auto result = future.get();
    
    // Cache result
    computed_[key] = result;
    
    return result;
}


void HybridNitroState::deleteComputed(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    computed_.erase(key);
    computeFns_.erase(key);
}

// ----- Batch Operations -----

void HybridNitroState::startBatch() {
    std::lock_guard<std::mutex> lock(mutex_);
    isBatching_ = true;
    pendingNotifications_.clear();
}

void HybridNitroState::endBatch() {
    std::lock_guard<std::mutex> lock(mutex_);
    isBatching_ = false;
    
    // Notify all pending subscribers
    for (const auto& key : pendingNotifications_) {
        auto subIt = subscribers_.find(key);
        if (subIt != subscribers_.end()) {
            for (const auto& [id, callback] : subIt->second) {
                callback();
            }
        }
    }
    
    pendingNotifications_.clear();
}

// ----- Utility -----

bool HybridNitroState::hasAtom(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    return atoms_.find(key) != atoms_.end();
}

std::vector<std::string> HybridNitroState::getAtomKeys() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> keys;
    keys.reserve(atoms_.size());
    for (const auto& [key, _] : atoms_) {
        keys.push_back(key);
    }
    return keys;
}

} // namespace margelo::nitro::nitrostate
