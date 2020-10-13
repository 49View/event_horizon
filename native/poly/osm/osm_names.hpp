//
// Created by dado on 12/10/2020.
//

#pragma once

#include <string>

namespace OSMElementName {
    [[maybe_unused]] static inline std::string unclassified() {
        return "unclassified";
    }
    [[maybe_unused]] static inline std::string road() {
        return "road";
    }
    [[maybe_unused]] static inline std::string park() {
        return "park";
    }
    [[maybe_unused]] static inline std::string parking() {
        return "parking";
    }
    [[maybe_unused]] static inline std::string water() {
        return "water";
    }

    [[maybe_unused]] static inline std::string building() {
        return "building";
    }
    [[maybe_unused]] static inline std::string tree() {
        return "tree";
    }

    // Barrier
    [[maybe_unused]] static inline std::string barrier() {
        return "barrier";
    }
    [[maybe_unused]] static inline std::string bollard() {
        return "bollard";
    }
    [[maybe_unused]] static inline std::string fence() {
        return "fence";
    }
    [[maybe_unused]] static inline std::string gate() {
        return "gate";
    }
    [[maybe_unused]] static inline std::string kerb() {
        return "kerb";
    }
    [[maybe_unused]] static inline std::string retaining_wall() {
        return "retaining_wall";
    }
    [[maybe_unused]] static inline std::string wall() {
        return "wall";
    }
}
