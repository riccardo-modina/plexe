#ifndef CAMMEMORYMAP_H
#define CAMMEMORYMAP_H

#include <unordered_map>

namespace plexe {

struct MessageInfo {
    int sender;
    double timestamp;
};

class CAMMemoryMap {
private:
    // Key: payload hash (size_t)
    // Value: insertion sender and timestamp (MessageInfo)
    std::unordered_map<size_t, MessageInfo> memoryMap;

public:
    CAMMemoryMap() = default;
    ~CAMMemoryMap() = default;

    // Disable copying for safety
    CAMMemoryMap(const CAMMemoryMap&) = delete;
    CAMMemoryMap& operator=(const CAMMemoryMap&) = delete;

    // Enable moving
    CAMMemoryMap(CAMMemoryMap&&) = default;
    CAMMemoryMap& operator=(CAMMemoryMap&&) = default;

    // Check if the key (hash) exists in the map
    bool contains(size_t key) const {
        return memoryMap.find(key) != memoryMap.end();
    }

    // Add a key (hash) with its sender and insertion timestamp.
    // Returns true if added/updated successfully, false if already present (replay attack).
    bool add(size_t key, int sender, double timestamp) {
        auto it = memoryMap.find(key);
        if (it != memoryMap.end()) {
            return false; // attack: payload hash already present
        }
        memoryMap[key] = {sender, timestamp};
        return true;
    }

    // Garbage collection: remove all entries older than maxAgeCAMReplayDetection seconds relative to currentTime
    void garbageCollect(double currentTime, double maxAgeCAMReplayDetection) {
        for (auto it = memoryMap.begin(); it != memoryMap.end(); ) {
            if ((currentTime - it->second.timestamp) > maxAgeCAMReplayDetection) {
                it = memoryMap.erase(it);
            } else {
                ++it;
            }
        }
    }

    // Clear map
    void clear() {
        memoryMap.clear();
    }

    // Returns the current size of the map (debug/monitoring)
    size_t size() const {
        return memoryMap.size();
    }
};

} 

#endif 