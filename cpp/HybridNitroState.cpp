#include "HybridNitroState.hpp"

namespace nitrostate {

HybridNitroState::HybridNitroState() : HybridObject("NitroState") {}

void HybridNitroState::loadHybridMethods() {
    // Register methods that will be exposed to JS
    registerHybridMethod("createAtom", &HybridNitroState::createAtom, this);
    registerHybridMethod("getAtomValue", &HybridNitroState::getAtomValue, this);
    registerHybridMethod("setAtomValue", &HybridNitroState::setAtomValue, this);
    registerHybridMethod("subscribeAtom", &HybridNitroState::subscribeAtom, this);
    registerHybridMethod("deleteAtom", &HybridNitroState::deleteAtom, this);
    
    registerHybridMethod("createComputed", &HybridNitroState::createComputed, this);
    registerHybridMethod("getComputedValue", &HybridNitroState::getComputedValue, this);
    registerHybridMethod("deleteComputed", &HybridNitroState::deleteComputed, this);
    
    registerHybridMethod("startBatch", &HybridNitroState::startBatch, this);
    registerHybridMethod("endBatch", &HybridNitroState::endBatch, this);
    
    registerHybridMethod("hasAtom", &HybridNitroState::hasAtom, this);
    registerHybridMethod("getAtomKeys", &HybridNitroState::getAtomKeys, this);
}

// ----- Atom Operations -----

void HybridNitroState::createAtom(
    const std::string& key, 
    jsi::Runtime& rt, 
    const jsi::Value& initialValue
) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (atoms_.find(key) != atoms_.end()) {
        throw std::runtime_error("Atom with key '" + key + "' already exists");
    }
    
    atoms_[key] = std::make_unique<AtomCore>(rt, initialValue);
}

jsi::Value HybridNitroState::getAtomValue(const std::string& key, jsi::Runtime& rt) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = atoms_.find(key);
    if (it == atoms_.end()) {
        throw std::runtime_error("Atom with key '" + key + "' not found");
    }
    
    return it->second->get(rt);
}

void HybridNitroState::setAtomValue(
    const std::string& key, 
    jsi::Runtime& rt, 
    const jsi::Value& value
) {
    AtomCore* atom = nullptr;
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = atoms_.find(key);
        if (it == atoms_.end()) {
            throw std::runtime_error("Atom with key '" + key + "' not found");
        }
        atom = it->second.get();
    }
    
    // Check if batching
    if (BatchManager::instance().isBatching()) {
        // Store value but defer notification
        atom->set(rt, value);
        atom->markClean(); // Don't notify yet
        BatchManager::instance().queueNotification(atom);
    } else {
        atom->set(rt, value);
    }
}

jsi::Function HybridNitroState::subscribeAtom(
    const std::string& key,
    jsi::Runtime& rt,
    jsi::Function callback
) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = atoms_.find(key);
    if (it == atoms_.end()) {
        throw std::runtime_error("Atom with key '" + key + "' not found");
    }
    
    // Store callback as shared_ptr for capture
    auto callbackPtr = std::make_shared<jsi::Function>(std::move(callback));
    auto* atom = it->second.get();
    
    auto subscriptionId = atom->subscribe([callbackPtr, &rt]() {
        callbackPtr->call(rt);
    });
    
    // Return unsubscribe function
    return jsi::Function::createFromHostFunction(
        rt,
        jsi::PropNameID::forAscii(rt, "unsubscribe"),
        0,
        [atom, subscriptionId](
            jsi::Runtime& rt,
            const jsi::Value& thisVal,
            const jsi::Value* args,
            size_t count
        ) -> jsi::Value {
            atom->unsubscribe(subscriptionId);
            return jsi::Value::undefined();
        }
    );
}

void HybridNitroState::deleteAtom(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    atoms_.erase(key);
}

// ----- Computed Operations -----

void HybridNitroState::createComputed(
    const std::string& key,
    jsi::Runtime& rt,
    const std::vector<std::string>& dependencies,
    jsi::Function compute
) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (computed_.find(key) != computed_.end()) {
        throw std::runtime_error("Computed with key '" + key + "' already exists");
    }
    
    // Store compute function
    auto computePtr = std::make_shared<jsi::Function>(std::move(compute));
    computeFns_[key] = computePtr;
    
    // Create computed with wrapped function
    auto computedCore = std::make_unique<ComputedCore>(
        [computePtr](jsi::Runtime& rt) -> jsi::Value {
            return computePtr->call(rt);
        }
    );
    
    // Add dependencies
    for (const auto& depKey : dependencies) {
        auto it = atoms_.find(depKey);
        if (it != atoms_.end()) {
            computedCore->addDependency(it->second.get());
        }
    }
    
    computed_[key] = std::move(computedCore);
}

jsi::Value HybridNitroState::getComputedValue(const std::string& key, jsi::Runtime& rt) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = computed_.find(key);
    if (it == computed_.end()) {
        throw std::runtime_error("Computed with key '" + key + "' not found");
    }
    
    return it->second->get(rt);
}

void HybridNitroState::deleteComputed(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    computed_.erase(key);
    computeFns_.erase(key);
}

// ----- Batch Operations -----

void HybridNitroState::startBatch() {
    BatchManager::instance().startBatch();
}

void HybridNitroState::endBatch() {
    BatchManager::instance().endBatch();
}

// ----- Utility -----

bool HybridNitroState::hasAtom(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return atoms_.find(key) != atoms_.end();
}

std::vector<std::string> HybridNitroState::getAtomKeys() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> keys;
    keys.reserve(atoms_.size());
    for (const auto& [key, _] : atoms_) {
        keys.push_back(key);
    }
    return keys;
}

} // namespace nitrostate
