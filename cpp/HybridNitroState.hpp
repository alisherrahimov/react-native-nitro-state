#pragma once

#include "HybridNitroStateSpec.hpp"
#include <NitroModules/AnyMap.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <mutex>
#include "AtomCore.hpp"
#include "ComputedCore.hpp"
#include "BatchManager.hpp"

namespace margelo::nitro::nitrostate {

using namespace margelo::nitro;

/**
 * HybridNitroState - Main JSI binding for the state management library
 * 
 * Implements HybridNitroStateSpec to expose atom/computed operations to JavaScript.
 */
class HybridNitroState : public HybridNitroStateSpec {
public:
    HybridNitroState() : HybridObject(TAG) {}
    ~HybridNitroState() override = default;

    // ----- Atom Operations -----
    void createAtom(const std::string& key, const std::shared_ptr<AnyMap>& initialValue) override;
    std::shared_ptr<AnyMap> getAtomValue(const std::string& key) override;
    void setAtomValue(const std::string& key, const std::shared_ptr<AnyMap>& value) override;
    std::function<void()> subscribeAtom(const std::string& key, const std::function<void()>& callback) override;
    void deleteAtom(const std::string& key) override;

    // ----- Computed Operations -----
    void createComputed(
        const std::string& key,
        const std::vector<std::string>& dependencies,
        const std::function<std::shared_ptr<Promise<std::shared_ptr<AnyMap>>>()>& compute
    ) override;
    std::shared_ptr<AnyMap> getComputedValue(const std::string& key) override;
    void deleteComputed(const std::string& key) override;

    // ----- Batch Operations -----
    void startBatch() override;
    void endBatch() override;

    // ----- Utility -----
    bool hasAtom(const std::string& key) override;
    std::vector<std::string> getAtomKeys() override;

private:
    std::unordered_map<std::string, std::shared_ptr<AnyMap>> atoms_;
    std::unordered_map<std::string, std::shared_ptr<AnyMap>> computed_;
    std::unordered_map<std::string, std::function<std::shared_ptr<Promise<std::shared_ptr<AnyMap>>>()>> computeFns_;
    std::unordered_map<std::string, std::vector<std::pair<size_t, std::function<void()>>>> subscribers_;
    size_t nextSubscriberId_ = 0;
    mutable std::mutex mutex_;
    bool isBatching_ = false;
    std::vector<std::string> pendingNotifications_;
};

} // namespace margelo::nitro::nitrostate
